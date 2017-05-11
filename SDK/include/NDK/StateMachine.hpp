// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_STATEMACHINE_HPP
#define NDK_STATEMACHINE_HPP

#include <NDK/Prerequesites.hpp>
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

			inline const std::shared_ptr<State>& GetCurrentState() const;

			inline bool IsTopState(const State* state) const;

			inline std::shared_ptr<State> PopState();
			inline bool PopStatesUntil(std::shared_ptr<State> state);
			inline void PushState(std::shared_ptr<State> state);

			inline void SetState(std::shared_ptr<State> state);

			inline bool Update(float elapsedTime);

			inline StateMachine& operator=(StateMachine&& fsm) = default;
			StateMachine& operator=(const StateMachine&) = delete;

		private:
			std::vector<std::shared_ptr<State>> m_states;
	};
}

#include <NDK/StateMachine.inl>

#endif // NDK_STATEMACHINE_HPP