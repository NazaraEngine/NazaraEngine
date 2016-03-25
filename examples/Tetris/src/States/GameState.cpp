#include "States/GameState.hpp"
#include <Nazara/Utility/Window.hpp>
#include <NDK/Application.hpp>
#include <NDK/StateMachine.hpp>
#include <NDK/World.hpp>

namespace Tet {
    GameState::GameState(Nz::Window& window, Ndk::World& world)
        : m_window{ window },
        m_world{ world }
    {}

    void GameState::Enter(Ndk::StateMachine& fsm) {

    }

    void GameState::Leave(Ndk::StateMachine& fsm) {

    }

    bool GameState::Update(Ndk::StateMachine& fsm, float elapsedTime) {
        Nz::WindowEvent event;
        while (m_window.PollEvent(&event)) {
            switch (event.type)
            {
                case Nz::WindowEventType_Quit:
                    Ndk::Application::Instance()->Quit();
                    break;
                default:
                    break;
            }
        }

        return true;
    }
}