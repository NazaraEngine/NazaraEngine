#include <Nazara/Core.hpp>
#include <Nazara/Platform.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Shader.hpp>
#include <Nazara/Shader/Ast/SanitizeVisitor.hpp>
#include <Nazara/Utility.hpp>
#include <array>
#include <iostream>

NAZARA_REQUEST_DEDICATED_GPU()

const char barModuleSource[] = R"(
[nzsl_version("1.0")]
[uuid("4BB09DEE-F70A-442E-859F-E8F2F3F8583D")]
module;

fn dummy() {}

[export]
[layout(std140)]
struct Bar
{
}
)";

const char dataModuleSource[] = R"(
[nzsl_version("1.0")]
[uuid("E49DC9AD-469C-462C-9719-A6F012372029")]
module;

import Test/Bar;

struct Foo {}

[export]
[layout(std140)]
struct Data
{
	projectionMatrix: mat4[f32],
	worldMatrix: mat4[f32],
	viewMatrix: mat4[f32],
	pilier: Bar
}
)";

const char shaderSource[] = R"(
[nzsl_version("1.0")]
module;

import Test/Data;
import Test/Bar;

option red: bool = false;

[set(0)]
external
{
	[binding(0)] viewerData: uniform[Data]
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
	vertOut.position = viewerData.projectionMatrix * viewerData.viewMatrix * viewerData.worldMatrix * vec4[f32](vertIn.position, 1.0);
	vertOut.normal = vertIn.normal;
	vertOut.uv = vertIn.uv;

	return vertOut;
}
)";

