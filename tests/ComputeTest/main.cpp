#include <Nazara/Core.hpp>
#include <Nazara/Math.hpp>
#include <Nazara/Platform.hpp>
#include <Nazara/Renderer.hpp>
#include <NZSL/FilesystemModuleResolver.hpp>
#include <NZSL/LangWriter.hpp>
#include <NZSL/Parser.hpp>
#include <iostream>
#include <thread>

NAZARA_REQUEST_DEDICATED_GPU()

struct SpriteRenderData
{
	std::shared_ptr<Nz::RenderBuffer> vertexBuffer;
	Nz::ShaderBindingPtr shaderBinding;
};

struct SpriteRenderPipeline
{
	std::shared_ptr<Nz::RenderPipeline> pipeline;
	std::shared_ptr<Nz::RenderPipelineLayout> pipelineLayout;

};

std::shared_ptr<Nz::ComputePipeline> BuildComputePipeline(Nz::RenderDevice& device, std::shared_ptr<Nz::RenderPipelineLayout> pipelineLayout, std::shared_ptr<nzsl::ModuleResolver> moduleResolver);
SpriteRenderPipeline BuildSpritePipeline(Nz::RenderDevice& device);
SpriteRenderData BuildSpriteData(Nz::RenderDevice& device, const SpriteRenderPipeline& pipelineData, const Nz::Rectf& textureRect, const Nz::Vector2f& screenSize, const Nz::Texture& texture, const Nz::TextureSampler& sampler);

