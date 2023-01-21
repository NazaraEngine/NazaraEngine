// Sources pour https://github.com/NazaraEngine/NazaraEngine/wiki/(FR)-Tutoriel:-%5B01%5D-Hello-World

#include <Nazara/Core/Application.hpp>
#include <Nazara/Core/AppEntitySystemComponent.hpp>
#include <Nazara/Core/Systems.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Graphics/Components.hpp>
#include <Nazara/Graphics/Systems.hpp>
#include <Nazara/Platform/AppWindowingComponent.hpp>
#include <Nazara/Renderer.hpp>
#include <Nazara/Utility.hpp>
#include <Nazara/Utility/Components.hpp>
#include <entt/entt.hpp>
#include <chrono>
#include <iostream>
#include <thread>

int main()
{
	Nz::Application<Nz::Graphics> app;

	auto& windowing = app.AddComponent<Nz::AppWindowingComponent>();
	Nz::Window& mainWindow = windowing.CreateWindow(Nz::VideoMode(1280, 720), "Hello world");

	auto& ecs = app.AddComponent<Nz::AppEntitySystemComponent>();

	Nz::RenderSystem& renderSystem = ecs.AddSystem<Nz::RenderSystem>();
	auto& windowSwapchain = renderSystem.CreateSwapchain(mainWindow);

	entt::handle cameraEntity = ecs.CreateEntity();
	{
		cameraEntity.emplace<Nz::NodeComponent>();

		auto& cameraComponent = cameraEntity.emplace<Nz::CameraComponent>(&windowSwapchain.GetSwapchain(), Nz::ProjectionType::Orthographic);
		cameraComponent.UpdateClearColor(Nz::Color(0.46f, 0.48f, 0.84f, 1.f));
	}

	Nz::SimpleTextDrawer textDrawer;
	textDrawer.SetCharacterSize(72);
	textDrawer.SetOutlineThickness(4.f);
	textDrawer.SetText("Hello world !");

	std::shared_ptr<Nz::TextSprite> textSprite = std::make_shared<Nz::TextSprite>();
	textSprite->Update(textDrawer);

	entt::handle textEntity = ecs.CreateEntity();
	{
		auto& nodeComponent = textEntity.emplace<Nz::NodeComponent>();
		auto& gfxComponent = textEntity.emplace<Nz::GraphicsComponent>();
		gfxComponent.AttachRenderable(textSprite, 0xFFFFFFFF);

		Nz::Boxf textBox = textSprite->GetAABB();
		Nz::Vector2ui windowSize = mainWindow.GetSize();
		nodeComponent.SetPosition(windowSize.x / 2 - textBox.width / 2, windowSize.y / 2 - textBox.height / 2);
	}

	return app.Run();
}
