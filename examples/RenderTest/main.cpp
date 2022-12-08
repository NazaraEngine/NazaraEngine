#include <Nazara/Core.hpp>
#include <Nazara/Math.hpp>
#include <Nazara/Platform.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Renderer/DebugDrawer.hpp>
#include <NZSL/FilesystemModuleResolver.hpp>
#include <NZSL/LangWriter.hpp>
#include <NZSL/Parser.hpp>
#include <NZSL/Ast/SanitizeVisitor.hpp>
#include <Nazara/Utility.hpp>
#include <array>
#include <chrono>
#include <iostream>
#include <thread>

NAZARA_REQUEST_DEDICATED_GPU()

const char shaderSource[] = R"(
[nzsl_version("1.0")]
module;

option red: bool = false;

[layout(std140)]
struct Data
{
	projectionMatrix: mat4[f32],
	worldMatrix: mat4[f32],
	viewMatrix: mat4[f32]
}

[set(0)]
external
{
	[binding(0)] data: uniform[Data]
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
	[location(2)] uv: vec2[f32]
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
	vertOut.position = data.projectionMatrix * data.viewMatrix * data.worldMatrix * vec4[f32](vertIn.position, 1.0);
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
	std::cout << "Run using Vulkan? (y/n)" << std::endl;
	if (std::getchar() == 'y')
		rendererConfig.preferredAPI = Nz::RenderAPI::Vulkan;
	else
		rendererConfig.preferredAPI = Nz::RenderAPI::OpenGL;

	Nz::Modules<Nz::Renderer> nazara(rendererConfig);

	std::shared_ptr<Nz::RenderDevice> device = Nz::Renderer::Instance()->InstanciateRenderDevice(0);

	Nz::RenderWindow window;

	std::string windowTitle = "Render Test";
	if (!window.Create(device, Nz::VideoMode(800, 600, 32), windowTitle))
	{
		std::cout << "Failed to create Window" << std::endl;
		return __LINE__;
	}

	nzsl::Ast::ModulePtr shaderModule = nzsl::Parse(std::string_view(shaderSource, sizeof(shaderSource)));
	if (!shaderModule)
	{
		std::cout << "Failed to parse shader module" << std::endl;
		return __LINE__;
	}

	nzsl::ShaderWriter::States states;
	states.optionValues[Nz::CRC32("red")] = false; //< Try enabling this!
	states.optimize = true;

	auto fragVertShader = device->InstantiateShaderModule(nzsl::ShaderStageType::Fragment | nzsl::ShaderStageType::Vertex, *shaderModule, states);
	if (!fragVertShader)
	{
		std::cout << "Failed to instantiate shader" << std::endl;
		return __LINE__;
	}

	Nz::MeshParams meshParams;
	meshParams.center = true;
	meshParams.vertexRotation = Nz::EulerAnglesf(0.f, -90.f, 0.f);
	meshParams.vertexScale = Nz::Vector3f(0.002f);
	meshParams.vertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout::XYZ_Normal_UV);

	std::shared_ptr<Nz::Mesh> spaceship = Nz::Mesh::LoadFromFile(resourceDir / "Spaceship/spaceship.obj", meshParams);
	if (!spaceship)
	{
		NazaraError("Failed to load model");
		return __LINE__;
	}

	std::shared_ptr<Nz::StaticMesh> spaceshipMesh = std::static_pointer_cast<Nz::StaticMesh>(spaceship->GetSubMesh(0));

	const std::shared_ptr<Nz::VertexBuffer>& meshVB = spaceshipMesh->GetVertexBuffer();
	const std::shared_ptr<const Nz::IndexBuffer>& meshIB = spaceshipMesh->GetIndexBuffer();

	// Index buffer
	std::cout << "Index count: " << meshIB->GetIndexCount() << std::endl;

	// Vertex buffer
	std::cout << "Vertex count: " << meshVB->GetVertexCount() << std::endl;

	// Create renderbuffers (GPU buffers)
	const std::shared_ptr<Nz::RenderDevice>& renderDevice = window.GetRenderDevice();

	assert(meshIB->GetBuffer()->GetStorage() == Nz::DataStorage::Software);
	assert(meshVB->GetBuffer()->GetStorage() == Nz::DataStorage::Software);
	const Nz::SoftwareBuffer* indexBufferContent = static_cast<const Nz::SoftwareBuffer*>(meshIB->GetBuffer().get());
	const Nz::SoftwareBuffer* vertexBufferContent = static_cast<const Nz::SoftwareBuffer*>(meshVB->GetBuffer().get());

	std::shared_ptr<Nz::RenderBuffer> renderBufferIB = renderDevice->InstantiateBuffer(Nz::BufferType::Index, indexBufferContent->GetSize(), Nz::BufferUsage::DeviceLocal, indexBufferContent->GetData());
	std::shared_ptr<Nz::RenderBuffer> renderBufferVB = renderDevice->InstantiateBuffer(Nz::BufferType::Vertex, vertexBufferContent->GetSize(), Nz::BufferUsage::DeviceLocal, vertexBufferContent->GetData());

	// Texture
	Nz::TextureParams texParams;
	texParams.renderDevice = device;
	texParams.loadFormat = Nz::PixelFormat::RGBA8_SRGB;

	std::shared_ptr<Nz::Texture> texture = Nz::Texture::LoadFromFile(resourceDir / "Spaceship/Texture/diffuse.png", texParams);
	std::shared_ptr<Nz::TextureSampler> textureSampler = device->InstantiateTextureSampler({});

	struct
	{
		Nz::Matrix4f projectionMatrix;
		Nz::Matrix4f modelMatrix;
		Nz::Matrix4f viewMatrix;
	}
	ubo;

	Nz::Vector2ui windowSize = window.GetSize();
	ubo.projectionMatrix = Nz::Matrix4f::Perspective(Nz::DegreeAnglef(70.f), float(windowSize.x) / windowSize.y, 0.1f, 1000.f);
	ubo.viewMatrix = Nz::Matrix4f::Translate(Nz::Vector3f::Backward() * 1);
	ubo.modelMatrix = Nz::Matrix4f::Translate(Nz::Vector3f::Forward() * 2);

	Nz::UInt32 uniformSize = sizeof(ubo);

	Nz::RenderPipelineLayoutInfo pipelineLayoutInfo;

	auto& uboBinding = pipelineLayoutInfo.bindings.emplace_back();
	uboBinding.setIndex = 0;
	uboBinding.bindingIndex = 0;
	uboBinding.shaderStageFlags = nzsl::ShaderStageType::Vertex;
	uboBinding.type = Nz::ShaderBindingType::UniformBuffer;

	std::shared_ptr<Nz::RenderPipelineLayout> basePipelineLayout = device->InstantiateRenderPipelineLayout(pipelineLayoutInfo);

	auto& pipelineTextureBinding = pipelineLayoutInfo.bindings.emplace_back();
	pipelineTextureBinding.setIndex = 1;
	pipelineTextureBinding.bindingIndex = 0;
	pipelineTextureBinding.shaderStageFlags = nzsl::ShaderStageType::Fragment;
	pipelineTextureBinding.type = Nz::ShaderBindingType::Texture;

	std::shared_ptr<Nz::RenderPipelineLayout> renderPipelineLayout = device->InstantiateRenderPipelineLayout(std::move(pipelineLayoutInfo));

	Nz::ShaderBindingPtr viewerShaderBinding = basePipelineLayout->AllocateShaderBinding(0);
	Nz::ShaderBindingPtr textureShaderBinding = renderPipelineLayout->AllocateShaderBinding(1);

	std::shared_ptr<Nz::RenderBuffer> uniformBuffer = device->InstantiateBuffer(Nz::BufferType::Uniform, uniformSize, Nz::BufferUsage::DeviceLocal | Nz::BufferUsage::Dynamic);

	viewerShaderBinding->Update({
		{
			0,
			Nz::ShaderBinding::UniformBufferBinding {
				uniformBuffer.get(), 0, uniformSize
			}
		}
	});

	Nz::ShaderBinding::TextureBinding textureBinding {
		texture.get(), textureSampler.get()
	};

	textureShaderBinding->Update({
		{
			0,
			Nz::ShaderBinding::TextureBindings {
				1, &textureBinding
			}
		}
	});

	Nz::RenderPipelineInfo pipelineInfo;
	pipelineInfo.pipelineLayout = renderPipelineLayout;
	pipelineInfo.faceCulling = Nz::FaceCulling::Back;

	pipelineInfo.depthBuffer = true;
	pipelineInfo.shaderModules.emplace_back(fragVertShader);

	auto& pipelineVertexBuffer = pipelineInfo.vertexBuffers.emplace_back();
	pipelineVertexBuffer.binding = 0;
	pipelineVertexBuffer.declaration = meshVB->GetVertexDeclaration();

	std::shared_ptr<Nz::RenderPipeline> pipeline = device->InstantiateRenderPipeline(pipelineInfo);

	std::shared_ptr<Nz::CommandPool> commandPool = renderDevice->InstantiateCommandPool(Nz::QueueType::Graphics);

	Nz::Vector3f viewerPos = Nz::Vector3f::Zero();

	Nz::EulerAnglesf camAngles(0.f, 0.f, 0.f);
	Nz::Quaternionf camQuat(camAngles);

	window.EnableEventPolling(true);

	Nz::Clock updateClock;
	Nz::Clock secondClock;
	unsigned int fps = 0;
	bool uboUpdate = true;

	Nz::Mouse::SetRelativeMouseMode(true);

	Nz::DebugDrawer debugDrawer(*renderDevice);

	while (window.IsOpen())
	{
		Nz::WindowEvent event;
		while (window.PollEvent(&event))
		{
			switch (event.type)
			{
				case Nz::WindowEventType::Quit:
					window.Close();
					break;

				case Nz::WindowEventType::MouseMoved: // La souris a bougé
				{
					// Gestion de la caméra free-fly (Rotation)
					float sensitivity = 0.3f; // Sensibilité de la souris

					// On modifie l'angle de la caméra grâce au déplacement relatif sur X de la souris
					camAngles.yaw = camAngles.yaw - event.mouseMove.deltaX * sensitivity;
					camAngles.yaw.Normalize();

					// Idem, mais pour éviter les problèmes de calcul de la matrice de vue, on restreint les angles
					camAngles.pitch = Nz::Clamp(camAngles.pitch - event.mouseMove.deltaY*sensitivity, -89.f, 89.f);

					camQuat = camAngles;
					
					uboUpdate = true;
					break;
				}

				case Nz::WindowEventType::Resized:
				{
					windowSize = window.GetSize();
					ubo.projectionMatrix = Nz::Matrix4f::Perspective(Nz::DegreeAnglef(70.f), float(windowSize.x) / windowSize.y, 0.1f, 1000.f);
					uboUpdate = true;
					break;
				}

				default:
					break;
			}
		}

		if (updateClock.GetMilliseconds() > 1000 / 60)
		{
			float cameraSpeed = 2.f * updateClock.GetSeconds();
			updateClock.Restart();

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

		Nz::RenderFrame frame = window.AcquireFrame();
		if (!frame)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			continue;
		}

		debugDrawer.Reset(frame);
		debugDrawer.SetViewerData(ubo.viewMatrix * ubo.projectionMatrix);

		Nz::Boxf aabb = spaceship->GetAABB();
		aabb.Transform(ubo.modelMatrix);

		debugDrawer.DrawBox(aabb, Nz::Color::Green);

		ubo.viewMatrix = Nz::Matrix4f::TransformInverse(viewerPos, camAngles);

		if (uboUpdate)
		{
			auto& allocation = frame.GetUploadPool().Allocate(uniformSize);

			std::memcpy(allocation.mappedPtr, &ubo, sizeof(ubo));

			frame.Execute([&](Nz::CommandBufferBuilder& builder)
			{
				builder.BeginDebugRegion("UBO Update", Nz::Color::Yellow);
				{
					builder.PreTransferBarrier();
					builder.CopyBuffer(allocation, uniformBuffer.get());
					builder.PostTransferBarrier();
				}
				builder.EndDebugRegion();
			}, Nz::QueueType::Transfer);

			uboUpdate = false;
		}

		debugDrawer.Prepare(frame);

		const Nz::RenderTarget* windowRT = window.GetRenderTarget();
		frame.Execute([&](Nz::CommandBufferBuilder& builder)
		{
			Nz::Recti renderRect(0, 0, window.GetSize().x, window.GetSize().y);

			Nz::CommandBufferBuilder::ClearValues clearValues[2];
			clearValues[0].color = Nz::Color::Black;
			clearValues[1].depth = 1.f;
			clearValues[1].stencil = 0;

			builder.BeginDebugRegion("Main window rendering", Nz::Color::Green);
			{
				builder.BeginRenderPass(windowRT->GetFramebuffer(frame.GetFramebufferIndex()), windowRT->GetRenderPass(), renderRect, { clearValues[0], clearValues[1] });
				{
					builder.BindIndexBuffer(*renderBufferIB, Nz::IndexType::U16);
					builder.BindPipeline(*pipeline);
					builder.BindVertexBuffer(0, *renderBufferVB);
					builder.BindShaderBinding(0, *viewerShaderBinding);
					builder.BindShaderBinding(1, *textureShaderBinding);

					builder.SetScissor(Nz::Recti{ 0, 0, int(windowSize.x), int(windowSize.y) });
					builder.SetViewport(Nz::Recti{ 0, 0, int(windowSize.x), int(windowSize.y) });

					builder.DrawIndexed(meshIB->GetIndexCount());

					debugDrawer.Draw(builder);
				}
				builder.EndRenderPass();
			}
			builder.EndDebugRegion();

		}, Nz::QueueType::Graphics);

		frame.Present();

		// On incrémente le compteur de FPS improvisé
		fps++;

		if (secondClock.GetMilliseconds() >= 1000) // Toutes les secondes
		{
			// Et on insère ces données dans le titre de la fenêtre
			window.SetTitle(windowTitle + " - " + Nz::NumberToString(fps) + " FPS");

			/*
			Note: En C++11 il est possible d'insérer de l'Unicode de façon standard, quel que soit l'encodage du fichier,
			via quelque chose de similaire à u8"Cha\u00CEne de caract\u00E8res".
			Cependant, si le code source est encodé en UTF-8 (Comme c'est le cas dans ce fichier),
			cela fonctionnera aussi comme ceci : "Chaîne de caractères".
			*/

			// Et on réinitialise le compteur de FPS
			fps = 0;

			// Et on relance l'horloge pour refaire ça dans une seconde
			secondClock.Restart();
		}
	}

	return EXIT_SUCCESS;
}
