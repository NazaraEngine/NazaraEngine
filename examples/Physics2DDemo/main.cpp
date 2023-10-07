#include <Nazara/Core.hpp>
#include <Nazara/Platform.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Math/PidController.hpp>
#include <Nazara/Physics2D.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Widgets.hpp>
#include <entt/entt.hpp>
#include <array>
#include <chrono>
#include <iostream>
#include <limits>
#include <thread>

NAZARA_REQUEST_DEDICATED_GPU()

int main(int argc, char* argv[])
{
	std::filesystem::path resourceDir = "assets/examples";
	if (!std::filesystem::is_directory(resourceDir) && std::filesystem::is_directory("../.." / resourceDir))
		resourceDir = "../.." / resourceDir;

	Nz::Application<Nz::Graphics, Nz::Physics2D> app(argc, argv);

	auto& windowing = app.AddComponent<Nz::WindowingAppComponent>();

	auto& ecs = app.AddComponent<Nz::EntitySystemAppComponent>();

	auto& world = ecs.AddWorld<Nz::EnttWorld>();
	Nz::Physics2DSystem& physSytem = world.AddSystem<Nz::Physics2DSystem>();
	Nz::RenderSystem& renderSystem = world.AddSystem<Nz::RenderSystem>();

	std::string windowTitle = "Physics 2D";
	Nz::Window& window = windowing.CreateWindow(Nz::VideoMode(1920, 1080, 32), windowTitle);
	Nz::WindowSwapchain& windowSwapchain = renderSystem.CreateSwapchain(window);

	std::shared_ptr<Nz::RenderDevice> device = Nz::Graphics::Instance()->GetRenderDevice();

	Nz::Vector2f windowSize = Nz::Vector2f(window.GetSize());

	physSytem.GetPhysWorld().SetGravity({ 0.f, -313.f });

	entt::handle viewer = world.CreateEntity();
	{
		viewer.emplace<Nz::NodeComponent>();
		auto& cameraComponent = viewer.emplace<Nz::CameraComponent>(std::make_shared<Nz::RenderWindow>(windowSwapchain), Nz::ProjectionType::Orthographic);
		cameraComponent.UpdateRenderMask(1);
		cameraComponent.UpdateClearColor(Nz::Color(0.5f, 0.5f, 0.5f));
	}

	Nz::TextureSamplerInfo samplerInfo;
	samplerInfo.anisotropyLevel = 8;

	Nz::TextureParams texParams;
	texParams.renderDevice = device;
	texParams.loadFormat = Nz::PixelFormat::RGBA8Unorm;

	std::shared_ptr<Nz::MaterialInstance> spriteMaterial = Nz::MaterialInstance::Instantiate(Nz::MaterialType::Phong);
	spriteMaterial->SetTextureProperty("BaseColorMap", Nz::Texture::LoadFromFile(resourceDir / "box.png", texParams));

	Nz::RigidBody2DComponent::DynamicSettings boxSettings;
	boxSettings.mass = 50.f;
	boxSettings.geom = std::make_shared<Nz::BoxCollider2D>(Nz::Vector2f(32.f, 32.f));

	std::shared_ptr<Nz::Sprite> boxSprite = std::make_shared<Nz::Sprite>(spriteMaterial);
	boxSprite->SetSize({ 32.f, 32.f });
	boxSprite->SetOrigin({ 0.5f, 0.5f });

	for (std::size_t y = 0; y < 30; ++y)
	{
		for (std::size_t x = 0; x < 30; ++x)
		{
			entt::handle spriteEntity = world.CreateEntity();

			spriteEntity.emplace<Nz::NodeComponent>(Nz::Vector2f(windowSize.x * 0.5f + x * 32.f - 15.f * 32.f, windowSize.y / 2 + y * 48.f));
			spriteEntity.emplace<Nz::GraphicsComponent>(boxSprite, 1);

			auto& rigidBody = spriteEntity.emplace<Nz::RigidBody2DComponent>(boxSettings);
			rigidBody.SetFriction(0.9f);
			//rigidBody.SetElasticity(0.99f);
		}
	}

	entt::handle groundEntity = world.CreateEntity();
	{
		std::shared_ptr<Nz::Tilemap> tilemap = std::make_shared<Nz::Tilemap>(Nz::Vector2ui(40, 20), Nz::Vector2f(64.f, 64.f), 18);
		tilemap->SetOrigin({ 0.5f, 0.5f });
		for (std::size_t i = 0; i < 18; ++i)
		{
			std::shared_ptr<Nz::MaterialInstance> tileMaterial = Nz::MaterialInstance::Instantiate(Nz::MaterialType::Basic, Nz::MaterialInstancePreset::Transparent);
			tileMaterial->SetTextureProperty("BaseColorMap", Nz::Texture::LoadFromFile(resourceDir / "tiles" / (std::to_string(i + 1) + ".png"), texParams));

			tilemap->SetMaterial(i, tileMaterial);
		}

		for (unsigned int y = 0; y < 20; ++y)
		{
			for (unsigned int x = 0; x < 40; ++x)
			{
				tilemap->EnableTile({ x, y }, Nz::Rectf{ 0.f, 0.f, 1.f, 1.f }, Nz::Color::White(), (y == 0) ? 1 : 4);
			}
		}

		Nz::RigidBody2DComponent::StaticSettings groundSettings;
		groundSettings.geom = std::make_shared<Nz::BoxCollider2D>(tilemap->GetSize());

		groundEntity.emplace<Nz::NodeComponent>().SetPosition({ windowSize.x * 0.5f, -windowSize.y * 0.2f });
		groundEntity.emplace<Nz::GraphicsComponent>().AttachRenderable(tilemap, 1);
		auto& rigidBody = groundEntity.emplace<Nz::RigidBody2DComponent>(groundSettings);
		rigidBody.SetFriction(0.9f);
	}

	Nz::EulerAnglesf camAngles(0.f, 0.f, 0.f);
	Nz::Quaternionf camQuat(camAngles);

	Nz::MillisecondClock secondClock;
	unsigned int fps = 0;

	//Nz::Mouse::SetRelativeMouseMode(true);

	Nz::PidController<Nz::Vector3f> headingController(0.5f, 0.f, 0.05f);
	Nz::PidController<Nz::Vector3f> upController(1.f, 0.f, 0.1f);
	
	std::optional<Nz::PhysPivotConstraint2D> grabConstraint;
	NazaraSlot(Nz::WindowEventHandler, OnMouseMoved, grabbedObjectMove);

	Nz::WindowEventHandler& eventHandler = window.GetEventHandler();
	eventHandler.OnMouseButtonPressed.Connect([&](const Nz::WindowEventHandler*, const Nz::WindowEvent::MouseButtonEvent& event)
	{
		if (event.button == Nz::Mouse::Left)
		{
			auto& viewerComponent = viewer.get<Nz::CameraComponent>();

			Nz::Vector2f worldPos = Nz::Vector2f(viewerComponent.Unproject(Nz::Vector3f(float(event.x), float(event.y), 0.f)));

			entt::handle nearestEntity;
			if (physSytem.NearestBodyQuery(worldPos, 1.f, 0, 0xFFFFFFFF, 0xFFFFFFFF, &nearestEntity))
			{
				if (nearestEntity && nearestEntity != groundEntity)
				{
					grabConstraint.emplace(nearestEntity.get<Nz::RigidBody2DComponent>(), worldPos);

					grabbedObjectMove.Connect(eventHandler.OnMouseMoved, [&, nearestEntity, viewer](const Nz::WindowEventHandler*, const Nz::WindowEvent::MouseMoveEvent& event)
					{
						auto& viewerComponent = viewer.get<Nz::CameraComponent>();
						Nz::Vector2f worldPos = Nz::Vector2f(viewerComponent.Unproject(Nz::Vector3f(event.x, event.y, 0.f)));

						grabConstraint->SetFirstAnchor(worldPos);
					});
				}
			}
		}
	});

	eventHandler.OnMouseButtonReleased.Connect([&](const Nz::WindowEventHandler*, const Nz::WindowEvent::MouseButtonEvent& event)
	{
		if (event.button == Nz::Mouse::Left)
		{
			grabConstraint.reset();
			grabbedObjectMove.Disconnect();
		}
	});

	app.AddUpdaterFunc([&]
	{
		fps++;

		if (secondClock.RestartIfOver(Nz::Time::Second()))
		{
			window.SetTitle(windowTitle + " - " + Nz::NumberToString(fps) + " FPS" + " - " + Nz::NumberToString(world.GetAliveEntityCount()) + " entities");
			fps = 0;
		}
	});

	return app.Run();
}
