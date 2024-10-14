#include <Nazara/Core.hpp>
#include <Nazara/Math.hpp>
#include <Nazara/Platform.hpp>
#include <Nazara/Renderer.hpp>
#include <NZSL/FilesystemModuleResolver.hpp>
#include <NZSL/LangWriter.hpp>
#include <NZSL/Parser.hpp>
#include <NZSL/Math/FieldOffsets.hpp>
#include <array>
#include <chrono>
#include <iostream>
#include <random>
#include <thread>

NAZARA_REQUEST_DEDICATED_GPU()

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

std::shared_ptr<Nz::ComputePipeline> BuildComputePipeline(Nz::RenderDevice& device, std::shared_ptr<Nz::RenderPipelineLayout> pipelineLayout, std::shared_ptr<nzsl::ModuleResolver> moduleResolver);
SpriteRenderPipeline BuildSpritePipeline(Nz::RenderDevice& device);
SpriteRenderData BuildSpriteData(Nz::RenderDevice& device, const SpriteRenderPipeline& pipelineData, const Nz::Rectf& textureRect, const Nz::Vector2f& screenSize, const Nz::RenderBufferView& buffer, const Nz::RenderBufferView& particleBuffer, std::shared_ptr<Nz::Texture> texture, std::shared_ptr<Nz::TextureSampler> sampler);
std::shared_ptr<Nz::Texture> GenerateSpriteTexture(Nz::RenderDevice& device, std::shared_ptr<nzsl::ModuleResolver> moduleResolver, Nz::WindowSwapchain& swapchain);

