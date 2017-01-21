// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::CallOnExit
	* \brief Core class that represents a function to call at the end of the scope
	*/

	/*!
	* \brief Constructs a CallOnExit object with a function
	*
	* \param func Function to call on exit
	*/

	inline CallOnExit::CallOnExit(Func func) :
	m_func(func)
	{
	}

	/*!
	* \brief Destructs the object and calls the function
	*/

	inline CallOnExit::~CallOnExit()
	{
		if (m_func)
			m_func();
	}

	/*!
	* \brief Calls the function and sets the new callback
	*
	* \param func Function to call on exit
	*/

	inline void CallOnExit::CallAndReset(Func func)
	{
		if (m_func)
			m_func();

		Reset(func);
	}

	/*!
	* \brief Resets the function
	*
	* \param func Function to call on exit
	*/

	inline void CallOnExit::Reset(Func func)
	{
		m_func = func;
	}
}

#include <Nazara/Core/DebugOff.hpp>
