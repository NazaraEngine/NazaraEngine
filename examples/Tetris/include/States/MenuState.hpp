#pragma once

#ifndef TET_MENUSTATE_HPP
#define TET_MENUSTATE_HPP

#include <NDK/State.hpp>

namespace Ndk { class StateMachine; class World; }
namespace Nz { class Window; }

namespace Tet {
    class MenuState : public Ndk::State {
    public:
        MenuState(Nz::Window& window, Ndk::World& world);
        ~MenuState() = default;

        virtual void Enter(Ndk::StateMachine& fsm) override;
        virtual void Leave(Ndk::StateMachine& fsm) override;
        virtual bool Update(Ndk::StateMachine& fsm, float elapsedTime) override;

    private:
        Nz::Window& m_window;
        Ndk::World& m_world;
    };
}

#endif // TET_MENUSTATE_HPP