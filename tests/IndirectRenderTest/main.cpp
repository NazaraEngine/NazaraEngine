#include <Nazara/Core.hpp>
#include <Nazara/Math.hpp>
#include <Nazara/Platform.hpp>
#include <Nazara/Renderer.hpp>
#include <NZSL/FilesystemModuleResolver.hpp>
#include <NZSL/LangWriter.hpp>
#include <NZSL/Parser.hpp>
#include <NZSL/Ast/Option.hpp>
#include <iostream>
#include <thread>

NAZARA_REQUEST_DEDICATED_GPU()

const char shaderSource[] = R"(
[nzsl_version("1.1")]
module;

option red: bool = false;

[layout(std140)]
struct ViewerData
{
	projectionMatrix: mat4[f32],
	viewMatrix: mat4[f32],
	frustumPlanes: array[vec4[f32], 6],
	viewerPos: vec3[f32]
}

[layout(std430)]
struct ObjectData
{
	worldMatrix: mat4[f32],
	lodDistances: array[f32, 4],
	lodFirstIndex: array[u32, 4],
	lodVertexOffset: array[i32, 4] //< shouldn't be required
}

[layout(std430)]
struct InstanceData
{
	objects: dyn_array[ObjectData]
}

[layout(std430)]
struct DrawIndexedIndirectCommand
{
	indexCount: u32,
	instanceCount: u32,
	firstIndex: u32,
	vertexOffset: i32,
	firstInstance: u32,
}

[layout(std430)]
struct DrawCommands
{
	draw_commands: dyn_array[DrawIndexedIndirectCommand]
}

[set(0)]
external
{
	[binding(0)] viewerData: uniform[ViewerData],
	[binding(1)] instanceData: storage[InstanceData, readonly],
	[binding(2)] output: storage[DrawCommands]
}

[set(1)]
external
{
	[binding(0)] tex: sampler2D[f32]
}

struct VertIn
{
	[location(0)] position: vec3[f32],
	[location(1)] normal: vec3[f32],
	[location(2)] uv: vec2[f32],
	[builtin(instance_index)] instanceIndex: i32
}

struct VertOut
{
	[builtin(position)] position: vec4[f32],
	[location(0)] normal: vec3[f32],
	[location(1)] uv: vec2[f32]
}

struct FragOut
{
	[location(0)] color: vec4[f32]
}

struct CullingInput
{
	[builtin(global_invocation_indices)] indices: vec3[u32]
}

fn plane_dist(plane: vec4[f32], pos: vec3[f32]) -> f32
{
	return dot(plane.xyz, pos) + plane.w;
}

fn frustum_check(frustum_planes: array[vec4[f32], 6], pos: vec3[f32]) -> bool
{
	let visible = true;
	for plane in viewerData.frustumPlanes
	{
		if (plane_dist(plane, pos) < 0.0)
			visible = false;
	}

	return visible;
}

[entry(comp), workgroup(32, 1, 1)]
fn cull(input: CullingInput)
{
	let objectCount = instanceData.objects.Size();
	let objectIndex = input.indices.x;
	if (objectIndex >= objectCount)
		return;

	let objectPos = instanceData.objects[objectIndex].worldMatrix[3].xyz;
	let frustumPlanes = viewerData.frustumPlanes;
	if (frustum_check(frustumPlanes, objectPos))
	{
		let dist = distance(viewerData.viewerPos, objectPos);

		let lod = 0;
		for i in 1 -> 4
		{
			if (dist < instanceData.objects[objectIndex].lodDistances[i])
			{
				lod = i - 1;
				break;
			}
		}

		output.draw_commands[objectIndex].instanceCount = 1;
		output.draw_commands[objectIndex].firstIndex = instanceData.objects[objectIndex].lodFirstIndex[lod];
		output.draw_commands[objectIndex].vertexOffset = instanceData.objects[objectIndex].lodVertexOffset[lod];
	}
	else
	{
		output.draw_commands[objectIndex].instanceCount = 0;
	}
}

[entry(frag)]
fn main(fragIn: VertOut) -> FragOut
{
	let lightDir = vec3[f32](0.0, 0.707, 0.707);
	let lightFactor = dot(fragIn.normal, lightDir);

	let fragOut: FragOut;
	fragOut.color = lightFactor * tex.Sample(fragIn.uv) * const_select(red, vec4[f32](1.0, 0.0, 0.0, 1.0), vec4[f32](1.0, 1.0, 1.0, 1.0));

	return fragOut;
}

