// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

inline NzCallOnExit::NzCallOnExit(Func func) :
m_func(func)
{
}

inline NzCallOnExit::~NzCallOnExit()
{
	if (m_func)
		m_func();
}

inline void NzCallOnExit::Reset(Func func)
{
	m_func = func;
}

#include <Nazara/Core/DebugOff.hpp>
