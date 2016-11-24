// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/Error.hpp>
#include <NDK/StateMachine.hpp>
#include <utility>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::StateMachine
	* \brief NDK class that represents a state machine, to represent the multiple states of your program
	*/

	/*!
	* \brief Constructs a StateMachine object with an original state
	*
	* \param originalState State which is the entry point of the application
	*
	* \remark Calls "Enter" on the state
	* \remark Produces a NazaraAssert if nullptr is given
	*/

	inline StateMachine::StateMachine(std::shared_ptr<State> originalState) :
	m_currentState(std::move(originalState))
	{
		NazaraAssert(m_currentState, "StateMachine must have a state to begin with");
		m_currentState->Enter(*this);
	}

	/*!
	* \brief Destructs the object
	*
	* \remark Calls "Leave" on the state
	*/

	inline StateMachine::~StateMachine()
	{
		m_currentState->Leave(*this);
	}

	/*!
	* \brief Changes the current state of the machine
	*
	* \param state Next state to represent
	*/

	inline void StateMachine::ChangeState(std::shared_ptr<State> state)
	{
		m_nextState = std::move(state);
	}

	/*!
	* \brief Gets the current state of the machine
	* \return A constant reference to the state
	*/

	inline const std::shared_ptr<State>& StateMachine::GetCurrentState() const
	{
		return m_currentState;
	}

	/*!
	* \brief Updates the state 
	* \return True if update is successful
	*
	* \param elapsedTime Delta time used for the update
	*/

	inline bool StateMachine::Update(float elapsedTime)
	{
		if (m_nextState)
		{
			m_currentState->Leave(*this);
			m_currentState = std::move(m_nextState);
			m_currentState->Enter(*this);
		}

		return m_currentState->Update(*this, elapsedTime);
	}
}
