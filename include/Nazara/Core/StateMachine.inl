// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <utility>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup Core
	* \class Nz::StateMachine
	* \brief Core class that represents a state machine, to represent the multiple states of your program as a stack
	*/

	/*!
	* \brief Constructs a StateMachine object with an original state
	*
	* \param originalState State which is the entry point of the application, a nullptr will create an empty state machine
	*/
	inline StateMachine::StateMachine(std::shared_ptr<State> initialState)
	{
		if (initialState)
			PushState(std::move(initialState));
	}

	/*!
	* \brief Destructs the object
	*
	* \remark Calls "Leave" on all the states from top to bottom
	*/
	inline StateMachine::~StateMachine()
	{
		// Leave state from top to bottom (as if states were popped out)
		for (auto rit = m_states.rbegin(); rit != m_states.rend(); ++rit)
		{
			if (rit->enabled)
				rit->state->Leave(*this);
		}
	}

	/*!
	* \brief Replaces the current state on the top of the machine
	*
	* \param state State to replace the top one if it is nullptr, no action is performed
	*
	* \remark It is forbidden for a state machine to have (at any moment) the same state in its list multiple times
	* \remark Like all actions popping or pushing a state, this is not immediate and will only take effect when state machine is updated
	*/
	inline void StateMachine::ChangeState(std::shared_ptr<State> state)
	{
		// Change state is just a pop followed by a push
		StateTransition& popTransition = m_transitions.emplace_back();
		popTransition.type = TransitionType::Pop;

		if (state)
		{
			StateTransition& pushTransition = m_transitions.emplace_back();
			pushTransition.state = std::move(state);
			pushTransition.type = TransitionType::Push;
		}
	}

	/*!
	* \brief Disables a state, calling its Leave method and no longer Updating it
	*
	* \param state State to disable
	*
	* \remark Does nothing if the state is already disabled
	* \remark Like all actions popping or pushing a state, this is not immediate and will only take effect when state machine is updated
	*/
	inline void StateMachine::Disable(std::shared_ptr<State> state)
	{
		StateTransition& disableTransition = m_transitions.emplace_back();
		disableTransition.state = std::move(state);
		disableTransition.type = TransitionType::Disable;
	}

	/*!
	* \brief Enables a state, calling its Enter method and updating it
	*
	* \param state State to enable
	*
	* \remark Does nothing if the state is already enabled
	* \remark Like all actions popping or pushing a state, this is not immediate and will only take effect when state machine is updated
	*/
	inline void StateMachine::Enable(std::shared_ptr<State> state)
	{
		StateTransition& disableTransition = m_transitions.emplace_back();
		disableTransition.state = std::move(state);
		disableTransition.type = TransitionType::Enable;
	}

	/*!
	* \brief Checks whether the state is enabled on this state machine
	* \return true If it is the case
	*
	* \param state State to compare the top with
	* 
	* \remark Because all actions popping or pushing a state don't take effect until next state machine update, this can return false on a just enabled state
	*/
	inline bool StateMachine::IsStateEnabled(const State* state) const
	{
		auto it = std::find_if(m_states.begin(), m_states.end(), [&](const StateInfo& stateInfo) { return stateInfo.state.get() == state; });
		assert(it != m_states.end());

		return it->enabled;
	}

	/*!
	* \brief Checks whether the state is on the top of the machine
	* \return true If it is the case
	*
	* \param state State to compare the top with
	* 
	* \remark Because all actions popping or pushing a state don't take effect until next state machine update, this can return false on a just pushed state
	*/
	inline bool StateMachine::IsTopState(const State* state) const
	{
		if (m_states.empty())
			return false;

		return m_states.back().state.get() == state;
	}

	/*!
	* \brief Pops the state on the top of the machine
	*
	* \remark This method can completely empty the stack
	* \remark Like all actions popping or pushing a state, this is not immediate and will only take effect when state machine is updated
	*/
	inline void StateMachine::PopState()
	{
		StateTransition& transition = m_transitions.emplace_back();
		transition.type = TransitionType::Pop;
	}

	/*!
	* \brief Pops all states of the machine until a specific one is reached
	*
	* \param state State to find on the stack. If nullptr is passed, no action is performed
	*
	* \remark This method will completely empty the stack if state is not present
	* \remark Like all actions popping or pushing a state, this is not immediate and will only take effect when state machine is updated
	*/
	inline void StateMachine::PopStatesUntil(std::shared_ptr<State> state)
	{
		if (state)
		{
			StateTransition& transition = m_transitions.emplace_back();
			transition.state = std::move(state);
			transition.type = TransitionType::PopUntil;
		}
	}

	/*!
	* \brief Pushes a new state on the top of the machine
	*
	* \param state Next state to represent if it is nullptr, it performs no action
	* \param enable If the state should be enabled right after pushing it
	*
	* \remark It is forbidden for a state machine to have (at any moment) the same state in its list multiple times
	* \remark Like all actions popping or pushing a state, this is not immediate and will only take effect when state machine is updated
	*/
	inline void StateMachine::PushState(std::shared_ptr<State> state, bool enabled)
	{
		if (state)
		{
			StateTransition& transition = m_transitions.emplace_back();
			transition.state = std::move(state);
			transition.type = (enabled) ? TransitionType::Push : TransitionType::PushDisabled;
		}
	}

	/*!
	* \brief Pops every states of the machine to put a new one
	*
	* \param state State to reset the stack with. If state is invalid, this will clear the state machine
	*
	* \remark It is forbidden for a state machine to have (at any moment) the same state in its list multiple times
	* \remark Like all actions popping or pushing a state, this is not immediate and will only take effect when state machine is updated
	*/
	inline void StateMachine::ResetState(std::shared_ptr<State> state)
	{
		StateTransition& transition = m_transitions.emplace_back();
		transition.type = TransitionType::PopUntil; //< Pop until nullptr, which basically clears the state machine

		if (state)
		{
			StateTransition& pushTransition = m_transitions.emplace_back();
			pushTransition.state = std::move(state);
			pushTransition.type = TransitionType::Push;
		}
	}

	/*!
	* \brief Updates all the states
	* \return true If update is successful for everyone of them
	*
	* \param elapsedTime Delta time used for the update
	*/
	inline bool StateMachine::Update(Time elapsedTime)
	{
		// Use a classic for instead of a range-for because some state may push/pop on enter/leave, adding new transitions as we iterate
		// (range-for is a problem here because it doesn't handle mutable containers)

		for (std::size_t i = 0; i < m_transitions.size(); ++i)
		{
			StateTransition& transition = m_transitions[i];

			switch (transition.type)
			{
				case TransitionType::Disable:
				{
					auto it = std::find_if(m_states.begin(), m_states.end(), [&](const StateInfo& stateInfo) { return stateInfo.state == transition.state; });
					assert(it != m_states.end());

					if (it->enabled)
					{
						it->state->Leave(*this);
						it->enabled = false;
					}

					break;
				}

				case TransitionType::Enable:
				{
					auto it = std::find_if(m_states.begin(), m_states.end(), [&](const StateInfo& stateInfo) { return stateInfo.state == transition.state; });
					assert(it != m_states.end());

					if (!it->enabled)
					{
						it->enabled = true;
						it->state->Enter(*this);
					}

					break;
				}

				case TransitionType::Pop:
				{
					StateInfo& topState = m_states.back();
					if (topState.enabled)
						topState.state->Leave(*this); //< Call leave before popping to ensure consistent IsTopState behavior

					m_states.pop_back();
					break;
				}

				case TransitionType::PopUntil:
				{
					while (!m_states.empty() && m_states.back().state != transition.state)
					{
						StateInfo& topState = m_states.back();
						if (topState.enabled)
							topState.state->Leave(*this);

						m_states.pop_back();
					}
					break;
				}

				case TransitionType::Push:
				{
					StateInfo& stateInfo = m_states.emplace_back();
					stateInfo.enabled = true;
					stateInfo.state = std::move(transition.state);
					stateInfo.state->Enter(*this);

					break;
				}

				case TransitionType::PushDisabled:
				{
					StateInfo& stateInfo = m_states.emplace_back();
					stateInfo.enabled = false;
					stateInfo.state = std::move(transition.state);

					break;
				}
			}
		}
		m_transitions.clear();

		for (StateInfo& stateInfo : m_states)
		{
			if (!stateInfo.enabled)
				continue;

			if (!stateInfo.state->Update(*this, elapsedTime))
				return false;
		}

		return true;
	}
}

#include <Nazara/Core/DebugOff.hpp>
