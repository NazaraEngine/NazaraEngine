#include "States/MenuState.hpp"
#include <Nazara/Utility/Window.hpp>
#include <NDK/Application.hpp>
#include <NDK/StateMachine.hpp>
#include <NDK/World.hpp>
#include "States/GameState.hpp"

namespace Tet {
    MenuState::MenuState(Nz::Window& window, Ndk::World& world)
        : m_window{ window },
        m_world{ world }
    {}

    void MenuState::Enter(Ndk::StateMachine& fsm) {
        fsm.ChangeState(std::make_shared<Tet::GameState>(m_window, m_world));
    }

    void MenuState::Leave(Ndk::StateMachine& fsm) {

    }

    bool MenuState::Update(Ndk::StateMachine& fsm, float elapsedTime) {
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