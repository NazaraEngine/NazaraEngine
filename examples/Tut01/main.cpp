// Sources pour https://github.com/NazaraEngine/NazaraEngine/wiki/(FR)-Tutoriel:-%5B01%5D-Hello-World

#include <Nazara/Core.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Platform/AppWindowingComponent.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
#include <iostream>

int main()
{
	// Mise en place de l'application, de la fenêtre et du monde
	Nz::Application<Nz::Graphics> app;

	auto& windowing = app.AddComponent<Nz::AppWindowingComponent>();
	Nz::Window& mainWindow = windowing.CreateWindow(Nz::VideoMode(1280, 720), "Tut01 - Hello world");

	auto& ecs = app.AddComponent<Nz::AppEntitySystemComponent>();
	auto& world = ecs.AddWorld<Nz::EnttWorld>();

	auto& renderSystem = world.AddSystem<Nz::RenderSystem>();
	Nz::WindowSwapchain& windowSwapchain = renderSystem.CreateSwapchain(mainWindow);

	// Création de la caméra
	entt::handle cameraEntity = world.CreateEntity();
	{
		cameraEntity.emplace<Nz::NodeComponent>();

		auto& cameraComponent = cameraEntity.emplace<Nz::CameraComponent>(&windowSwapchain, Nz::ProjectionType::Orthographic);
		cameraComponent.UpdateClearColor(Nz::Color(0.46f, 0.48f, 0.84f, 1.f));
	}

	// Création d'un texte
	Nz::SimpleTextDrawer textDrawer;
	textDrawer.SetText("Hello world !");
	textDrawer.SetCharacterSize(72);
	textDrawer.SetOutlineThickness(4.f);

	std::shared_ptr<Nz::TextSprite> textSprite = std::make_shared<Nz::TextSprite>();
	textSprite->Update(textDrawer);

	entt::handle textEntity = world.CreateEntity();
	{
		auto& nodeComponent = textEntity.emplace<Nz::NodeComponent>();

		auto& gfxComponent = textEntity.emplace<Nz::GraphicsComponent>();
		gfxComponent.AttachRenderable(textSprite);

		Nz::Boxf textBox = textSprite->GetAABB();
		Nz::Vector2ui windowSize = mainWindow.GetSize();
		nodeComponent.SetPosition(windowSize.x / 2 - textBox.width / 2, windowSize.y / 2 - textBox.height / 2);
	}

	return app.Run();
}
