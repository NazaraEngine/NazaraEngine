// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_STATEMACHINE_HPP
#define NDK_STATEMACHINE_HPP

#include <NDK/Prerequisites.hpp>
#include <NDK/State.hpp>
#include <memory>
#include <vector>

namespace Ndk
{
	class StateMachine
	{
		public:
			inline StateMachine(std::shared_ptr<State> originalState);
			StateMachine(const StateMachine&) = delete;
			inline StateMachine(StateMachine&& fsm) = default;
			inline ~StateMachine();

			inline void ChangeState(std::shared_ptr<State> state);

			inline bool IsTopState(const State* state) const;

			inline void PopState();
			inline void PopStatesUntil(std::shared_ptr<State> state);
			inline void PushState(std::shared_ptr<State> state);

			inline void ResetState(std::shared_ptr<State> state);

			inline bool Update(float elapsedTime);

			inline StateMachine& operator=(StateMachine&& fsm) = default;
			StateMachine& operator=(const StateMachine&) = delete;

		private:
			enum class TransitionType
			{
				Pop,
				PopUntil,
				Push,
			};

			struct StateTransition
			{
				TransitionType type;
				std::shared_ptr<State> state;
			};

			std::vector<std::shared_ptr<State>> m_states;
			std::vector<StateTransition> m_transitions;
	};
}

#include <NDK/StateMachine.inl>

#endif // NDK_STATEMACHINE_HPP