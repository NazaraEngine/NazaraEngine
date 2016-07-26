// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_STATE_HPP
#define NDK_STATE_HPP

#include <NDK/Prerequesites.hpp>

namespace Ndk
{
	class StateMachine;

	class State
	{
		public:
			State() = default;
			virtual ~State();

			virtual void Enter(StateMachine& fsm) = 0;
			virtual void Leave(StateMachine& fsm) = 0;
			virtual bool Update(StateMachine& fsm, float elapsedTime) = 0;
	};
}

#endif // NDK_STATE_HPP