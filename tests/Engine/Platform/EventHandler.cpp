#include <Nazara/Graphics/ColorBackground.hpp>
#include <Nazara/Renderer/RenderWindow.hpp>

#include <NDK/Application.hpp>
#include <NDK/StateMachine.hpp>
#include <NDK/World.hpp>
#include <NDK/Components/CameraComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Systems/RenderSystem.hpp>

#include "EventHandler/StateContext.hpp"
#include "EventHandler/StateFactory.hpp"

#include <Catch/catch.hpp>

Ndk::EntityHandle AddCamera(Ndk::World& world, Nz::RenderWindow& window);

/*!
	Known issues on Linux:

		- There is no real double click event in X11
		- Should always the key be repeated in key pressed mode ?
		- No smooth wheel, only 1.f or -1.f
		- Modify dimension of window updates position (which is not wrong)
		- Text entered is never repeated
*/

SCENARIO("EventHandler", "[PLATFORM][EVENTHANDLER][INTERACTIVE][.]")
{
	GIVEN("An application")
	{
		Ndk::Application& app = *Ndk::Application::Instance();
		auto& window = app.AddWindow<Nz::RenderWindow>();
		if (!window.Create(Nz::VideoMode(1024, 768, 32), "EventHandler"))
		{
			NazaraError("Failed to create window. See NazaraLog.log for further informations");
			REQUIRE(false);
		}
		window.EnableVerticalSync(true);

		auto& world = app.AddWorld();

		auto camera = AddCamera(world, window);

		StateContext context(window, world);

		StateFactory::Initialize(context);
		Ndk::StateMachine fsm(StateFactory::Get(EventStatus::Menu));

		Nz::Clock elapsedTimeClock;
		while (app.Run())
		{
			window.Display();

			float elapsedTime = elapsedTimeClock.Restart() / 1'000'000;
			if (!fsm.Update(elapsedTime))
			{
				NazaraError("Failed to update state machine.");
				REQUIRE(false);
			}
		}

		StateFactory::Uninitialize();
		REQUIRE(true);
	}
}

Ndk::EntityHandle AddCamera(Ndk::World& world, Nz::RenderWindow& window)
{
	Ndk::EntityHandle view = world.CreateEntity();
	auto& node = view->AddComponent<Ndk::NodeComponent>();
	node.SetPosition(Nz::Vector3f::Zero());
	auto& cam = view->AddComponent<Ndk::CameraComponent>();
	cam.SetProjectionType(Nz::ProjectionType_Orthogonal); // 2D
	cam.SetTarget(&window);

	world.GetSystem<Ndk::RenderSystem>().SetGlobalUp(Nz::Vector3f::Down());
	world.GetSystem<Ndk::RenderSystem>().SetDefaultBackground(Nz::ColorBackground::New(Nz::Color::Black));

	return view;
}
