#include <Nazara/Renderer/RenderWindow.hpp>
#include <NDK/Application.hpp>
#include <NDK/StateMachine.hpp>
#include <NDK/World.hpp>
#include "States/MenuState.hpp"
#include "Utils/InitializeGame.hpp"
#include <iostream>

int main() {
	Ndk::Application app{};
	auto& window = app.AddWindow<Nz::RenderWindow>();
	if (!window.Create(Nz::VideoMode(800, 600, 32), "Tetris")) {
		std::cerr << "Failed to create window. See NazaraLog.log for further informations" << std::endl;
		return EXIT_FAILURE;
	}

	auto& world = app.AddWorld();
	Tet::Utils::InitializeGame(window, world);

    Ndk::StateMachine fsm{ std::make_shared<Tet::MenuState>(window, world) };

	Nz::Clock elapsedTimeClock;
    while (app.Run()) {
		float elapsedTime{ elapsedTimeClock.GetSeconds() };
		elapsedTimeClock.Restart();

        if (!fsm.Update(elapsedTime)) {
            std::cerr << "Failed to update state machine." << std::endl;
            return EXIT_FAILURE;
        }

        window.Display();
    }
}