// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/StateMachine.hpp>
#include <Nazara/Core/Error.hpp>
#include <utility>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::StateMachine
	* \brief NDK class that represents a state machine, to represent the multiple states of your program as a stack
	*/

	/*!
	* \brief Constructs a StateMachine object with an original state
	*
	* \param originalState State which is the entry point of the application, a nullptr will create an empty state machine
	*/
	inline StateMachine::StateMachine(std::shared_ptr<State> originalState)
	{
		if (originalState)
			PushState(std::move(originalState));
	}

	/*!
	* \brief Destructs the object
	*
	* \remark Calls "Leave" on all the states from top to bottom
	*/
	inline StateMachine::~StateMachine()
	{
		// Leave state from top to bottom (as if states were popped out)
		for (auto it = m_states.rbegin(); it != m_states.rend(); ++it)
			(*it)->Leave(*this);
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
		if (state)
		{
			// Change state is just a pop followed by a push
			StateTransition transition;
			transition.type = TransitionType::Pop;
			m_transitions.emplace_back(std::move(transition));

			transition.state = std::move(state);
			transition.type = TransitionType::Push;
			m_transitions.emplace_back(std::move(transition));
		}
	}

	/*!
	* \brief Checks whether the state is on the top of the machine
	* \return true If it is the case
	*
	* \param state State to compare the top with
	* \remark Because all actions popping or pushing a state don't take effect until next state machine update, this can return false on a just pushed state
	*/
	inline bool StateMachine::IsTopState(const State* state) const
	{
		if (m_states.empty())
			return false;

		return m_states.back().get() == state;
	}

	/*!
	* \brief Pops the state on the top of the machine
	*
	* \remark This method can completely empty the stack
	* \remark Like all actions popping or pushing a state, this is not immediate and will only take effect when state machine is updated
	*/
	inline void StateMachine::PopState()
	{
		StateTransition transition;
		transition.type = TransitionType::Pop;

		m_transitions.emplace_back(std::move(transition));
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
			StateTransition transition;
			transition.state = std::move(state);
			transition.type = TransitionType::PopUntil;

			m_transitions.emplace_back(std::move(transition));
		}
	}

	/*!
	* \brief Pushes a new state on the top of the machine
	*
	* \param state Next state to represent if it is nullptr, it performs no action
	*
	* \remark It is forbidden for a state machine to have (at any moment) the same state in its list multiple times
	* \remark Like all actions popping or pushing a state, this is not immediate and will only take effect when state machine is updated
	*/
	inline void StateMachine::PushState(std::shared_ptr<State> state)
	{
		if (state)
		{
			StateTransition transition;
			transition.state = std::move(state);
			transition.type = TransitionType::Push;

			m_transitions.emplace_back(std::move(transition));
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
		StateTransition transition;
		transition.type = TransitionType::PopUntil; //< Pop until nullptr, which basically clears the state machine
		m_transitions.emplace_back(std::move(transition));

		if (state)
		{
			transition.state = std::move(state);
			transition.type = TransitionType::Push;
			m_transitions.emplace_back(std::move(transition));
		}
	}

	/*!
	* \brief Updates all the states
	* \return true If update is successful for everyone of them
	*
	* \param elapsedTime Delta time used for the update
	*/
	inline bool StateMachine::Update(float elapsedTime)
	{
		for (StateTransition& transition : m_transitions)
		{
			switch (transition.type)
			{
				case TransitionType::Pop:
				{
					std::shared_ptr<State>& topState = m_states.back();
					topState->Leave(*this); //< Call leave before popping to ensure consistent IsTopState behavior

					m_states.pop_back();
					break;
				}

				case TransitionType::PopUntil:
				{
					while (!m_states.empty() && m_states.back() != transition.state)
					{
						m_states.back()->Leave(*this);
						m_states.pop_back();
					}
					break;
				}

				case TransitionType::Push:
				{
					m_states.emplace_back(std::move(transition.state));
					m_states.back()->Enter(*this);
					break;
				}
			}
		}
		m_transitions.clear();

		return std::all_of(m_states.begin(), m_states.end(), [=](std::shared_ptr<State>& state) {
			return state->Update(*this, elapsedTime);
		});
	}
}