int main()
{
	std::filesystem::path resourceDir = "resources";
	if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory(".." / resourceDir))
		resourceDir = ".." / resourceDir;

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

	Nz::ShaderAst::ModulePtr shaderModule = Nz::ShaderLang::Parse(std::string_view(shaderSource, sizeof(shaderSource)));
	if (!shaderModule)
	{
		std::cout << "Failed to parse shader module" << std::endl;
		return __LINE__;
	}

	Nz::ShaderAst::SanitizeVisitor::Options sanitizeOpt;
	sanitizeOpt.moduleCallback = [](const std::vector<std::string>& modulePath) -> Nz::ShaderAst::ModulePtr
	{
		if (modulePath.size() != 2)
			return {};

		if (modulePath[0] != "Test")
			return {};

		if (modulePath[1] == "Bar")
			return Nz::ShaderLang::Parse(std::string_view(barModuleSource, sizeof(barModuleSource)));
		else if (modulePath[1] == "Data")
			return Nz::ShaderLang::Parse(std::string_view(dataModuleSource, sizeof(dataModuleSource)));
		else
			return {};
	};

	shaderModule = Nz::ShaderAst::Sanitize(*shaderModule, sanitizeOpt);
	if (!shaderModule)
	{
		std::cout << "Failed to compile shader module" << std::endl;
		return __LINE__;
	}

	Nz::LangWriter langWriter;
	std::string output = langWriter.Generate(*shaderModule);
	std::cout << output << std::endl;
	assert(Nz::ShaderAst::Sanitize(*Nz::ShaderLang::Parse(output)));

	Nz::ShaderWriter::States states;
	states.optimize = true;

	auto fragVertShader = device->InstantiateShaderModule(Nz::ShaderStageType::Fragment | Nz::ShaderStageType::Vertex, *shaderModule, states);
	if (!fragVertShader)
	{
		std::cout << "Failed to instantiate shader" << std::endl;
		return __LINE__;
	}

	Nz::MeshParams meshParams;
	meshParams.bufferFactory = Nz::GetRenderBufferFactory(device);
	meshParams.center = true;
	meshParams.matrix = Nz::Matrix4f::Rotate(Nz::EulerAnglesf(0.f, -90.f, 0.f)) * Nz::Matrix4f::Scale(Nz::Vector3f(0.002f));
	meshParams.vertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout::XYZ_Normal_UV);

	std::shared_ptr<Nz::Mesh> drfreak = Nz::Mesh::LoadFromFile(resourceDir / "Spaceship/spaceship.obj", meshParams);
	if (!drfreak)
	{
		NazaraError("Failed to load model");
		return __LINE__;
	}

	std::shared_ptr<Nz::StaticMesh> spaceshipMesh = std::static_pointer_cast<Nz::StaticMesh>(drfreak->GetSubMesh(0));

	const std::shared_ptr<Nz::VertexBuffer>& meshVB = spaceshipMesh->GetVertexBuffer();
	const std::shared_ptr<const Nz::IndexBuffer>& meshIB = spaceshipMesh->GetIndexBuffer();

	// Index buffer
	std::cout << "Index count: " << meshIB->GetIndexCount() << std::endl;

	// Vertex buffer
	std::cout << "Vertex count: " << meshVB->GetVertexCount() << std::endl;

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
	uboBinding.shaderStageFlags = Nz::ShaderStageType::Vertex;
	uboBinding.type = Nz::ShaderBindingType::UniformBuffer;

	std::shared_ptr<Nz::RenderPipelineLayout> basePipelineLayout = device->InstantiateRenderPipelineLayout(pipelineLayoutInfo);

	auto& textureBinding = pipelineLayoutInfo.bindings.emplace_back();
	textureBinding.setIndex = 1;
	textureBinding.bindingIndex = 0;
	textureBinding.shaderStageFlags = Nz::ShaderStageType::Fragment;
	textureBinding.type = Nz::ShaderBindingType::Texture;

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

	textureShaderBinding->Update({
		{
			0,
			Nz::ShaderBinding::TextureBinding {
				texture.get(), textureSampler.get()
			}
		}
	});

	Nz::RenderPipelineInfo pipelineInfo;
	pipelineInfo.pipelineLayout = renderPipelineLayout;
	pipelineInfo.faceCulling = true;

	pipelineInfo.depthBuffer = true;
	pipelineInfo.shaderModules.emplace_back(fragVertShader);

	auto& pipelineVertexBuffer = pipelineInfo.vertexBuffers.emplace_back();
	pipelineVertexBuffer.binding = 0;
	pipelineVertexBuffer.declaration = meshVB->GetVertexDeclaration();

	std::shared_ptr<Nz::RenderPipeline> pipeline = device->InstantiateRenderPipeline(pipelineInfo);

	const std::shared_ptr<Nz::RenderDevice>& renderDevice = window.GetRenderDevice();

	std::shared_ptr<Nz::CommandPool> commandPool = renderDevice->InstantiateCommandPool(Nz::QueueType::Graphics);

	Nz::RenderBuffer& renderBufferIB = static_cast<Nz::RenderBuffer&>(*meshIB->GetBuffer());
	Nz::RenderBuffer& renderBufferVB = static_cast<Nz::RenderBuffer&>(*meshVB->GetBuffer());

	Nz::Vector3f viewerPos = Nz::Vector3f::Zero();

	Nz::EulerAnglesf camAngles(0.f, 0.f, 0.f);
	Nz::Quaternionf camQuat(camAngles);

	window.EnableEventPolling(true);

	Nz::Clock updateClock;
	Nz::Clock secondClock;
	unsigned int fps = 0;
	bool uboUpdate = true;

	Nz::Mouse::SetRelativeMouseMode(true);

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
					Nz::Vector2ui windowSize = window.GetSize();
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
			continue;

		ubo.viewMatrix = Nz::Matrix4f::ViewMatrix(viewerPos, camAngles);

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
					builder.BindIndexBuffer(renderBufferIB);
					builder.BindPipeline(*pipeline);
					builder.BindVertexBuffer(0, renderBufferVB);
					builder.BindShaderBinding(0, *viewerShaderBinding);
					builder.BindShaderBinding(1, *textureShaderBinding);

					builder.SetScissor(Nz::Recti{ 0, 0, int(windowSize.x), int(windowSize.y) });
					builder.SetViewport(Nz::Recti{ 0, 0, int(windowSize.x), int(windowSize.y) });

					builder.DrawIndexed(meshIB->GetIndexCount());
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