int main()
{
	std::filesystem::path resourceDir = "assets/examples";
	if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory("../.." / resourceDir))
		resourceDir = "../.." / resourceDir;

	Nz::Renderer::Config rendererConfig;
	std::cout << "Run using Vulkan? (y/n)" << std::endl;
	if (std::getchar() == 'y')
		rendererConfig.preferredAPI = Nz::RenderAPI::Vulkan;
	else
		rendererConfig.preferredAPI = Nz::RenderAPI::OpenGL;

	Nz::Modules<Nz::Renderer> nazara(rendererConfig);

	Nz::RenderDeviceFeatures enabledFeatures;
	enabledFeatures.computeShaders = true;
	enabledFeatures.textureReadWrite = true;

	std::shared_ptr<Nz::RenderDevice> device = Nz::Renderer::Instance()->InstanciateRenderDevice(0, enabledFeatures);

	// Source texture
	Nz::TextureParams texParams;
	texParams.renderDevice = device;
	texParams.loadFormat = Nz::PixelFormat::RGBA8;
	texParams.usageFlags = Nz::TextureUsage::ShaderReadWrite | Nz::TextureUsage::ShaderSampling | Nz::TextureUsage::TransferDestination;
	texParams.buildMipmaps = false;

	std::shared_ptr<Nz::Texture> texture = Nz::Texture::LoadFromFile(resourceDir / "lynix.jpg", texParams);

	// Destination texture
	Nz::TextureInfo destTexParams = texture->GetTextureInfo();
	destTexParams.usageFlags = Nz::TextureUsage::ShaderReadWrite | Nz::TextureUsage::ShaderSampling;

	std::shared_ptr<Nz::Texture> targetTexture = device->InstantiateTexture(destTexParams);
	std::shared_ptr<Nz::TextureSampler> textureSampler = device->InstantiateTextureSampler({});

	// Compute part
	Nz::RenderPipelineLayoutInfo computePipelineLayoutInfo;
	computePipelineLayoutInfo.bindings.assign({
		{
			0, 0, 1,
			Nz::ShaderBindingType::Texture,
			nzsl::ShaderStageType::Compute
		},
		{
			0, 1, 1,
			Nz::ShaderBindingType::Texture,
			nzsl::ShaderStageType::Compute
		},
	});

	std::shared_ptr<Nz::RenderPipelineLayout> computePipelineLayout = device->InstantiateRenderPipelineLayout(computePipelineLayoutInfo);

	std::shared_ptr<nzsl::FilesystemModuleResolver> moduleResolver = std::make_shared<nzsl::FilesystemModuleResolver>();
	moduleResolver->RegisterDirectory(resourceDir / "../shaders/", true);

	std::shared_ptr<Nz::ComputePipeline> computePipeline;
	try
	{
		computePipeline = BuildComputePipeline(*device, computePipelineLayout, moduleResolver);
	}
	catch (const std::exception& e)
	{
		std::cerr << "failed to compile compute shaders: " << e.what() << std::endl;
		std::abort();
	}
	std::atomic_bool hasNewPipeline = false;

	Nz::ShaderBindingPtr computeBinding = computePipelineLayout->AllocateShaderBinding(0);
	computeBinding->Update({
		{
			0,
			Nz::ShaderBinding::TextureBinding {
				texture.get(),
				Nz::TextureAccess::ReadOnly
			}
		},
		{
			1,
			Nz::ShaderBinding::TextureBinding {
				targetTexture.get(),
				Nz::TextureAccess::WriteOnly
			}
		}
	});

	moduleResolver->OnModuleUpdated.Connect([&](nzsl::ModuleResolver*, const std::string& moduleName)
	{
		std::cout << moduleName << " has been updated" << std::endl;
		hasNewPipeline = true;
	});

	std::string windowTitle = "Compute test";
	Nz::Window window;
	if (!window.Create(Nz::VideoMode(1280, 720), windowTitle))
	{
		std::cout << "Failed to create Window" << std::endl;
		std::abort();
	}
	Nz::WindowSwapchain windowSwapchain(device, window);

	Nz::Vector2ui windowSize = window.GetSize();
	constexpr float textureSize = 512.f;
	float margin = (windowSize.y - textureSize) * 0.5f;

	SpriteRenderPipeline spriteRenderPipeline = BuildSpritePipeline(*device);
	SpriteRenderData spriteRenderData1 = BuildSpriteData(*device, spriteRenderPipeline, Nz::Rectf(margin, windowSize.y - margin - textureSize, textureSize, textureSize), Nz::Vector2f(windowSize), *texture, *textureSampler);
	SpriteRenderData spriteRenderData2 = BuildSpriteData(*device, spriteRenderPipeline, Nz::Rectf(windowSize.x - textureSize - margin, windowSize.y - margin - textureSize, textureSize, textureSize), Nz::Vector2f(windowSize), *targetTexture, *textureSampler);

	Nz::MillisecondClock fpsClock;
	unsigned int fps = 0;

	while (window.IsOpen())
	{
		window.ProcessEvents();

		Nz::RenderFrame frame = windowSwapchain.AcquireFrame();
		if (!frame)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			continue;
		}

		if (hasNewPipeline)
		{
			try
			{
				hasNewPipeline = false;
				std::shared_ptr<Nz::ComputePipeline> newComputePipeline = BuildComputePipeline(*device, computePipelineLayout, moduleResolver);
				frame.PushForRelease(std::move(computePipeline));
				computePipeline = std::move(newComputePipeline);
			}
			catch (const std::exception& e)
			{
				std::cerr << e.what() << std::endl;
			}
		}

		const Nz::WindowSwapchain* windowRT = &windowSwapchain;
		frame.Execute([&](Nz::CommandBufferBuilder& builder)
		{
			builder.BeginDebugRegion("Compute part", Nz::Color::Blue());
			{
				builder.TextureBarrier(Nz::PipelineStage::FragmentShader, Nz::PipelineStage::ComputeShader, Nz::MemoryAccess::ShaderRead, Nz::MemoryAccess::ShaderRead, Nz::TextureLayout::ColorInput, Nz::TextureLayout::General, *texture);
				builder.TextureBarrier(Nz::PipelineStage::FragmentShader, Nz::PipelineStage::ComputeShader, Nz::MemoryAccess::ShaderRead, Nz::MemoryAccess::ShaderWrite, Nz::TextureLayout::Undefined, Nz::TextureLayout::General, *targetTexture);

				builder.BindComputePipeline(*computePipeline);
				builder.BindComputeShaderBinding(0, *computeBinding);
				builder.Dispatch(destTexParams.width / 32, destTexParams.height / 32, 1);

				builder.TextureBarrier(Nz::PipelineStage::ComputeShader, Nz::PipelineStage::FragmentShader, Nz::MemoryAccess::ShaderRead, Nz::MemoryAccess::ShaderRead, Nz::TextureLayout::General, Nz::TextureLayout::ColorInput, *texture);
				builder.TextureBarrier(Nz::PipelineStage::ComputeShader, Nz::PipelineStage::FragmentShader, Nz::MemoryAccess::ShaderWrite, Nz::MemoryAccess::ShaderRead, Nz::TextureLayout::General, Nz::TextureLayout::ColorInput, *targetTexture);
			}
			builder.EndDebugRegion();

			builder.BeginDebugRegion("Main window rendering", Nz::Color::Green());
			{
				Nz::Recti renderRect(0, 0, window.GetSize().x, window.GetSize().y);

				Nz::CommandBufferBuilder::ClearValues clearValues[2];
				clearValues[0].color = Nz::Color::Gray();
				clearValues[1].depth = 1.f;
				clearValues[1].stencil = 0;

				builder.BeginRenderPass(windowRT->GetFramebuffer(frame.GetImageIndex()), windowRT->GetRenderPass(), renderRect, { clearValues[0], clearValues[1] });
				{
					builder.SetScissor(Nz::Recti{ 0, 0, int(windowSize.x), int(windowSize.y) });
					builder.SetViewport(Nz::Recti{ 0, 0, int(windowSize.x), int(windowSize.y) });

					builder.BindRenderPipeline(*spriteRenderPipeline.pipeline);

					builder.BindVertexBuffer(0, *spriteRenderData1.vertexBuffer);
					builder.BindRenderShaderBinding(0, *spriteRenderData1.shaderBinding);
					builder.Draw(4);

					builder.BindVertexBuffer(0, *spriteRenderData2.vertexBuffer);
					builder.BindRenderShaderBinding(0, *spriteRenderData2.shaderBinding);
					builder.Draw(4);
				}
				builder.EndRenderPass();
			}
			builder.EndDebugRegion();

		}, Nz::QueueType::Graphics);

		frame.Present();

		fps++;

		if (fpsClock.RestartIfOver(Nz::Time::Second()))
		{
			window.SetTitle(windowTitle + " - " + Nz::NumberToString(fps) + " FPS");
			fps = 0;
		}
	}

	return EXIT_SUCCESS;
}

