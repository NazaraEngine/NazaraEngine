#include <Nazara/Core.hpp>
#include <Nazara/Platform.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Renderer.hpp>
#include <chrono>
#include <iostream>
#include <thread>

int main(int argc, char* argv[])
{
	std::filesystem::path resourceDir = "assets/examples";
	if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory("../.." / resourceDir))
		resourceDir = "../.." / resourceDir;

	Nz::Application<Nz::Graphics> app(argc, argv);

	std::shared_ptr<Nz::RenderDevice> device = Nz::Graphics::Instance()->GetRenderDevice();

	auto& windowing = app.AddComponent<Nz::WindowingAppComponent>();

	std::string windowTitle = "Physically Based Rendering Test";
	Nz::Window& window = windowing.CreateWindow(Nz::VideoMode(1920, 1080), windowTitle);
	Nz::WindowSwapchain windowSwapchain(device, window);

	Nz::MeshParams meshParams;
	meshParams.center = true;
	meshParams.vertexRotation = Nz::EulerAnglesf(0.f, -90.f, 0.f);
	meshParams.vertexScale = Nz::Vector3f(0.002f);
	meshParams.vertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout::XYZ_Normal_UV_Tangent);

	std::shared_ptr<Nz::Mesh> sphereMesh = std::make_shared<Nz::Mesh>();
	sphereMesh->CreateStatic();
	sphereMesh->BuildSubMesh(Nz::Primitive::UVSphere(1.f, 50, 50));
	sphereMesh->SetMaterialCount(1);
	sphereMesh->GenerateNormalsAndTangents();

	std::shared_ptr<Nz::GraphicalMesh> gfxMesh = Nz::GraphicalMesh::BuildFromMesh(*sphereMesh);

	// Textures
	Nz::TextureParams texParams;
	texParams.renderDevice = device;

	Nz::TextureParams srgbTexParams = texParams;
	srgbTexParams.loadFormat = Nz::PixelFormat::RGBA8_sRGB;

	std::shared_ptr<Nz::Texture> normalMap = Nz::Texture::LoadFromFile(resourceDir / "Rusty/rustediron2_normal.png", texParams);

	std::shared_ptr<Nz::MaterialInstance> materialInstance = Nz::MaterialInstance::Instantiate(Nz::MaterialType::PhysicallyBased);
	materialInstance->SetTextureProperty("AlphaMap", Nz::Texture::LoadFromFile(resourceDir / "alphatile.png", texParams));
	materialInstance->SetTextureProperty("BaseColorMap", Nz::Texture::LoadFromFile(resourceDir / "Rusty/rustediron2_basecolor.png", srgbTexParams));
	materialInstance->SetTextureProperty("MetallicMap", Nz::Texture::LoadFromFile(resourceDir / "Rusty/rustediron2_metallic.png", texParams));
	materialInstance->SetTextureProperty("RoughnessMap", Nz::Texture::LoadFromFile(resourceDir / "Rusty/rustediron2_roughness.png", texParams));

	std::size_t normalMapProperty = materialInstance->FindTextureProperty("NormalMap");
	materialInstance->SetTextureProperty(normalMapProperty, normalMap);

	Nz::Model model(std::move(gfxMesh));
	for (std::size_t i = 0; i < model.GetSubMeshCount(); ++i)
		model.SetMaterial(i, materialInstance);

	Nz::Vector2ui windowSize = window.GetSize();

	Nz::Camera camera(std::make_shared<Nz::RenderWindow>(windowSwapchain));
	//camera.UpdateClearColor(Nz::Color::Gray);

	Nz::ViewerInstance& viewerInstance = camera.GetViewerInstance();
	viewerInstance.UpdateTargetSize(Nz::Vector2f(window.GetSize()));
	viewerInstance.UpdateProjViewMatrices(Nz::Matrix4f::Perspective(Nz::DegreeAnglef(70.f), float(windowSize.x) / windowSize.y, 0.1f, 1000.f), Nz::Matrix4f::Translate(Nz::Vector3f::Backward() * 1));
	viewerInstance.UpdateNearFarPlanes(0.1f, 1000.f);

	Nz::WorldInstancePtr modelInstance = std::make_shared<Nz::WorldInstance>();
	modelInstance->UpdateWorldMatrix(Nz::Matrix4f::Translate(Nz::Vector3f::Forward() * 2 + Nz::Vector3f::Left()));

	Nz::Recti scissorBox(Nz::Vector2i::Zero(), Nz::Vector2i(window.GetSize()));

	Nz::ElementRendererRegistry elementRegistry;
	Nz::ForwardFramePipeline framePipeline(elementRegistry);
	[[maybe_unused]] std::size_t cameraIndex = framePipeline.RegisterViewer(&camera, 0);
	std::size_t worldInstanceIndex1 = framePipeline.RegisterWorldInstance(modelInstance);
	framePipeline.RegisterRenderable(worldInstanceIndex1, Nz::FramePipeline::NoSkeletonInstance, &model, 0xFFFFFFFF, scissorBox);

	std::unique_ptr<Nz::DirectionalLight> light = std::make_unique<Nz::DirectionalLight>();
	light->UpdateRotation(Nz::EulerAnglesf(-45.f, 0.f, 0.f));

	framePipeline.RegisterLight(light.get(), 0xFFFFFFFF);

	Nz::Vector3f viewerPos = Nz::Vector3f::Zero();

	Nz::EulerAnglesf camAngles(0.f, 0.f, 0.f);
	Nz::Quaternionf camQuat(camAngles);

	Nz::MillisecondClock updateClock;
	Nz::MillisecondClock fpsClock;
	unsigned int fps = 0;

	Nz::Mouse::SetRelativeMouseMode(true);

	window.GetEventHandler().OnEvent.Connect([&](const Nz::WindowEventHandler*, const Nz::WindowEvent& event)
	{
		switch (event.type)
		{
			case Nz::WindowEventType::KeyPressed:
				if (event.key.virtualKey == Nz::Keyboard::VKey::N)
				{
					if (materialInstance->GetTextureProperty(normalMapProperty))
						materialInstance->SetTextureProperty(normalMapProperty, {});
					else
						materialInstance->SetTextureProperty(normalMapProperty, normalMap);
				}
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
				//light->UpdateRotation(camQuat);
				break;
			}

			case Nz::WindowEventType::Resized:
			{
				Nz::Vector2ui newWindowSize = window.GetSize();
				viewerInstance.UpdateProjectionMatrix(Nz::Matrix4f::Perspective(Nz::DegreeAnglef(70.f), float(newWindowSize.x) / newWindowSize.y, 0.1f, 1000.f));
				viewerInstance.UpdateTargetSize(Nz::Vector2f(newWindowSize));
				break;
			}

			default:
				break;
		}
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

			//light->UpdatePosition(viewerPos);
		}

		Nz::RenderFrame frame = windowSwapchain.AcquireFrame();
		if (!frame)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			return;
		}

		viewerInstance.UpdateViewMatrix(Nz::Matrix4f::TransformInverse(viewerPos, camAngles));
		viewerInstance.UpdateEyePosition(viewerPos);

		framePipeline.Render(frame);

		frame.Present();

		// On incrémente le compteur de FPS improvisé
		fps++;

		if (fpsClock.RestartIfOver(Nz::Time::Second()))
		{
			window.SetTitle(windowTitle + " - " + Nz::NumberToString(fps) + " FPS");
			fps = 0;
		}
	});

	return app.Run();
}
