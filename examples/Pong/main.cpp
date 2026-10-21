#include <Nazara/Core.hpp>
#include <Nazara/Graphics.hpp>
#include <Nazara/Platform/AppWindowingComponent.hpp>
#include <Nazara/Utility.hpp>
#include <Nazara/Renderer.hpp>

#include "State/GameState.hpp"


int main() {
	Nz::Application<Nz::Graphics> app;

	auto& windowing = app.AddComponent<Nz::AppWindowingComponent>();
	Nz::Window& mainWindow = windowing.CreateWindow(Nz::VideoMode(1280, 720), "Pong");
	
	auto& ecs = app.AddComponent<Nz::AppEntitySystemComponent>();
	auto& world = ecs.AddWorld<Nz::EnttWorld>();

	auto& velocitySystem = world.AddSystem<Nz::VelocitySystem>();
	
	auto& renderSystem = world.AddSystem<Nz::RenderSystem>();
	Nz::WindowSwapchain& windowSwapchain = renderSystem.CreateSwapchain(mainWindow);

	entt::handle cameraEntity = world.CreateEntity();
	
	cameraEntity.emplace<Nz::NodeComponent>();
	
	Nz::CameraComponent& cameraComponent = cameraEntity.emplace<Nz::CameraComponent>(&windowSwapchain, Nz::ProjectionType::Orthographic);

	Nz::StateMachine fsm{ std::make_shared<GameState>(world, mainWindow)};

	app.AddUpdaterFunc([&](Nz::Time elapsedTime) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 240));

		if (!fsm.Update(elapsedTime)) {
			NazaraError("Failed to update state machine");
			app.Quit();
		}
	});
	return app.Run();
}