std::shared_ptr<Nz::ComputePipeline> BuildComputePipeline(Nz::RenderDevice& device, std::shared_ptr<Nz::RenderPipelineLayout> pipelineLayout, std::shared_ptr<nzsl::ModuleResolver> moduleResolver)
{
	nzsl::Ast::ModulePtr shaderModule = moduleResolver->Resolve("Compute.Sepia");
	if (!shaderModule)
	{
		std::cout << "Failed to parse shader module" << std::endl;
		std::abort();
	}

	nzsl::BackendParameters parameters;
	parameters.backendPasses |= nzsl::BackendPass::Optimize;

	auto computeShader = device.InstantiateShaderModule(nzsl::ShaderStageType::Compute, *shaderModule, parameters);
	if (!computeShader)
	{
		std::cout << "Failed to instantiate shader" << std::endl;
		std::abort();
	}

	Nz::ComputePipelineInfo computePipelineInfo;
	computePipelineInfo.pipelineLayout = pipelineLayout;
	computePipelineInfo.shaderModule = computeShader;

	std::shared_ptr<Nz::ComputePipeline> pipeline = device.InstantiateComputePipeline(computePipelineInfo);
	if (!pipeline)
	{
		std::cout << "Failed to instantiate compute pipeline" << std::endl;
		std::abort();
	}

	return pipeline;
}

const char fragVertSource[] = R"(
[nzsl_version("1.0")]
module;

external
{
	[binding(0)] texture: sampler2D[f32]
}

struct FragOut
{
	[location(0)] color: vec4[f32]
}

struct VertIn
{
	[location(0)] pos: vec2[f32],
	[location(1)] uv: vec2[f32]
}

struct VertOut
{
	[location(0)] uv: vec2[f32],
	[builtin(position)] pos: vec4[f32]
}

