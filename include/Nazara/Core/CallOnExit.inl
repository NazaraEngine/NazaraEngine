// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline CallOnExit::CallOnExit(Func func) :
	m_func(func)
	{
	}

	inline CallOnExit::~CallOnExit()
	{
		if (m_func)
			m_func();
	}

	inline void CallOnExit::CallAndReset(Func func)
	{
		if (m_func)
			m_func();

		Reset(func);
	}

	inline void CallOnExit::Reset(Func func)
	{
		m_func = func;
	}
}

#include <Nazara/Core/DebugOff.hpp>
