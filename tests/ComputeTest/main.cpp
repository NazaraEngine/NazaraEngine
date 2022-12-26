#include <Nazara/Core.hpp>
#include <Nazara/Math.hpp>
#include <Nazara/Platform.hpp>
#include <Nazara/Renderer.hpp>
#include <NZSL/LangWriter.hpp>
#include <NZSL/Parser.hpp>
#include <Nazara/Utility.hpp>
#include <array>
#include <chrono>
#include <iostream>
#include <thread>

NAZARA_REQUEST_DEDICATED_GPU()

struct ComputePipeline
{
	std::shared_ptr<Nz::RenderPipelineLayout> layout;
	std::shared_ptr<Nz::ComputePipeline> pipeline;
};

struct SpriteRenderData
{
	std::shared_ptr<Nz::RenderBuffer> vertexBuffer;
	std::shared_ptr<Nz::ShaderBinding> shaderBinding;
};

struct SpriteRenderPipeline
{
	std::shared_ptr<Nz::RenderPipeline> pipeline;
	std::shared_ptr<Nz::RenderPipelineLayout> pipelineLayout;

};

ComputePipeline BuildComputePipeline(Nz::RenderDevice& device);
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

	std::shared_ptr<Nz::Texture> texture = Nz::Texture::LoadFromFile(resourceDir / "lynix.jpg", texParams);

	// Destination texture
	Nz::TextureInfo destTexParams = texture->GetTextureInfo();
	destTexParams.usageFlags = Nz::TextureUsage::ShaderReadWrite | Nz::TextureUsage::ShaderSampling;

	std::shared_ptr<Nz::Texture> targetTexture = device->InstantiateTexture(destTexParams);
	std::shared_ptr<Nz::TextureSampler> textureSampler = device->InstantiateTextureSampler({});

	// Compute part
	ComputePipeline computePipeline = BuildComputePipeline(*device);
	std::shared_ptr<Nz::ShaderBinding> computeBinding = computePipeline.layout->AllocateShaderBinding(0);
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

	std::string windowTitle = "Compute test";

	Nz::RenderWindow window;
	if (!window.Create(device, Nz::VideoMode(1280, 720, 32), windowTitle))
	{
		std::cout << "Failed to create Window" << std::endl;
		std::abort();
	}

	Nz::Vector2ui windowSize = window.GetSize();
	constexpr float textureSize = 512.f;
	float margin = (windowSize.y - textureSize) * 0.5f;

	SpriteRenderPipeline spriteRenderPipeline = BuildSpritePipeline(*device);
	SpriteRenderData spriteRenderData1 = BuildSpriteData(*device, spriteRenderPipeline, Nz::Rectf(margin, windowSize.y - margin - textureSize, textureSize, textureSize), Nz::Vector2f(windowSize), *texture, *textureSampler);
	SpriteRenderData spriteRenderData2 = BuildSpriteData(*device, spriteRenderPipeline, Nz::Rectf(windowSize.x - textureSize - margin, windowSize.y - margin - textureSize, textureSize, textureSize), Nz::Vector2f(windowSize), *targetTexture, *textureSampler);

	Nz::Clock secondClock;
	unsigned int fps = 0;

	while (window.IsOpen())
	{
		window.ProcessEvents();

		Nz::RenderFrame frame = window.AcquireFrame();
		if (!frame)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			continue;
		}

		const Nz::RenderTarget* windowRT = window.GetRenderTarget();
		frame.Execute([&](Nz::CommandBufferBuilder& builder)
		{
			builder.BeginDebugRegion("Compute part", Nz::Color::Blue);
			{
				builder.TextureBarrier(Nz::PipelineStage::FragmentShader, Nz::PipelineStage::ComputeShader, Nz::MemoryAccess::ShaderRead, Nz::MemoryAccess::ShaderRead, Nz::TextureLayout::ColorInput, Nz::TextureLayout::General, *texture);
				builder.TextureBarrier(Nz::PipelineStage::FragmentShader, Nz::PipelineStage::ComputeShader, Nz::MemoryAccess::ShaderRead, Nz::MemoryAccess::ShaderWrite, Nz::TextureLayout::Undefined, Nz::TextureLayout::General, *targetTexture);

				builder.BindComputePipeline(*computePipeline.pipeline);
				builder.BindComputeShaderBinding(0, *computeBinding);
				builder.Dispatch(destTexParams.width / 32, destTexParams.height / 32, 1);

				builder.TextureBarrier(Nz::PipelineStage::ComputeShader, Nz::PipelineStage::FragmentShader, Nz::MemoryAccess::ShaderRead, Nz::MemoryAccess::ShaderRead, Nz::TextureLayout::General, Nz::TextureLayout::ColorInput, *texture);
				builder.TextureBarrier(Nz::PipelineStage::ComputeShader, Nz::PipelineStage::FragmentShader, Nz::MemoryAccess::ShaderWrite, Nz::MemoryAccess::ShaderRead, Nz::TextureLayout::General, Nz::TextureLayout::ColorInput, *targetTexture);
			}
			builder.EndDebugRegion();

			builder.BeginDebugRegion("Main window rendering", Nz::Color::Green);
			{
				Nz::Recti renderRect(0, 0, window.GetSize().x, window.GetSize().y);

				Nz::CommandBufferBuilder::ClearValues clearValues[2];
				clearValues[0].color = Nz::Color::Gray;
				clearValues[1].depth = 1.f;
				clearValues[1].stencil = 0;

				builder.BeginRenderPass(windowRT->GetFramebuffer(frame.GetFramebufferIndex()), windowRT->GetRenderPass(), renderRect, { clearValues[0], clearValues[1] });
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

		if (secondClock.GetMilliseconds() >= 1000)
		{
			window.SetTitle(windowTitle + " - " + Nz::NumberToString(fps) + " FPS");

			fps = 0;

			secondClock.Restart();
		}
	}

	return EXIT_SUCCESS;
}

// Edge detection, translated to NZSL from Sascha Willems compute shader example
const char computeSource[] = R"(
[nzsl_version("1.0")]
module;

external
{
	[binding(0)] input_tex: texture2D[f32, readonly, rgba8],
	[binding(1)] output_tex: texture2D[f32, writeonly, rgba8]
}

struct Input
{
	[builtin(global_invocation_indices)] global_invocation_id: vec3[u32]
}

[entry(compute)]
[workgroup(32, 32, 1)]
fn main(input: Input)
{
	let indices = vec2[i32](input.global_invocation_id.xy);

	// Fetch neighbouring texels
	let avg: array[f32, 9];

	let n = 0;
	[unroll]
	for i in -1 -> 2
	{
		[unroll]
		for j in -1 -> 2
		{
			let rgb = input_tex.Read(indices + vec2[i32](i, j)).rgb;
			avg[n] = (rgb.r + rgb.b + rgb.b) / 3.0;
			n += 1;
		}
	}

	let kernel: array[f32, 9];
	[unroll]
	for i in 0 -> 9
	{
		if (i == 4)
			kernel[i] = 1.0;
		else
			kernel[i] = -1.0/8.0;
	}

	let res = vec4[f32](conv(kernel, avg, 0.1, 0.0).rrr, 1.0);
	output_tex.Write(indices, res);
}

fn conv(kernel: array[f32, 9], data: array[f32, 9], denom: f32, offset: f32) -> f32
{
	let res = 0.0;
	[unroll]
	for i in 0 -> 9
		res += kernel[i] * data[i];

	return clamp(res/denom + offset, 0.0, 1.0);
}
)";

