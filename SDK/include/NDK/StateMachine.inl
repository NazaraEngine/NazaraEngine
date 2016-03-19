// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/StateMachine.hpp>
#include <utility>

namespace Ndk
{
	inline void StateMachine::ChangeState(std::shared_ptr<State> state)
	{
		m_nextState = std::move(state);
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
