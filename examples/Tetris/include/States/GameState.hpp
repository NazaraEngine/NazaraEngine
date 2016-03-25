#pragma once

#ifndef TET_GAMESTATE_HPP
#define TET_GAMESTATE_HPP

#include <NDK/State.hpp>

namespace Ndk { class StateMachine; class World; }
namespace Nz { class Window; }

namespace Tet {
    class GameState : public Ndk::State {
    public:
        GameState(Nz::Window& window, Ndk::World& world);
        ~GameState() = default;

        virtual void Enter(Ndk::StateMachine& fsm) override;
        virtual void Leave(Ndk::StateMachine& fsm) override;
        virtual bool Update(Ndk::StateMachine& fsm, float elapsedTime) override;

    private:
        Nz::Window& m_window;
        Ndk::World& m_world;
    };
}

#endif // TET_GAMESTATE_HPP