ComputePipeline BuildComputePipeline(Nz::RenderDevice& device)
{
	try
	{
		nzsl::Ast::ModulePtr shaderModule = nzsl::Parse(std::string_view(computeSource, sizeof(computeSource)));
		if (!shaderModule)
		{
			std::cout << "Failed to parse shader module" << std::endl;
			std::abort();
		}

		nzsl::ShaderWriter::States states;
		states.optimize = true;

		auto computeShader = device.InstantiateShaderModule(nzsl::ShaderStageType::Compute, *shaderModule, states);
		if (!computeShader)
		{
			std::cout << "Failed to instantiate shader" << std::endl;
			std::abort();
		}

		Nz::RenderPipelineLayoutInfo computePipelineLayoutInfo;

		auto& inputBinding = computePipelineLayoutInfo.bindings.emplace_back();
		inputBinding.setIndex = 0;
		inputBinding.bindingIndex = 0;
		inputBinding.shaderStageFlags = nzsl::ShaderStageType::Compute;
		inputBinding.type = Nz::ShaderBindingType::Texture;

		auto& outputBinding = computePipelineLayoutInfo.bindings.emplace_back();
		outputBinding.setIndex = 0;
		outputBinding.bindingIndex = 1;
		outputBinding.shaderStageFlags = nzsl::ShaderStageType::Compute;
		outputBinding.type = Nz::ShaderBindingType::Texture;

		std::shared_ptr<Nz::RenderPipelineLayout> pipelineLayout = device.InstantiateRenderPipelineLayout(computePipelineLayoutInfo);

		Nz::ComputePipelineInfo computePipelineInfo;
		computePipelineInfo.pipelineLayout = pipelineLayout;
		computePipelineInfo.shaderModule = computeShader;

		std::shared_ptr<Nz::ComputePipeline> pipeline = device.InstantiateComputePipeline(computePipelineInfo);
		if (!pipeline)
		{
			std::cout << "Failed to instantiate compute pipeline" << std::endl;
			std::abort();
		}

		ComputePipeline result;
		result.layout = std::move(pipelineLayout);
		result.pipeline = std::move(pipeline);
		return result;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		std::abort();
	}
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

		nzsl::ShaderWriter::States states;
		states.optimize = true;

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
		pos[0].position = ToClipSpace(textureRect.GetCorner(Nz::RectCorner::LeftBottom));
		pos[0].uv = Nz::Vector2f(0.f, 0.f);
		pos[1].position = ToClipSpace(textureRect.GetCorner(Nz::RectCorner::LeftTop));
		pos[1].uv = Nz::Vector2f(0.f, 1.f);
		pos[2].position = ToClipSpace(textureRect.GetCorner(Nz::RectCorner::RightBottom));
		pos[2].uv = Nz::Vector2f(1.f, 0.f);
		pos[3].position = ToClipSpace(textureRect.GetCorner(Nz::RectCorner::RightTop));
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