[entry(frag)]
fn main(input: VertOut) -> FragOut
{
	let output: FragOut;
	output.color = texture.Sample(input.uv);

	return output;
}

[entry(vert)]
fn main(input: VertIn) -> VertOut
{
	let output: VertOut;
	output.pos = vec4[f32](input.pos, 0.0, 1.0);
	output.uv = input.uv;
	return output;
}
)";

SpriteRenderPipeline BuildSpritePipeline(Nz::RenderDevice& device)
{
	try
	{
		nzsl::Ast::ModulePtr shaderModule = nzsl::Parse(std::string_view(fragVertSource, sizeof(fragVertSource)));
		if (!shaderModule)
		{
			std::cout << "Failed to parse shader module" << std::endl;
			std::abort();
		}

		nzsl::BackendParameters states;
		states.backendPasses |= nzsl::BackendPass::Optimize;

		auto fragVertShader = device.InstantiateShaderModule(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, *shaderModule, states);
		if (!fragVertShader)
		{
			std::cout << "Failed to instantiate shader" << std::endl;
			std::abort();
		}

		std::shared_ptr<Nz::VertexDeclaration> vertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout::XY_UV);

		SpriteRenderPipeline pipelineData;

		Nz::RenderPipelineLayoutInfo pipelineLayoutInfo;
		pipelineLayoutInfo.bindings.push_back({
			0, 0, 1,
			Nz::ShaderBindingType::Sampler,
			nzsl::ShaderStageType::Fragment
		});

		pipelineData.pipelineLayout = device.InstantiateRenderPipelineLayout(std::move(pipelineLayoutInfo));

		Nz::RenderPipelineInfo pipelineInfo;
		pipelineInfo.primitiveMode = Nz::PrimitiveMode::TriangleStrip;
		pipelineInfo.pipelineLayout = pipelineData.pipelineLayout;
		pipelineInfo.shaderModules.push_back(fragVertShader);
		pipelineInfo.vertexBuffers.push_back({
			0, vertexDeclaration
		});

		pipelineData.pipeline = device.InstantiateRenderPipeline(std::move(pipelineInfo));

		return pipelineData;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		std::abort();
	}
}

SpriteRenderData BuildSpriteData(Nz::RenderDevice& device, const SpriteRenderPipeline& pipelineData, const Nz::Rectf& textureRect, const Nz::Vector2f& screenSize, const Nz::Texture& texture, const Nz::TextureSampler& sampler)
{
	try
	{
		auto ToClipSpace = [&](Nz::Vector2f pos) -> Nz::Vector2f
		{
			// From 0..size to 0..1
			pos /= screenSize;
			// From 0..1 to -1..1
			pos *= 2.f;
			pos -= Nz::Vector2f(1.f, 1.f);
			// Reverse Y
			pos.y = -pos.y;
			return pos;
		};

		std::array<Nz::VertexStruct_XY_UV, 4> pos;
		pos[0].position = ToClipSpace(textureRect.GetCorner(Nz::RectCorner::LeftTop));
		pos[0].uv = Nz::Vector2f(0.f, 0.f);
		pos[1].position = ToClipSpace(textureRect.GetCorner(Nz::RectCorner::LeftBottom));
		pos[1].uv = Nz::Vector2f(0.f, 1.f);
		pos[2].position = ToClipSpace(textureRect.GetCorner(Nz::RectCorner::RightTop));
		pos[2].uv = Nz::Vector2f(1.f, 0.f);
		pos[3].position = ToClipSpace(textureRect.GetCorner(Nz::RectCorner::RightBottom));
		pos[3].uv = Nz::Vector2f(1.f, 1.f);

		SpriteRenderData renderData;
		renderData.vertexBuffer = device.InstantiateBuffer(Nz::BufferType::Vertex, 4 * 4 * sizeof(float), Nz::BufferUsage::DeviceLocal, pos.data());

		renderData.shaderBinding = pipelineData.pipelineLayout->AllocateShaderBinding(0);
		renderData.shaderBinding->Update({
			{
				0,
				Nz::ShaderBinding::SampledTextureBinding {
					&texture, &sampler
				}
			}
		});

		return renderData;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		std::abort();
	}
}
