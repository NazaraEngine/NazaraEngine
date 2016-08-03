// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/Error.hpp>
#include <NDK/StateMachine.hpp>
#include <utility>

namespace Ndk
{
	inline StateMachine::StateMachine(std::shared_ptr<State> originalState) :
	m_currentState(std::move(originalState))
	{
		NazaraAssert(m_currentState, "StateMachine must have a state to begin with");
		m_currentState->Enter(*this);
	}

	inline StateMachine::~StateMachine()
	{
		m_currentState->Leave(*this);
	}


	inline void StateMachine::ChangeState(std::shared_ptr<State> state)
	{
		m_nextState = std::move(state);
	}

	inline const std::shared_ptr<State>& StateMachine::GetCurrentState() const
	{
		return m_currentState;
	}

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
