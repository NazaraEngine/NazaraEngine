// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/ConditionVariable.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \class Nz::ConditionVariable
	*/

	/*!
	* \brief Constructs a ConditionVariable object by moving another one
	*/
	inline ConditionVariable::ConditionVariable(ConditionVariable&& condition) noexcept :
	m_impl(condition.m_impl)
	{
		condition.m_impl = nullptr;
	}
}

#include <Nazara/Core/DebugOff.hpp>
