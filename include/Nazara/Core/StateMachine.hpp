// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_STATEMACHINE_HPP
#define NAZARA_CORE_STATEMACHINE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/State.hpp>
#include <Nazara/Core/Time.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class StateMachine
	{
		public:
			inline StateMachine(std::shared_ptr<State> originalState);
			StateMachine(const StateMachine&) = delete;
			StateMachine(StateMachine&&) = default;
			inline ~StateMachine();

			inline void ChangeState(std::shared_ptr<State> state);

			inline bool IsTopState(const State* state) const;

			inline void PopState();
			inline void PopStatesUntil(std::shared_ptr<State> state);
			inline void PushState(std::shared_ptr<State> state);

			inline void ResetState(std::shared_ptr<State> state);

			inline bool Update(Time elapsedTime);

			StateMachine& operator=(StateMachine&& fsm) = default;
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

#include <Nazara/Core/StateMachine.inl>

#endif // NAZARA_CORE_STATEMACHINE_HPP
