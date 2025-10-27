#include <Nazara/Core.hpp>
#include <Nazara/Platform.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Renderer.hpp>
#include <entt/entt.hpp>
#include <array>
#include <chrono>
#include <iostream>
#include <random>

int main()
{
	std::filesystem::path resourceDir = "assets";
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

	Nz::Application<Nz::Graphics> app(rendererConfig);
	auto& windowingApp = app.AddComponent<Nz::WindowingAppComponent>();
	auto& fsApp = app.AddComponent<Nz::FilesystemAppComponent>();
	fsApp.Mount("assets", resourceDir);

	auto& ecsApp = app.AddComponent<Nz::EntitySystemAppComponent>();

	auto& world = ecsApp.AddWorld<Nz::EnttWorld>();
	auto& renderSystem = world.AddSystem<Nz::RenderSystem>();

	std::string windowTitle = "Deferred Test";
	Nz::Window& window = windowingApp.CreateWindow(Nz::VideoMode(1920, 1080, 32), windowTitle);
	Nz::WindowSwapchain& windowSwapchain = renderSystem.CreateSwapchain(window);

	Nz::ModelParams modelParams;
	modelParams.mesh.center = true;
	modelParams.mesh.vertexRotation = Nz::EulerAnglesf(0.f, -90.f, 0.f);
	modelParams.mesh.vertexScale = Nz::Vector3f(0.002f);
	modelParams.mesh.vertexDeclaration = Nz::VertexDeclaration::Get(Nz::VertexLayout::XYZ_Normal_UV);

	std::shared_ptr<Nz::Model> spaceshipModel = fsApp.Load<Nz::Model>("assets/examples/Spaceship/spaceship.obj", modelParams);
	if (!spaceshipModel)
	{
		NazaraError("failed to load model");
		return __LINE__;
	}

	std::shared_ptr<Nz::MaterialInstance> materialInstance = spaceshipModel->GetMaterial(0);
	std::shared_ptr<Nz::MaterialInstance> materialInstance2 = Nz::MaterialInstance::Instantiate(Nz::MaterialType::PhysicallyBased);
	materialInstance2->SetValueProperty(0, Nz::Color::Green());

	Nz::Vector2ui windowSize = window.GetSize();

	entt::handle camera = world.CreateEntity();
	{
		camera.emplace<Nz::NodeComponent>(Nz::Vector3f::Backward() * 2.f);

		auto pipelinePass = fsApp.Load<Nz::PipelinePassList>("assets/passes/deferred.passlist");

		auto& cameraComponent = camera.emplace<Nz::CameraComponent>(std::make_shared<Nz::RenderWindow>(windowSwapchain), std::move(pipelinePass));
		cameraComponent.UpdateClearColor(Nz::Color::Gray());
		cameraComponent.UpdateZNear(0.1f);
	}

	entt::handle spaceship1 = world.CreateEntity();
	{
		spaceship1.emplace<Nz::NodeComponent>(Nz::Vector3f::Left());
		spaceship1.emplace<Nz::GraphicsComponent>(spaceshipModel);
	}

	entt::handle spaceship2 = world.CreateEntity();
	{
		spaceship2.emplace<Nz::NodeComponent>(Nz::Vector3f::Right());
		spaceship2.emplace<Nz::GraphicsComponent>(spaceshipModel);
	}

	entt::handle light = world.CreateEntity();
	{
		auto& lightNode = light.emplace<Nz::NodeComponent>();
		lightNode.SetParent(camera);

		auto& lightComponent = light.emplace<Nz::LightComponent>();

		auto& directionalLight = lightComponent.AddLight<Nz::DirectionalLight>();
		directionalLight.UpdateColor(Nz::Color::Red());

		auto& pointLight = lightComponent.AddLight<Nz::PointLight>();
		pointLight.UpdateColor(Nz::Color::Green());

		auto& spotLight = lightComponent.AddLight<Nz::SpotLight>();
		spotLight.UpdateColor(Nz::Color::Blue());
		spotLight.UpdateInnerAngle(Nz::DegreeAnglef(15.f));
		spotLight.UpdateOuterAngle(Nz::DegreeAnglef(20.f));
	}

	std::minstd_rand colorGen(0xDEADBEEF);

	constexpr float radius = 3.0f;
	for (std::size_t i = 0; i < 20; ++i)
	{
		std::uniform_real_distribution<float> dis(0.f, 360.f);

		Nz::TurnAnglef angle = i / 20.f;

		auto [sin, cos] = angle.GetSinCos();

		entt::handle spotlight = world.CreateEntity();

		auto& spotlightNode = spotlight.emplace<Nz::NodeComponent>();
		spotlightNode.SetPosition(Nz::Vector3f(sin * radius, 0.0f, cos * radius));
		spotlightNode.SetRotation(Nz::Quaternionf::RotationBetween(Nz::Vector3f::Forward(), -spotlightNode.GetPosition().GetNormal()));

		auto& lightComponent = spotlight.emplace<Nz::LightComponent>();

		auto& spotLight = lightComponent.AddLight<Nz::SpotLight>();
		spotLight.UpdateRadius(5.0f);
		spotLight.UpdateColor(Nz::Color::FromHSV(dis(colorGen), 1.0f, 1.0f));
		spotLight.UpdateInnerAngle(Nz::DegreeAnglef(15.f));
		spotLight.UpdateOuterAngle(Nz::DegreeAnglef(20.f));
	}


	Nz::EulerAnglesf camAngles(0.f, 0.f, 0.f);

	Nz::MillisecondClock updateClock;
	Nz::MillisecondClock fpsClock;
	unsigned int fps = 0;

	window.SetRelativeMouseMode(true);

	window.GetEventHandler().OnEvent.Connect([&](const Nz::WindowEventHandler*, const Nz::WindowEvent& event)
	{
		switch (event.type)
		{
			case Nz::WindowEventType::Quit:
				window.Close();
				break;

			case Nz::WindowEventType::KeyPressed:
				if (event.key.virtualKey == Nz::Keyboard::VKey::A)
				{
					for (std::size_t i = 0; i < spaceshipModel->GetSubMeshCount(); ++i)
						spaceshipModel->SetMaterial(i, materialInstance);
				}
				else if (event.key.virtualKey == Nz::Keyboard::VKey::B)
				{
					for (std::size_t i = 0; i < spaceshipModel->GetSubMeshCount(); ++i)
						spaceshipModel->SetMaterial(i, materialInstance2);
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

				camera.get<Nz::NodeComponent>().SetRotation(camAngles);
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

			auto& cameraNode = camera.get<Nz::NodeComponent>();

			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Up) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Z))
				cameraNode.Move(Nz::Vector3f::Forward() * cameraSpeed);

			// Si la flèche du bas ou la touche S est pressée, on recule
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Down) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::S))
				cameraNode.Move(Nz::Vector3f::Backward() * cameraSpeed);

			// Etc...
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Left) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Q))
				cameraNode.Move(Nz::Vector3f::Left() * cameraSpeed);

			// Etc...
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::Right) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::D))
				cameraNode.Move(Nz::Vector3f::Right() * cameraSpeed);

			// Majuscule pour monter, notez l'utilisation d'une direction globale (Non-affectée par la rotation)
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::LShift) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::RShift))
				cameraNode.MoveGlobal(Nz::Vector3f::Up() * cameraSpeed);

			// Contrôle (Gauche ou droite) pour descendre dans l'espace global, etc...
			if (Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::LControl) || Nz::Keyboard::IsKeyPressed(Nz::Keyboard::VKey::RControl))
				cameraNode.MoveGlobal(Nz::Vector3f::Down() * cameraSpeed);
		}

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
