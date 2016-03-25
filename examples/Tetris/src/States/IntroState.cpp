#include "States/IntroState.hpp"
#include <NDK/StateMachine.hpp>
#include <NDK/World.hpp>

namespace Tet {
    void IntroState::IntroState(Ndk::World& world)
        : m_world{ world }
    {}

    void IntroState::Enter(Ndk::StateMachine& fsm) {

    }

    void IntroState::Leave(Ndk::StateMachine& fsm) {

    }

    bool IntroState::Update(Ndk::StateMachine& fsm, float elapsedTime) {
        return true;
    }
}