// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/CallOnExit.hpp>
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
	* \brief Constructs a CallOnExit object with a functor
	*
	* \param func Function to call on exit
	*/
	template<typename F>
	CallOnExit<F>::CallOnExit(F&& functor) :
	m_functor(std::move(functor))
	{
	}

	/*!
	* \brief Destructs the object and calls the function
	*/
	template<typename F>
	CallOnExit<F>::~CallOnExit()
	{
		if (m_functor)
			(*m_functor)();
	}

	/*!
	* \brief Calls the function and sets the new callback
	*
	* \param func Function to call on exit
	*/
	template<typename F>
	void CallOnExit<F>::CallAndReset()
	{
		if (m_functor)
			(*m_functor)();

		m_functor.reset();
	}

	/*!
	* \brief Resets the function
	*
	* \param func Function to call on exit
	*/
	template<typename F>
	void CallOnExit<F>::Reset()
	{
		m_functor.reset();
	}
}

#include <Nazara/Core/DebugOff.hpp>
