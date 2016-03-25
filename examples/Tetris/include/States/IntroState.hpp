#pragma once

#ifndef TET_INTROSTATE_HPP
#define TET_INTROSTATE_HPP

#include <NDK/State.hpp>

namespace Ndk { class StateMachine; class World; }

namespace Tet {
    class IntroState : public Ndk::State {
    public:
        IntroState(Ndk::World& world);
        ~IntroState() = default;

        virtual void Enter(Ndk::StateMachine& fsm) override;
        virtual void Leave(Ndk::StateMachine& fsm) override;
        virtual bool Update(Ndk::StateMachine& fsm, float elapsedTime) override;

    private:
        Ndk::World& m_world;
    };
}

#endif // TET_INTROSTATE_HPP