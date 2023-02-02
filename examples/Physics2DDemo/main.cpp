#include <Nazara/Core.hpp>
#include <Nazara/Platform.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Math/PidController.hpp>
#include <Nazara/Physics2D.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
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

	Nz::Application<Nz::Graphics, Nz::Physics2D> app;

	auto& windowing = app.AddComponent<Nz::AppWindowingComponent>();

	auto& ecs = app.AddComponent<Nz::AppEntitySystemComponent>();

	auto& world = ecs.AddWorld<Nz::EnttWorld>();
	Nz::Physics2DSystem& physSytem = world.AddSystem<Nz::Physics2DSystem>();
	Nz::RenderSystem& renderSystem = world.AddSystem<Nz::RenderSystem>();

	std::string windowTitle = "Physics 2D";
	Nz::Window& window = windowing.CreateWindow(Nz::VideoMode(1920, 1080, 32), windowTitle);
	Nz::WindowSwapchain& windowSwapchain = renderSystem.CreateSwapchain(window);

	std::shared_ptr<Nz::RenderDevice> device = Nz::Graphics::Instance()->GetRenderDevice();

	Nz::Vector2f windowSize = Nz::Vector2f(window.GetSize());

	physSytem.GetPhysWorld().SetGravity({ 0.f, -98.1f });

	entt::handle viewer = world.CreateEntity();
	{
		viewer.emplace<Nz::NodeComponent>();
		auto& cameraComponent = viewer.emplace<Nz::CameraComponent>(&windowSwapchain, Nz::ProjectionType::Orthographic);
		cameraComponent.UpdateRenderMask(1);
		cameraComponent.UpdateClearColor(Nz::Color(0.5f, 0.5f, 0.5f));
	}

	Nz::TextureSamplerInfo samplerInfo;
	samplerInfo.anisotropyLevel = 8;

	Nz::TextureParams texParams;
	texParams.renderDevice = device;
	texParams.loadFormat = Nz::PixelFormat::RGBA8;

	std::shared_ptr<Nz::MaterialInstance> spriteMaterial = Nz::Graphics::Instance()->GetDefaultMaterials().phongMaterial->Instantiate();
	spriteMaterial->SetTextureProperty("BaseColorMap", Nz::Texture::LoadFromFile(resourceDir / "box.png", texParams));

	for (std::size_t y = 0; y < 30; ++y)
	{
		for (std::size_t x = 0; x < 30; ++x)
		{
			entt::handle spriteEntity = world.CreateEntity();
			{
				std::shared_ptr<Nz::Sprite> sprite = std::make_shared<Nz::Sprite>(spriteMaterial);
				sprite->SetSize({ 32.f, 32.f });
				sprite->SetOrigin({ 0.5f, 0.5f });

				spriteEntity.emplace<Nz::NodeComponent>().SetPosition(windowSize.x * 0.5f + x * 48.f - 15.f * 48.f, windowSize.y / 2 + y * 48.f);

				spriteEntity.emplace<Nz::GraphicsComponent>().AttachRenderable(sprite, 1);
				auto& rigidBody = spriteEntity.emplace<Nz::RigidBody2DComponent>(physSytem.CreateRigidBody(50.f, std::make_shared<Nz::BoxCollider2D>(Nz::Vector2f(32.f, 32.f))));
				rigidBody.SetFriction(0.9f);
				//rigidBody.SetElasticity(0.99f);
			}
		}
	}

	entt::handle groundEntity = world.CreateEntity();
	{
		std::shared_ptr<Nz::Tilemap> tilemap = std::make_shared<Nz::Tilemap>(Nz::Vector2ui(40, 20), Nz::Vector2f(64.f, 64.f), 18);
		tilemap->SetOrigin({ 0.5f, 0.5f });
		for (std::size_t i = 0; i < 18; ++i)
		{
			std::shared_ptr<Nz::MaterialInstance> tileMaterial = Nz::Graphics::Instance()->GetDefaultMaterials().basicTransparent->Clone();
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

		groundEntity.emplace<Nz::NodeComponent>().SetPosition(windowSize.x * 0.5f, -windowSize.y * 0.2f);
		groundEntity.emplace<Nz::GraphicsComponent>().AttachRenderable(tilemap, 1);
		auto& rigidBody = groundEntity.emplace<Nz::RigidBody2DComponent>(physSytem.CreateRigidBody(0.f, std::make_shared<Nz::BoxCollider2D>(tilemap->GetSize())));
		rigidBody.SetFriction(0.9f);
	}

	Nz::EulerAnglesf camAngles(0.f, 0.f, 0.f);
	Nz::Quaternionf camQuat(camAngles);

	Nz::MillisecondClock secondClock;
	unsigned int fps = 0;

	//Nz::Mouse::SetRelativeMouseMode(true);

	float elapsedTime = 0.f;

	Nz::PidController<Nz::Vector3f> headingController(0.5f, 0.f, 0.05f);
	Nz::PidController<Nz::Vector3f> upController(1.f, 0.f, 0.1f);

	app.AddUpdater([&](Nz::Time elapsedTime)
	{
		fps++;

		if (secondClock.RestartIfOver(Nz::Time::Second()))
		{
			window.SetTitle(windowTitle + " - " + Nz::NumberToString(fps) + " FPS" + " - " + Nz::NumberToString(world.GetRegistry().alive()) + " entities");
			fps = 0;
		}
	});

	return app.Run();
}