[entry(vert)]
fn main(vertIn: VertIn) -> VertOut
{
	let vertOut: VertOut;
	vertOut.position = viewerData.projectionMatrix * viewerData.viewMatrix * instanceData.objects[vertIn.instanceIndex].worldMatrix * vec4[f32](vertIn.position, 1.0);
	vertOut.normal = vertIn.normal;
	vertOut.uv = vertIn.uv;

	return vertOut;
}
)";

int main()
{
	std::filesystem::path resourceDir = "assets/examples";
	if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory("../.." / resourceDir))
		resourceDir = "../.." / resourceDir;

	Nz::Renderer::Config rendererConfig;
#ifndef NAZARA_PLATFORM_WEB
	std::cout << "Run using Vulkan? (y/n)" << std::endl;
	if (std::getchar() == 'y')
		rendererConfig.preferredAPI = Nz::RenderAPI::Vulkan;
	else
		rendererConfig.preferredAPI = Nz::RenderAPI::OpenGL;
#endif

	Nz::Application<Nz::Renderer> app(rendererConfig);
	auto& windowingApp = app.AddComponent<Nz::WindowingAppComponent>();

	std::shared_ptr<Nz::RenderDevice> device = Nz::Renderer::Instance()->InstanciateRenderDevice(0, {
		.computeShaders = true,
		.multiDrawIndirect = true
	});

	std::string windowTitle = "Render Test";
	Nz::Window& window = windowingApp.CreateWindow(Nz::VideoMode(1280, 720), windowTitle);
	Nz::WindowSwapchain windowSwapchain(device, window);

	nzsl::Ast::ModulePtr shaderModule;
	std::shared_ptr<Nz::ShaderModule> fragVertShader;
	std::shared_ptr<Nz::ShaderModule> cullingShader;

	try
	{
		shaderModule = nzsl::Parse(std::string_view(shaderSource, sizeof(shaderSource)));
		if (!shaderModule)
		{
			std::cout << "Failed to parse shader module" << std::endl;
			return __LINE__;
		}

		nzsl::BackendParameters states;
		states.backendPasses |= nzsl::BackendPass::Optimize;

		fragVertShader = device->InstantiateShaderModule(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, *shaderModule, states);
		if (!fragVertShader)
		{
			std::cout << "Failed to instantiate rendering shader" << std::endl;
			return __LINE__;
		}

		cullingShader = device->InstantiateShaderModule(nzsl::ShaderStageType::Compute, *shaderModule, states);
		if (!cullingShader)
		{
			std::cout << "Failed to instantiate culling shader" << std::endl;
			return __LINE__;
		}

	}
	catch (const std::exception& e)
	{
		std::cerr << "failed to create shaders: " << e.what() << std::endl;
		return __LINE__;
	}

	struct Lod
	{
		Nz::UInt32 indexCount;
		Nz::UInt32 indexOffset;
		Nz::Int32 vertexOffset;
	};

	std::array<Lod, 4> lods;
	std::vector<Nz::UInt16> indices;
	std::vector<Nz::VertexStruct_XYZ_Normal_UV> vertices;
	Nz::Boxf spaceshipAabb;

	Nz::MeshParams meshParams;
	meshParams.center = true;
	meshParams.vertexRotation = Nz::EulerAnglesf(0.f, -90.f, 0.f);
	meshParams.vertexScale = Nz::Vector3f(0.2f);
	meshParams.vertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout::XYZ_Normal_UV);

	for (unsigned int lod : { 0, 1, 2, 3 })
	{
		std::string path = Nz::Format("Spaceship/spaceship_lod{}.obj", lod);

		std::shared_ptr<Nz::Mesh> spaceship = Nz::Mesh::LoadFromFile(resourceDir / path, meshParams);
		if (!spaceship)
		{
			NazaraError("failed to load {0}", path);
			return __LINE__;
		}

		if (lod == 0)
			spaceshipAabb = spaceship->GetAABB();

		std::shared_ptr<Nz::StaticMesh> spaceshipMesh = std::static_pointer_cast<Nz::StaticMesh>(spaceship->GetSubMesh(0));

		const std::shared_ptr<Nz::VertexBuffer>& meshVB = spaceshipMesh->GetVertexBuffer();
		const std::shared_ptr<const Nz::IndexBuffer>& meshIB = spaceshipMesh->GetIndexBuffer();

		// Index buffer
		std::cout << "Index count (lod " << lod << "): " << meshIB->GetIndexCount() << std::endl;

		// Vertex buffer
		std::cout << "Vertex count (lod " << lod << "): " << meshVB->GetVertexCount() << std::endl;

		// Create renderbuffers (GPU buffers)
		assert(meshIB->GetBuffer()->GetStorage() == Nz::DataStorage::Software);
		assert(meshVB->GetBuffer()->GetStorage() == Nz::DataStorage::Software);
		const Nz::SoftwareBuffer* indexBufferContent = static_cast<const Nz::SoftwareBuffer*>(meshIB->GetBuffer().get());
		const Nz::SoftwareBuffer* vertexBufferContent = static_cast<const Nz::SoftwareBuffer*>(meshVB->GetBuffer().get());

		Nz::UInt32 previousIndexCount = indices.size();
		indices.resize(previousIndexCount + meshIB->GetIndexCount());
		std::memcpy(&indices[previousIndexCount], indexBufferContent->GetData(), meshIB->GetIndexCount() * sizeof(Nz::UInt16));

		Nz::UInt32 previousVertexCount = vertices.size();
		vertices.resize(previousVertexCount + meshVB->GetVertexCount());
		std::memcpy(&vertices[previousVertexCount], vertexBufferContent->GetData(), meshVB->GetVertexCount() * meshParams.vertexDeclaration->GetStride());

		lods[lod] = Lod{
			meshIB->GetIndexCount(),
			previousIndexCount,
			static_cast<Nz::Int32>(previousVertexCount)
		};
	}

	std::shared_ptr<Nz::RenderBuffer> renderBufferIB = device->InstantiateBuffer(indices.size() * sizeof(Nz::UInt16), Nz::BufferUsage::IndexBuffer | Nz::BufferUsage::DeviceLocal, indices.data());
	std::shared_ptr<Nz::RenderBuffer> renderBufferVB = device->InstantiateBuffer(vertices.size() * meshParams.vertexDeclaration->GetStride(), Nz::BufferUsage::VertexBuffer | Nz::BufferUsage::DeviceLocal, vertices.data());

	struct ObjectData
	{
		Nz::Matrix4f worldMatrix;
		std::array<float, 4> lodDistances;
		std::array<Nz::UInt32, 4> lodFirstIndex;
		std::array<Nz::Int32, 4> lodVertexOffset; //< shouldn't be required
	};

	unsigned int instanceCount = 10000;
	std::vector<ObjectData> objectData(instanceCount);
	for (unsigned int i = 0; i < instanceCount; ++i)
	{
		objectData[i].worldMatrix = Nz::Matrix4f::Translate(Nz::Vector3f((i / 100) * 2.0f, 0.0f, (i % 100) * 2.0f));
		objectData[i].lodDistances = std::array<float, 4>{ 0.f, 10.f, 50.f, 100.f };
		objectData[i].lodFirstIndex = std::array<Nz::UInt32, 4>{ lods[0].indexOffset, lods[1].indexOffset, lods[2].indexOffset, lods[3].indexOffset };
		objectData[i].lodVertexOffset = std::array<Nz::Int32, 4>{ lods[0].vertexOffset, lods[1].vertexOffset, lods[2].vertexOffset, lods[3].vertexOffset };
	}

	std::shared_ptr<Nz::RenderBuffer> objectBuffers = device->InstantiateBuffer(instanceCount * sizeof(ObjectData), Nz::BufferUsage::StorageBuffer | Nz::BufferUsage::DeviceLocal, objectData.data());


	// Texture
	Nz::TextureParams texParams;
	texParams.renderDevice = device;
	texParams.loadFormat = Nz::PixelFormat::RGBA8_SRGB;

	std::shared_ptr<Nz::Texture> texture = Nz::Texture::LoadFromFile(resourceDir / "Spaceship/Texture/diffuse.png", texParams);
	std::shared_ptr<Nz::TextureSampler> textureSampler = device->InstantiateTextureSampler({});

	struct
	{
		Nz::Matrix4f projectionMatrix;
		Nz::Matrix4f viewMatrix;
		std::array<Nz::Vector4f, 6> frustumPlanes;
		Nz::Vector3f viewerPos;
	}
	ubo;

	Nz::Vector2ui windowSize = window.GetSize();
	ubo.projectionMatrix = Nz::Matrix4f::Perspective(Nz::DegreeAnglef(70.f), float(windowSize.x) / windowSize.y, 0.1f, 1000.f);

	Nz::UInt32 uniformSize = sizeof(ubo);

	Nz::RenderPipelineLayoutInfo pipelineLayoutInfo;
	{
		pipelineLayoutInfo.bindings.push_back({
			.bindingIndex = 0,
			.type = Nz::ShaderBindingType::UniformBuffer,
			.shaderStageFlags = nzsl::ShaderStageType::Vertex | nzsl::ShaderStageType::Compute,
		});
		
		pipelineLayoutInfo.bindings.push_back({
			.bindingIndex = 1,
			.type = Nz::ShaderBindingType::StorageBuffer,
			.shaderStageFlags = nzsl::ShaderStageType::Vertex | nzsl::ShaderStageType::Compute,
		});
		
		pipelineLayoutInfo.bindings.push_back({
			.bindingIndex = 2,
			.type = Nz::ShaderBindingType::StorageBuffer,
			.shaderStageFlags = nzsl::ShaderStageType::Compute,
		});
	}

	std::shared_ptr<Nz::RenderPipelineLayout> basePipelineLayout = device->InstantiateRenderPipelineLayout(pipelineLayoutInfo);

	auto& pipelineTextureBinding = pipelineLayoutInfo.bindings.emplace_back();
	pipelineTextureBinding.setIndex = 1;
	pipelineTextureBinding.bindingIndex = 0;
	pipelineTextureBinding.shaderStageFlags = nzsl::ShaderStageType::Fragment;
	pipelineTextureBinding.type = Nz::ShaderBindingType::Sampler;

	std::shared_ptr<Nz::RenderPipelineLayout> renderPipelineLayout = device->InstantiateRenderPipelineLayout(std::move(pipelineLayoutInfo));

	std::shared_ptr<Nz::RenderBuffer> uniformBuffer = device->InstantiateBuffer(uniformSize, Nz::BufferUsage::UniformBuffer | Nz::BufferUsage::DeviceLocal);


	Nz::RenderPipelineInfo renderPipelineInfo;
	renderPipelineInfo.pipelineLayout = renderPipelineLayout;
	renderPipelineInfo.faceCulling = Nz::FaceCulling::Back;

	renderPipelineInfo.depthBuffer = true;
	renderPipelineInfo.shaderModules.emplace_back(fragVertShader);

	auto& pipelineVertexBuffer = renderPipelineInfo.vertexBuffers.emplace_back();
	pipelineVertexBuffer.binding = 0;
	pipelineVertexBuffer.declaration = meshParams.vertexDeclaration;

	std::shared_ptr<Nz::RenderPipeline> pipeline = device->InstantiateRenderPipeline(renderPipelineInfo);

	Nz::RenderPipelineInfo cullPipelineInfo;
	renderPipelineInfo.pipelineLayout = renderPipelineLayout;

	std::shared_ptr<Nz::ComputePipeline> cullPipeline = device->InstantiateComputePipeline({
		.pipelineLayout = basePipelineLayout,
		.shaderModule = cullingShader
	});

	std::shared_ptr<Nz::CommandPool> commandPool = device->InstantiateCommandPool(Nz::QueueType::Graphics);

	// Indirect buffer
	std::vector<Nz::DrawIndexedIndirectCommand> indirectCommands(instanceCount);

	Nz::UInt32 instanceIndex = 0;
	for (Nz::DrawIndexedIndirectCommand& indirectCommand : indirectCommands)
	{
		indirectCommand.firstIndex = lods[0].indexOffset;
		indirectCommand.firstInstance = instanceIndex++;
		indirectCommand.indexCount = lods[0].indexCount;
		indirectCommand.instanceCount = 1;
		indirectCommand.vertexOffset = lods[0].vertexOffset;
	}

	std::shared_ptr<Nz::RenderBuffer> indirectBuffer = device->InstantiateBuffer(sizeof(Nz::DrawIndexedIndirectCommand) * instanceCount, Nz::BufferUsage::IndirectBuffer | Nz::BufferUsage::StorageBuffer | Nz::BufferUsage::DeviceLocal, indirectCommands.data());

	Nz::ShaderBindingPtr viewerShaderBinding = basePipelineLayout->AllocateShaderBinding(0);
	viewerShaderBinding->Update({
		{
			0,
			Nz::ShaderBinding::UniformBufferBinding {
				uniformBuffer.get(), 0, uniformSize
			}
		},
		{
			1,
			Nz::ShaderBinding::StorageBufferBinding {
				objectBuffers.get(), 0, objectBuffers->GetSize()
			}
		},
		{
			2,
			Nz::ShaderBinding::StorageBufferBinding {
				indirectBuffer.get(), 0, indirectBuffer->GetSize()
			}
		}
	});

	Nz::ShaderBinding::SampledTextureBinding textureBinding {
		texture.get(), textureSampler.get()
	};

	Nz::ShaderBindingPtr textureShaderBinding = renderPipelineLayout->AllocateShaderBinding(1);
	textureShaderBinding->Update({
		{
			0,
			Nz::ShaderBinding::SampledTextureBindings {
				1, &textureBinding
			}
		}
	});

	Nz::Vector3f viewerPos = Nz::Vector3f::Zero();

	Nz::EulerAnglesf camAngles(0.f, 0.f, 0.f);
	Nz::Quaternionf camQuat(camAngles);

	Nz::MillisecondClock updateClock;
	Nz::MillisecondClock secondClock;
	unsigned int fps = 0;
	bool uboUpdate = true;

	window.SetRelativeMouseMode(true);

	Nz::DebugDrawer debugDrawer(*device);

	Nz::WindowEventHandler& windowEvents = window.GetEventHandler();
	windowEvents.OnKeyPressed.Connect([&](const Nz::WindowEventHandler*, const Nz::WindowEvent::KeyEvent& key)
	{
		if (key.virtualKey == Nz::Keyboard::VKey::F1)
			window.Create(Nz::VideoMode(1920, 1080), windowTitle);
	});

	windowEvents.OnMouseMoved.Connect([&](const Nz::WindowEventHandler*, const Nz::WindowEvent::MouseMoveEvent& mouseMove)
	{
		// Gestion de la caméra free-fly (Rotation)
		float sensitivity = 0.3f; // Sensibilité de la souris

		// On modifie l'angle de la caméra grâce au déplacement relatif sur X de la souris
		camAngles.yaw = camAngles.yaw - mouseMove.deltaX * sensitivity;
		camAngles.yaw.Normalize();

		// Idem, mais pour éviter les problèmes de calcul de la matrice de vue, on restreint les angles
		camAngles.pitch = Nz::Clamp(camAngles.pitch - mouseMove.deltaY * sensitivity, -89.f, 89.f);

		camQuat = camAngles;

		uboUpdate = true;
	});

	windowEvents.OnResized.Connect([&](const Nz::WindowEventHandler*, const Nz::WindowEvent::SizeEvent& sizeEvent)
	{
		windowSize = { sizeEvent.width, sizeEvent.height };
		ubo.projectionMatrix = Nz::Matrix4f::Perspective(Nz::DegreeAnglef(70.f), float(windowSize.x) / windowSize.y, 0.1f, 1000.f);
		uboUpdate = true;
	});

	app.AddUpdaterFunc([&]
	{
		if (std::optional<Nz::Time> deltaTime = updateClock.RestartIfOver(Nz::Time::TickDuration(60)))
		{
			float cameraSpeed = 2.f * deltaTime->AsSeconds();

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Up) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Z))
				viewerPos += camQuat * Nz::Vector3f::Forward() * cameraSpeed;

			// Si la flèche du bas ou la touche S est pressée, on recule
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Down) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::S))
				viewerPos += camQuat * Nz::Vector3f::Backward() * cameraSpeed;

			// Etc...
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Left) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Q))
				viewerPos += camQuat * Nz::Vector3f::Left() * cameraSpeed;

			// Etc...
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Right) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::D))
				viewerPos += camQuat * Nz::Vector3f::Right() * cameraSpeed;

			// Majuscule pour monter, notez l'utilisation d'une direction globale (Non-affectée par la rotation)
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::LShift) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::RShift))
				viewerPos += Nz::Vector3f::Up() * cameraSpeed;

			// Contrôle (Gauche ou droite) pour descendre dans l'espace global, etc...
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::LControl) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::RControl))
				viewerPos += Nz::Vector3f::Down() * cameraSpeed;

			uboUpdate = true;
		}

		Nz::RenderFrame frame = windowSwapchain.AcquireFrame();
		if (!frame)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			return;
		}

		debugDrawer.Reset();
		debugDrawer.SetViewerData(ubo.viewMatrix * ubo.projectionMatrix);

		Nz::Boxf aabb = spaceshipAabb;
		aabb.Transform(objectData[0].worldMatrix);

		debugDrawer.DrawBox(aabb, Nz::Color::Green());

		ubo.viewMatrix = Nz::Matrix4f::TransformInverse(viewerPos, camAngles);
		ubo.viewerPos = viewerPos;

		auto frustumPlanes = Nz::Frustumf::Extract(ubo.viewMatrix * ubo.projectionMatrix).GetPlanes();
		for (std::size_t i = 0; i < 6; ++i)
			ubo.frustumPlanes[i] = Nz::Vector4f(frustumPlanes.at(i).normal, frustumPlanes.at(i).distance);

		if (uboUpdate)
		{
			auto& allocation = frame.GetUploadPool().Allocate(uniformSize);

			std::memcpy(allocation.mappedPtr, &ubo, sizeof(ubo));

			frame.Execute([&](Nz::CommandBufferBuilder& builder)
			{
				builder.BeginDebugRegion("UBO Update", Nz::Color::Yellow());
				{
					builder.MemoryBarrier({ .srcStageMask = Nz::PipelineStage::BottomOfPipe, .dstStageMask = Nz::PipelineStage::Transfer, .srcAccessMask = {}, .dstAccessMask = Nz::MemoryAccess::TransferWrite });
					builder.CopyBuffer(allocation, uniformBuffer.get());
					builder.MemoryBarrier({ .srcStageMask = Nz::PipelineStage::Transfer, .dstStageMask = Nz::PipelineStage::FragmentShader | Nz::PipelineStage::VertexShader, .srcAccessMask = Nz::MemoryAccess::TransferWrite, .dstAccessMask = Nz::MemoryAccess::ShaderRead | Nz::MemoryAccess::UniformBufferRead });
				}
				builder.EndDebugRegion();
			}, Nz::QueueType::Transfer);

			uboUpdate = false;
		}

		debugDrawer.Prepare(frame);

		const Nz::WindowSwapchain* windowRT = &windowSwapchain;
		frame.Execute([&](Nz::CommandBufferBuilder& builder)
		{
			builder.BeginDebugRegion("GPU Culling", Nz::Color::Blue());
			{
				builder.BindComputePipeline(*cullPipeline);
				builder.BindComputeShaderBinding(0, *viewerShaderBinding);
				builder.Dispatch(Nz::AlignPow2(instanceCount, 32u), 1, 1);
			}
			builder.EndDebugRegion();

			windowSize = window.GetSize();
			Nz::Recti renderRect(0, 0, windowSize.x, windowSize.y);

			Nz::CommandBufferBuilder::ClearValues clearValues[2];
			clearValues[0].color = Nz::Color::Black();
			clearValues[1].depth = 1.f;
			clearValues[1].stencil = 0;

			builder.BeginDebugRegion("Main window rendering", Nz::Color::Green());
			{
				builder.BeginRenderPass(windowRT->GetFramebuffer(frame.GetImageIndex()), windowRT->GetRenderPass(), renderRect, { clearValues[0], clearValues[1] });
				{
					builder.BindIndexBuffer(*renderBufferIB, Nz::IndexType::U16);
					builder.BindRenderPipeline(*pipeline);
					builder.BindVertexBuffer(0, *renderBufferVB);
					builder.BindRenderShaderBinding(0, *viewerShaderBinding);
					builder.BindRenderShaderBinding(1, *textureShaderBinding);

					builder.SetScissor(Nz::Recti{ 0, 0, int(windowSize.x), int(windowSize.y) });
					builder.SetViewport(Nz::Recti{ 0, 0, int(windowSize.x), int(windowSize.y) });

					builder.DrawIndexedIndirect(*indirectBuffer, 0, instanceCount, sizeof(Nz::DrawIndexedIndirectCommand));

					debugDrawer.Draw(builder);
				}
				builder.EndRenderPass();
			}
			builder.EndDebugRegion();

		}, Nz::QueueType::Graphics);

		frame.Present();

		// On incrémente le compteur de FPS improvisé
		fps++;

		if (secondClock.RestartIfOver(Nz::Time::Second()))
		{
			// Et on insère ces données dans le titre de la fenêtre
			window.SetTitle(windowTitle + " - " + Nz::NumberToString(fps) + " FPS");

			/*
			Note: En C++11 il est possible d'insérer de l'Unicode de façon standard, quel que soit l'encodage du fichier,
			via quelque chose de similaire à "Cha\u00CEne de caract\u00E8res".
			Cependant, si le code source est encodé en UTF-8 (Comme c'est le cas dans ce fichier),
			cela fonctionnera aussi comme ceci : "Chaîne de caractères".
			*/

			// Et on réinitialise le compteur de FPS
			fps = 0;
		}
	});

	return app.Run();
}
