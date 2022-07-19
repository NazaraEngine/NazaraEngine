#include <Nazara/Core.hpp>
#include <Nazara/Core/Systems.hpp>
#include <Nazara/Platform.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Graphics/TextSprite.hpp>
#include <Nazara/Graphics/Components.hpp>
#include <Nazara/Graphics/Systems.hpp>
#include <Nazara/Math/PidController.hpp>
#include <Nazara/Physics2D.hpp>
#include <Nazara/Physics2D/Components.hpp>
#include <Nazara/Physics2D/Systems.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
#include <Nazara/Utility/Components.hpp>
#include <Nazara/Widgets.hpp>
#include <entt/entt.hpp>
#include <array>
#include <chrono>
#include <iostream>
#include <limits>
#include <thread>

NAZARA_REQUEST_DEDICATED_GPU()

int main()
{
	std::filesystem::path resourceDir = "assets/examples";
	if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory("../.." / resourceDir))
		resourceDir = "../.." / resourceDir;

	Nz::Renderer::Config rendererConfig;
	std::cout << "Run using Vulkan? (y/n)" << std::endl;
	if (std::getchar() != 'n')
		rendererConfig.preferredAPI = Nz::RenderAPI::Vulkan;
	else
		rendererConfig.preferredAPI = Nz::RenderAPI::OpenGL;

	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	Nz::Modules<Nz::Graphics, Nz::Physics2D> nazara(rendererConfig);

	std::shared_ptr<Nz::RenderDevice> device = Nz::Graphics::Instance()->GetRenderDevice();

	entt::registry registry;

	Nz::SystemGraph systemGraph(registry);
	Nz::Physics2DSystem& physSytem = systemGraph.AddSystem<Nz::Physics2DSystem>();
	Nz::RenderSystem& renderSystem = systemGraph.AddSystem<Nz::RenderSystem>();

	std::string windowTitle = "Graphics Test";
	Nz::RenderWindow& window = renderSystem.CreateWindow(device, Nz::VideoMode(1920, 1080), windowTitle);

	Nz::Vector2ui windowSize = window.GetSize();

	physSytem.GetPhysWorld().SetGravity({ 0.f, -9.81f });

	entt::entity viewer = registry.create();
	{
		registry.emplace<Nz::NodeComponent>(viewer);
		auto& cameraComponent = registry.emplace<Nz::CameraComponent>(viewer, window.GetRenderTarget(), Nz::ProjectionType::Orthographic);
		cameraComponent.UpdateRenderMask(1);
		cameraComponent.UpdateClearColor(Nz::Color(0.5f, 0.5f, 0.5f));
	}

	std::shared_ptr<Nz::Material> material = std::make_shared<Nz::Material>();

	std::shared_ptr<Nz::MaterialPass> materialPass = std::make_shared<Nz::MaterialPass>(Nz::BasicMaterial::GetSettings());
	material->AddPass("ForwardPass", materialPass);

	Nz::TextureSamplerInfo samplerInfo;
	samplerInfo.anisotropyLevel = 8;

	Nz::TextureParams texParams;
	texParams.renderDevice = device;
	texParams.loadFormat = Nz::PixelFormat::RGBA8_SRGB;

	Nz::BasicMaterial basicMat(*materialPass);
	basicMat.SetBaseColorMap(Nz::Texture::LoadFromFile(resourceDir / "Spaceship/Texture/diffuse.png", texParams));
	basicMat.SetBaseColorSampler(samplerInfo);

	for (std::size_t y = 0; y < 10; ++y)
	{
		for (std::size_t x = 0; x < 10; ++x)
		{
			entt::entity spriteEntity = registry.create();
			{
				std::shared_ptr<Nz::Sprite> sprite = std::make_shared<Nz::Sprite>(material);
				sprite->SetSize({ 32.f, 32.f });
				sprite->SetOrigin({ 16.f, 16.f, 0.f });

				registry.emplace<Nz::NodeComponent>(spriteEntity).SetPosition(1920 / 2 + x * 36.f, 1080 / 2 + y * 36.f);

				registry.emplace<Nz::GraphicsComponent>(spriteEntity).AttachRenderable(sprite, 1);
				auto& rigidBody = registry.emplace<Nz::RigidBody2DComponent>(spriteEntity, physSytem.CreateRigidBody(50.f, std::make_shared<Nz::BoxCollider2D>(Nz::Vector2f(32.f, 32.f))));
				rigidBody.SetElasticity(0.99f);
			}
		}
	}

	entt::entity groundEntity = registry.create();
	{
		std::shared_ptr<Nz::Material> whiteMaterial = std::make_shared<Nz::Material>();

		std::shared_ptr<Nz::MaterialPass> materialPass = std::make_shared<Nz::MaterialPass>(Nz::BasicMaterial::GetSettings());
		whiteMaterial->AddPass("ForwardPass", materialPass);

		std::shared_ptr<Nz::Sprite> sprite = std::make_shared<Nz::Sprite>(whiteMaterial);
		sprite->SetSize({ 800.f, 20.f });
		sprite->SetOrigin({ 400.f, 10.f, 0.f });

		registry.emplace<Nz::NodeComponent>(groundEntity).SetPosition(1920.f / 2.f, 50.f);
		registry.emplace<Nz::GraphicsComponent>(groundEntity).AttachRenderable(sprite, 1);
		auto& rigidBody = registry.emplace<Nz::RigidBody2DComponent>(groundEntity, physSytem.CreateRigidBody(0.f, std::make_shared<Nz::BoxCollider2D>(Nz::Vector2f(800.f, 20.f))));
		rigidBody.SetElasticity(0.99f);
	}

	Nz::EulerAnglesf camAngles(0.f, 0.f, 0.f);
	Nz::Quaternionf camQuat(camAngles);

	window.EnableEventPolling(true);

	Nz::Clock updateClock;
	Nz::Clock secondClock;
	unsigned int fps = 0;

	//Nz::Mouse::SetRelativeMouseMode(true);

	float elapsedTime = 0.f;
	Nz::UInt64 time = Nz::GetElapsedMicroseconds();

	Nz::PidController<Nz::Vector3f> headingController(0.5f, 0.f, 0.05f);
	Nz::PidController<Nz::Vector3f> upController(1.f, 0.f, 0.1f);

	bool showColliders = false;
	while (window.IsOpen())
	{
		Nz::UInt64 now = Nz::GetElapsedMicroseconds();
		elapsedTime = (now - time) / 1'000'000.f;
		time = now;

		Nz::WindowEvent event;
		while (window.PollEvent(&event))
		{
			switch (event.type)
			{
				case Nz::WindowEventType::Quit:
					window.Close();
					break;

				case Nz::WindowEventType::KeyPressed:
					break;

				case Nz::WindowEventType::MouseMoved:
					break;

				default:
					break;
			}
		}

		if (updateClock.GetMilliseconds() > 1000 / 60)
		{
			float updateTime = updateClock.Restart() / 1'000'000.f;

			physSytem.Update(1000.f / 60.f);
		}

		systemGraph.Update();

		fps++;

		if (secondClock.GetMilliseconds() >= 1000)
		{
			window.SetTitle(windowTitle + " - " + Nz::NumberToString(fps) + " FPS" + " - " + Nz::NumberToString(registry.alive()) + " entities");

			fps = 0;

			secondClock.Restart();
		}
	}

	return EXIT_SUCCESS;
}