int main()
{
	Nz::Vector2ui windowSize = { 1920, 1080 };

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
	enabledFeatures.storageBuffers = true;
	enabledFeatures.textureReadWrite = true;

	std::shared_ptr<Nz::RenderDevice> device = Nz::Renderer::Instance()->InstanciateRenderDevice(0, enabledFeatures);

	nzsl::FieldOffsets particleLayout(nzsl::StructLayout::Std140);
	std::size_t particleColorOffset = particleLayout.AddField(nzsl::StructFieldType::Float3);
	std::size_t particlePosOffset = particleLayout.AddField(nzsl::StructFieldType::Float2);
	std::size_t particleTargetPosOffset = particleLayout.AddField(nzsl::StructFieldType::Float2);
	std::size_t particleVelOffset = particleLayout.AddField(nzsl::StructFieldType::Float2);

	std::size_t particleSize = particleLayout.GetAlignedSize();

	std::shared_ptr<Nz::Image> logo = Nz::Image::LoadFromFile(resourceDir / "Logo.png");
	if (!logo)
	{
		std::cerr << "failed to load logo" << std::endl;
		return EXIT_FAILURE;
	}

	std::vector<std::pair<Nz::Vector2ui, Nz::Color>> logoParticles;
	for (unsigned int y = 0; y < logo->GetHeight(); ++y)
	{
		for (unsigned int x = 0; x < logo->GetWidth(); ++x)
		{
			Nz::Color color = logo->GetPixelColor(x, y);
			if (color.a == 0)
				continue;

			logoParticles.push_back({
				{ x, y },
				color
			});
		}
	}

	std::size_t particleCount = logoParticles.size();

	nzsl::FieldOffsets bufferLayout(nzsl::StructLayout::Std140);
	std::size_t particleCountOffset = bufferLayout.AddField(nzsl::StructFieldType::UInt1);
	std::size_t particlesArrayOffset = bufferLayout.AddStructArray(particleLayout, particleCount);

	std::size_t bufferSize = bufferLayout.GetAlignedSize();

	std::vector<Nz::UInt8> particleBufferInitialData(bufferSize);
	Nz::AccessByOffset<Nz::UInt32&>(particleBufferInitialData.data(), particleCountOffset) = Nz::SafeCast<Nz::UInt32>(particleCount);

	Nz::Vector2f logoImageSize(Nz::Vector2ui(logo->GetSize()));
	float logoRatio = logoImageSize.x / logoImageSize.y;

	Nz::Vector2f logoSize = Nz::Vector2f(windowSize.x * 0.8f, windowSize.x * 0.8f / logoRatio);

	// Center the logo in the canvas
	Nz::Vector2f posScale = logoSize / logoImageSize;
	Nz::Vector2f posOffset = (Nz::Vector2f(windowSize) - logoSize) * 0.5f;

	// from image space to world space (topleft Y down to bottomleft Y up)
	posScale.y *= -1.f;
	posOffset.y += logoSize.y;

	// Build particles
	std::mt19937 rand(std::random_device{}());
	std::uniform_real_distribution<float> velDis(-500.f, 500.f);

	Nz::UInt8* particleBasePtr = particleBufferInitialData.data() + particlesArrayOffset;
	Nz::SparsePtr<Nz::Vector2f> particlePosPtr(particleBasePtr + particlePosOffset, particleSize);
	Nz::SparsePtr<Nz::Vector2f> particleTargetPosPtr(particleBasePtr + particleTargetPosOffset, particleSize);
	Nz::SparsePtr<Nz::Vector3f> particleColorPtr(particleBasePtr + particleColorOffset, particleSize);
	Nz::SparsePtr<Nz::Vector2f> particleVelPtr(particleBasePtr + particleVelOffset, particleSize);
	for (std::size_t i = 0; i < particleCount; ++i)
	{
		auto&& [pos, color] = logoParticles[i];

		particleTargetPosPtr[i] = posScale * Nz::Vector2f(pos) + posOffset;
		particlePosPtr[i] = particleTargetPosPtr[i];
		particleColorPtr[i] = Nz::Vector3f(color.r, color.g, color.b) * color.a;
		particleVelPtr[i] = Nz::Vector2f(velDis(rand), velDis(rand));
	}

	std::shared_ptr<Nz::RenderBuffer> particleBuffer = device->InstantiateBuffer(Nz::BufferType::Storage, bufferSize, Nz::BufferUsage::DeviceLocal | Nz::BufferUsage::DirectMapping | Nz::BufferUsage::Read | Nz::BufferUsage::Write, particleBufferInitialData.data());

	nzsl::FieldOffsets sceneBufferLayout(nzsl::StructLayout::Std140);
	std::size_t deltaTimeOffset = sceneBufferLayout.AddField(nzsl::StructFieldType::Float1);
	std::size_t mousePosOffset = sceneBufferLayout.AddField(nzsl::StructFieldType::Float2);
	std::size_t effectRadiusOffset = sceneBufferLayout.AddField(nzsl::StructFieldType::Float1);

	std::size_t sceneBufferSize = sceneBufferLayout.GetAlignedSize();

	std::shared_ptr<Nz::RenderBuffer> sceneDataBuffer = device->InstantiateBuffer(Nz::BufferType::Uniform, sceneBufferSize, Nz::BufferUsage::DeviceLocal | Nz::BufferUsage::Dynamic);

	// Compute part
	Nz::RenderPipelineLayoutInfo computePipelineLayoutInfo;
	computePipelineLayoutInfo.bindings.assign({
		{
			0, 0, 1,
			Nz::ShaderBindingType::StorageBuffer,
			nzsl::ShaderStageType::Compute
		},
		{
			0, 1, 1,
			Nz::ShaderBindingType::UniformBuffer,
			nzsl::ShaderStageType::Compute
		}
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

	std::shared_ptr<Nz::ShaderBinding> computeBinding = computePipelineLayout->AllocateShaderBinding(0);
	computeBinding->Update({
		{
			0,
			Nz::ShaderBinding::StorageBufferBinding {
				particleBuffer.get(), 0, bufferSize
			}
		},
		{
			1,
			Nz::ShaderBinding::UniformBufferBinding {
				sceneDataBuffer.get(), 0, sceneBufferSize
			}
		}
	});

	moduleResolver->OnModuleUpdated.Connect([&](nzsl::ModuleResolver*, const std::string& moduleName)
	{
		std::cout << moduleName << " has been updated" << std::endl;
		hasNewPipeline = true;
	});

	std::string windowTitle = "Particle test (" + std::to_string(particleCount) + " particles)";
	Nz::Window window;
	if (!window.Create(Nz::VideoMode(windowSize.x, windowSize.y), windowTitle))
	{
		std::cout << "Failed to create Window" << std::endl;
		std::abort();
	}
	Nz::WindowSwapchain windowSwapchain(device, window);

	nzsl::FieldOffsets viewerLayout(nzsl::StructLayout::Std140);
	std::size_t projectionMatrixOffset = viewerLayout.AddMatrix(nzsl::StructFieldType::Float1, 4, 4, true);

	std::size_t viewerBufferSize = viewerLayout.GetAlignedSize();

	std::vector<Nz::UInt8> viewerBufferInitialData(viewerBufferSize);
	Nz::AccessByOffset<Nz::Matrix4f&>(viewerBufferInitialData.data(), projectionMatrixOffset) = Nz::Matrix4f::Ortho(0.f, float(windowSize.x), 0.f, float(windowSize.y));

	std::shared_ptr<Nz::RenderBuffer> uniformBuffer = device->InstantiateBuffer(Nz::BufferType::Uniform, viewerBufferSize, Nz::BufferUsage::DeviceLocal, viewerBufferInitialData.data());

	std::shared_ptr<Nz::Texture> texture = GenerateSpriteTexture(*device, moduleResolver, windowSwapchain);
	std::shared_ptr<Nz::TextureSampler> textureSampler = device->InstantiateTextureSampler({});

	SpriteRenderPipeline spriteRenderPipeline = BuildSpritePipeline(*device);
	SpriteRenderData spriteRenderData1 = BuildSpriteData(*device, spriteRenderPipeline, Nz::Rectf(-8.f, -8.f, 16.f, 16.f), Nz::Vector2f(windowSize), uniformBuffer.get(), particleBuffer.get(), texture, textureSampler);

	Nz::MillisecondClock fpsClock;
	Nz::HighPrecisionClock updateClock;
	unsigned int fps = 0;

	// Smooth mouse position over time
	Nz::Vector2f previousMousePos;
	Nz::Vector2f newMousePos;
	{
		Nz::Vector2i mousePos = Nz::Mouse::GetPosition(window);
		previousMousePos = Nz::Vector2f(mousePos.x, windowSize.y - mousePos.y);
		newMousePos = previousMousePos;
	}

	Nz::Time mouseSampleTimer = Nz::Time::Zero();
	constexpr Nz::Time mouseSampleRate = Nz::Time::TickDuration(60);

	Nz::TaskScheduler taskScheduler;

	auto& eventHandler = window.GetEventHandler();
	eventHandler.OnKeyReleased.Connect([&](const Nz::WindowEventHandler*, const Nz::WindowEvent::KeyEvent& key)
	{
		if (key.virtualKey != Nz::Keyboard::VKey::Space)
			return;

		// The particle buffer is used concurrently by the GPU, force it to finish its work before updating
		device->WaitForIdle();

		void* ptr = particleBuffer->Map(0, particleBuffer->GetSize());
		if (!ptr)
		{
			std::cerr << "failed to map particle buffer" << std::endl;
			return;
		}

		Nz::UInt8* particleBasePtr = static_cast<Nz::UInt8*>(ptr) + particlesArrayOffset;
		Nz::SparsePtr<Nz::Vector2f> particlePosPtr(particleBasePtr + particlePosOffset, particleSize);
		Nz::SparsePtr<Nz::Vector2f> particleVelPtr(particleBasePtr + particleVelOffset, particleSize);

		unsigned int workerCount = taskScheduler.GetWorkerCount();
		std::size_t particlePerWorker = particleCount / workerCount;
		std::size_t leftover = particleCount - particlePerWorker * workerCount;

		for (unsigned int i = 0; i < workerCount; ++i)
		{
			taskScheduler.AddTask([&, offset = i * particlePerWorker, count = (i != workerCount - 1) ? particlePerWorker : particlePerWorker + leftover]
			{
				static thread_local std::mt19937 rand_mt(std::random_device{}());

				for (std::size_t i = 0; i < count; ++i)
				{
					std::size_t index = offset + i;
					particleVelPtr[index] += (particlePosPtr[index] - newMousePos).GetNormal() * 500.f;
					particleVelPtr[index] += Nz::Vector2f(velDis(rand_mt), velDis(rand_mt));
				}
			});
		}
		taskScheduler.WaitForTasks();

		particleBuffer->Unmap();
	});

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

		Nz::Time deltaTime = updateClock.Restart();

		Nz::UploadPool& uploadPool = frame.GetUploadPool();

		mouseSampleTimer += deltaTime;
		if (mouseSampleTimer >= mouseSampleRate)
		{
			mouseSampleTimer %= mouseSampleRate;

			previousMousePos = newMousePos;
			Nz::Vector2i mousePos = Nz::Mouse::GetPosition(window);
			newMousePos = Nz::Vector2f(mousePos.x, windowSize.y - mousePos.y);
		}

		frame.Execute([&](Nz::CommandBufferBuilder& builder)
		{
			builder.BeginDebugRegion("Upload scene data", Nz::Color::Yellow());
			{
				// Smooth mouse position over time (as this demo runs at a higher framerate than mouse polling)
				float mouseInterp = mouseSampleTimer.AsSeconds() / mouseSampleRate.AsSeconds();
				Nz::Vector2f mousePos = Nz::Vector2f::Lerp(previousMousePos, newMousePos, mouseInterp);

				auto& allocation = uploadPool.Allocate(sceneBufferSize);
				Nz::AccessByOffset<float&>(allocation.mappedPtr, deltaTimeOffset) = deltaTime.AsSeconds();
				Nz::AccessByOffset<Nz::Vector2f&>(allocation.mappedPtr, mousePosOffset) = mousePos;
				Nz::AccessByOffset<float&>(allocation.mappedPtr, effectRadiusOffset) = (Nz::Mouse::IsButtonPressed(Nz::Mouse::Button::Left)) ? 10000.f : 100.f;

				builder.PreTransferBarrier();
				builder.CopyBuffer(allocation, sceneDataBuffer.get());
				builder.PostTransferBarrier();
			}
			builder.EndDebugRegion();

			builder.BeginDebugRegion("Compute part", Nz::Color::Blue());
			{
				builder.BindComputePipeline(*computePipeline);
				builder.BindComputeShaderBinding(0, *computeBinding);
				builder.Dispatch(particleCount / 64 + 1, 1, 1);
			}
			builder.EndDebugRegion();

			builder.BeginDebugRegion("Main window rendering", Nz::Color::Green());
			{
				Nz::Recti renderRect(0, 0, window.GetSize().x, window.GetSize().y);

				Nz::CommandBufferBuilder::ClearValues clearValues[2];
				clearValues[0].color = Nz::Color::Black();
				clearValues[1].depth = 1.f;
				clearValues[1].stencil = 0;

				builder.BeginRenderPass(windowSwapchain.GetFramebuffer(frame.GetImageIndex()), windowSwapchain.GetRenderPass(), renderRect, { clearValues[0], clearValues[1] });
				{
					builder.SetScissor(Nz::Recti{ 0, 0, int(windowSize.x), int(windowSize.y) });
					builder.SetViewport(Nz::Recti{ 0, 0, int(windowSize.x), int(windowSize.y) });

					builder.BindRenderPipeline(*spriteRenderPipeline.pipeline);

					builder.BindVertexBuffer(0, *spriteRenderData1.vertexBuffer);
					builder.BindRenderShaderBinding(0, *spriteRenderData1.shaderBinding);
					builder.Draw(4, particleCount);
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
	nzsl::Ast::ModulePtr shaderModule = moduleResolver->Resolve("Compute.Particles");
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

[layout(std140)]
struct Particle
{
	color: vec3[f32],
	position: vec2[f32],
	target_position: vec2[f32],
	velocity: vec2[f32]
}

[layout(std140)]
struct ParticleData
{
	particle_count: u32,
	particles: dyn_array[Particle]
}

struct ViewerData
{
	projectionMatrix: mat4[f32]
}

external
{
	[binding(0)] viewerData: uniform[ViewerData],
	[binding(1)] particleData: storage[ParticleData],
	[binding(2)] texture: sampler2D[f32]
}

struct FragOut
{
	[location(0)] color: vec4[f32]
}

struct VertIn
{
	[location(0)] pos: vec2[f32],
	[location(1)] uv: vec2[f32],
	[builtin(instance_index)] particle_index: i32
}

struct VertOut
{
	[location(0)] uv: vec2[f32],
	[location(1)] color: vec3[f32],
	[builtin(position)] pos: vec4[f32]
}

[entry(frag)]
fn main(input: VertOut) -> FragOut
{
	let output: FragOut;
	output.color = vec4[f32](input.color, 1.0) * texture.Sample(input.uv);

	return output;
}

[entry(vert)]
fn main(input: VertIn) -> VertOut
{
	let output: VertOut;
	output.pos = viewerData.projectionMatrix * vec4[f32](input.pos + particleData.particles[input.particle_index].position, 0.0, 1.0);
	output.color = particleData.particles[input.particle_index].color;
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
		pipelineLayoutInfo.bindings.assign({
			{
				0, 0, 1,
				Nz::ShaderBindingType::UniformBuffer,
				nzsl::ShaderStageType::Vertex
			},
			{
				0, 1, 1,
				Nz::ShaderBindingType::StorageBuffer,
				nzsl::ShaderStageType::Vertex | nzsl::ShaderStageType::Fragment
			},
			{
				0, 2, 1,
				Nz::ShaderBindingType::Sampler,
				nzsl::ShaderStageType::Fragment
			}
		});

		pipelineData.pipelineLayout = device.InstantiateRenderPipelineLayout(std::move(pipelineLayoutInfo));

		Nz::RenderPipelineInfo pipelineInfo;
		pipelineInfo.blending = true;
		pipelineInfo.blend.dstColor = Nz::BlendFunc::One;
		pipelineInfo.blend.srcColor = Nz::BlendFunc::One;
		pipelineInfo.blend.dstAlpha = Nz::BlendFunc::ConstantAlpha;
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

SpriteRenderData BuildSpriteData(Nz::RenderDevice& device, const SpriteRenderPipeline& pipelineData, const Nz::Rectf& textureRect, const Nz::Vector2f& screenSize, const Nz::RenderBufferView& buffer, const Nz::RenderBufferView& particleBuffer, std::shared_ptr<Nz::Texture> texture, std::shared_ptr<Nz::TextureSampler> sampler)
{
	try
	{
		std::array<Nz::VertexStruct_XY_UV, 4> pos;
		pos[0].position = textureRect.GetCorner(Nz::RectCorner::LeftBottom);
		pos[0].uv = Nz::Vector2f(0.f, 0.f);
		pos[1].position = textureRect.GetCorner(Nz::RectCorner::LeftTop);
		pos[1].uv = Nz::Vector2f(0.f, 1.f);
		pos[2].position = textureRect.GetCorner(Nz::RectCorner::RightBottom);
		pos[2].uv = Nz::Vector2f(1.f, 0.f);
		pos[3].position = textureRect.GetCorner(Nz::RectCorner::RightTop);
		pos[3].uv = Nz::Vector2f(1.f, 1.f);

		SpriteRenderData renderData;
		renderData.vertexBuffer = device.InstantiateBuffer(Nz::BufferType::Vertex, 4 * 4 * sizeof(float), Nz::BufferUsage::DeviceLocal, pos.data());

		renderData.shaderBinding = pipelineData.pipelineLayout->AllocateShaderBinding(0);
		renderData.shaderBinding->Update({
			{
				0,
				Nz::ShaderBinding::UniformBufferBinding {
					buffer.GetBuffer(), buffer.GetOffset(), buffer.GetSize()
				}
			},
			{
				1,
				Nz::ShaderBinding::StorageBufferBinding {
					particleBuffer.GetBuffer(), particleBuffer.GetOffset(), particleBuffer.GetSize()
				}
			},
			{
				2,
				Nz::ShaderBinding::SampledTextureBinding {
					texture.get(), sampler.get()
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

std::shared_ptr<Nz::Texture> GenerateSpriteTexture(Nz::RenderDevice& device, std::shared_ptr<nzsl::ModuleResolver> moduleResolver, Nz::WindowSwapchain& swapchain)
{
	nzsl::Ast::ModulePtr shaderModule = moduleResolver->Resolve("Compute.ParticleTexture");
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

	Nz::RenderPipelineLayoutInfo pipelineLayoutInfo;
	pipelineLayoutInfo.bindings.assign({
		{
			0, 0, 1,
			Nz::ShaderBindingType::Texture,
			nzsl::ShaderStageType::Compute
		}
	});

	std::shared_ptr<Nz::RenderPipelineLayout> pipelineLayout = device.InstantiateRenderPipelineLayout(std::move(pipelineLayoutInfo));


	Nz::ComputePipelineInfo computePipelineInfo;
	computePipelineInfo.pipelineLayout = pipelineLayout;
	computePipelineInfo.shaderModule = computeShader;

	std::shared_ptr<Nz::ComputePipeline> pipeline = device.InstantiateComputePipeline(std::move(computePipelineInfo));
	if (!pipeline)
	{
		std::cout << "Failed to instantiate compute pipeline" << std::endl;
		std::abort();
	}

	// Destination texture
	Nz::TextureInfo texParams;
	texParams.type = Nz::ImageType::E2D;
	texParams.pixelFormat = Nz::PixelFormat::RGBA8;
	texParams.width = texParams.height = 256;
	texParams.usageFlags = Nz::TextureUsage::ShaderReadWrite | Nz::TextureUsage::ShaderSampling | Nz::TextureUsage::TransferSource | Nz::TextureUsage::TransferDestination;

	std::shared_ptr<Nz::Texture> targetTexture = device.InstantiateTexture(texParams);

	Nz::ShaderBindingPtr binding = pipelineLayout->AllocateShaderBinding(0);
	binding->Update({
		{
			0,
			Nz::ShaderBinding::TextureBinding {
				targetTexture.get(), Nz::TextureAccess::WriteOnly
			}
		}
	});

	swapchain.GetTransientResources().Execute([&](Nz::CommandBufferBuilder& builder)
	{
		builder.TextureBarrier(Nz::PipelineStage::BottomOfPipe, Nz::PipelineStage::ComputeShader, {}, Nz::MemoryAccess::ShaderWrite, Nz::TextureLayout::Undefined, Nz::TextureLayout::General, *targetTexture);

		builder.BindComputePipeline(*pipeline);
		builder.BindComputeShaderBinding(0, *binding);
		builder.Dispatch(texParams.width / 32, texParams.height / 32, 1);

		builder.BuildMipmaps(*targetTexture, 0, targetTexture->GetLevelCount(), Nz::PipelineStage::ComputeShader, Nz::PipelineStage::FragmentShader, Nz::MemoryAccess::ShaderWrite, Nz::MemoryAccess::ShaderRead, Nz::TextureLayout::General, Nz::TextureLayout::ColorInput);

	}, Nz::QueueType::Compute);

	device.WaitForIdle();

	return targetTexture;
}
