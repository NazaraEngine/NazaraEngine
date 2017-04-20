// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

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
	* \param originalState State which is the entry point of the application
	*
	* \remark Calls "Enter" on the state
	* \remark Produces a NazaraAssert if nullptr is given
	*/

	inline StateMachine::StateMachine(std::shared_ptr<State> originalState)
	{
		NazaraAssert(originalState, "StateMachine must have a state to begin with");
		PushState(std::move(originalState));
	}

	/*!
	* \brief Destructs the object
	*
	* \remark Calls "Leave" on all the states
	*/

	inline StateMachine::~StateMachine()
	{
		for (std::shared_ptr<State>& state : m_states)
			state->Leave(*this);
	}

	/*!
	* \brief Replaces the current state on the top of the machine
	*
	* \param state State to replace the top one if it is nullptr, no action is performed
	*/

	inline void StateMachine::ChangeState(std::shared_ptr<State> state)
	{
		if (state)
		{
			PopState();
			PushState(std::move(state));
		}
	}

	/*!
	* \brief Gets the current state on the top of the machine
	* \return A constant reference to the state
	*
	* \remark The stack is supposed to be non empty, otherwise it is undefined behaviour
	*
	* \see PopStatesUntil
	*/

	inline const std::shared_ptr<State>& StateMachine::GetCurrentState() const
	{
		return m_states.back();
	}

	/*!
	* \brief Checks whether the state is on the top of the machine
	* \return true If it is the case
	*
	* \param state State to compare the top with
	*/

	inline bool StateMachine::IsTopState(const State* state) const
	{
		if (m_states.empty())
			return false;

		return m_states.back().get() == state;
	}

	/*!
	* \brief Pops the state on the top of the machine
	* \return Old state on the top, nullptr if stack was empty
	*
	* \remark This method can completely empty the stack
	*/

	inline std::shared_ptr<State> StateMachine::PopState()
	{
		if (m_states.empty())
			return nullptr;

		m_states.back()->Leave(*this);
		std::shared_ptr<State> oldTopState = std::move(m_states.back());
		m_states.pop_back();
		return oldTopState;
	}

	/*!
	* \brief Pops all the states of the machine until a specific one is reached
	* \return true If that specific state is on top, false if stack is empty
	*
	* \param state State to find on the stack if it is nullptr, no action is performed
	*
	* \remark This method can completely empty the stack
	*/

	inline bool StateMachine::PopStatesUntil(std::shared_ptr<State> state)
	{
		if (!state)
			return false;

		while (!m_states.empty() && !IsTopState(state.get()))
			PopState();

		return !m_states.empty();
	}

	/*!
	* \brief Pushes a new state on the top of the machine
	*
	* \param state Next state to represent if it is nullptr, it performs no action
	*
	* \remark Produces a NazaraAssert if the same state is pushed two times on the stack
	*/

	inline void StateMachine::PushState(std::shared_ptr<State> state)
	{
		if (state)
		{
			NazaraAssert(std::find(m_states.begin(), m_states.end(), state) == m_states.end(), "The same state was pushed two times");

			m_states.push_back(std::move(state));
			m_states.back()->Enter(*this);
		}
	}

	/*!
	* \brief Pops every states of the machine to put a new one
	*
	* \param state State to reset the stack with if it is nullptr, no action is performed
	*/

	inline void StateMachine::SetState(std::shared_ptr<State> state)
	{
		if (state)
		{
			while (!m_states.empty())
				PopState();

			PushState(std::move(state));
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
		return std::all_of(m_states.begin(), m_states.end(), [=](std::shared_ptr<State>& state) {
			return state->Update(*this, elapsedTime);
		});
	}
}
