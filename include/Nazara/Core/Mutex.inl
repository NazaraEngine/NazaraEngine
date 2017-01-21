// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Mutex.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::Mutex
	*/

	/*!
	* \brief Constructs a Mutex object by moving another one
	*/
	inline Mutex::Mutex(Mutex&& mutex) noexcept :
	m_impl(mutex.m_impl)
	{
		mutex.m_impl = nullptr;
	}
}

#include <Nazara/Core/DebugOff.hpp>
