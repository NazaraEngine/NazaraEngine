// Sources pour https://github.com/NazaraEngine/NazaraEngine/wiki/(FR)-Tutoriel:-%5B02%5D-Signaux-et-gestion-des-%C3%A9v%C3%A9nements

#include <Nazara/Core.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Platform/WindowingAppComponent.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
#include <iostream>

int main(int argc, char* argv[])
{
	Nz::Application<Nz::Graphics> app(argc, argv);

	// Création de la fenêtre
	auto& windowing = app.AddComponent<Nz::WindowingAppComponent>();
	Nz::Window& mainWindow = windowing.CreateWindow(Nz::VideoMode(1280, 720), "Tut02 - Events");

	// Ajout d'un monde
	auto& ecs = app.AddComponent<Nz::EntitySystemAppComponent>();
	auto& world = ecs.AddWorld<Nz::EnttWorld>();

	Nz::RenderSystem& renderSystem = world.AddSystem<Nz::RenderSystem>();
	auto& windowSwapchain = renderSystem.CreateSwapchain(mainWindow);

	// Création de l'entité caméra
	entt::handle cameraEntity = world.CreateEntity();
	{
		cameraEntity.emplace<Nz::NodeComponent>();

		auto& cameraComponent = cameraEntity.emplace<Nz::CameraComponent>(std::make_shared<Nz::RenderWindow>(windowSwapchain), Nz::ProjectionType::Orthographic);
		cameraComponent.UpdateClearColor(Nz::Color(0.46f, 0.48f, 0.84f, 1.f));
	}

	Nz::WindowEventHandler& eventHandler = mainWindow.GetEventHandler();
	eventHandler.OnQuit.Connect([&](const Nz::WindowEventHandler*)
	{
		app.Quit(); //< fermeture de l'application
	});

	Nz::SimpleTextDrawer textDrawer;
	textDrawer.SetCharacterSize(72);
	textDrawer.SetTextOutlineThickness(4.f);
	textDrawer.SetText("Press a key");

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

	eventHandler.OnKeyPressed.Connect([&](const Nz::WindowEventHandler*, const Nz::WindowEvent::KeyEvent& e)
	{
		textDrawer.SetText("You pressed " + Nz::Keyboard::GetKeyName(e.virtualKey));
		textSprite->Update(textDrawer);

		Nz::Boxf textBox = textSprite->GetAABB();
		Nz::Vector2ui windowSize = mainWindow.GetSize();

		auto& nodeComponent = textEntity.get<Nz::NodeComponent>();
		nodeComponent.SetPosition(windowSize.x / 2 - textBox.width / 2, windowSize.y / 2 - textBox.height / 2);

		// Profitons-en aussi pour nous donner un moyen de quitter le programme
		if (e.virtualKey == Nz::Keyboard::VKey::Escape)
			mainWindow.Close(); // Cette ligne casse la boucle de la fenêtre
	});

	return app.Run();
}
