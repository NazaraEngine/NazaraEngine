// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <utility>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::FunctorWithoutArgs
	* \brief Core class that represents a functor using a function without argument
	*/

	/*!
	* \brief Constructs a FunctorWithoutArgs object with a function
	*
	* \param func Function to execute
	*/
	template<typename F>
	FunctorWithoutArgs<F>::FunctorWithoutArgs(F func) :
	m_func(func)
	{
	}

	/*!
	* \brief Runs the function
	*/
	template<typename F>
	void FunctorWithoutArgs<F>::Run()
	{
		m_func();
	}

	/*!
	* \ingroup core
	* \class Nz::FunctorWithArgs
	* \brief Core class that represents a functor using a function with arguments
	*/

	/*!
	* \brief Constructs a FunctorWithArgs object with a function and its arguments
	*
	* \param func Function to execute
	* \param args Arguments for the function
	*/
	template<typename F, typename... Args>
	FunctorWithArgs<F, Args...>::FunctorWithArgs(F func, Args&&... args) :
	m_func(func),
	m_args(std::forward<Args>(args)...)
	{
	}

	/*!
	* \brief Runs the function
	*/
	template<typename F, typename... Args>
	void FunctorWithArgs<F, Args...>::Run()
	{
		Apply(m_func, m_args);
	}

	/*!
	* \ingroup core
	* \class Nz::MemberWithoutArgs
	* \brief Core class that represents a functor using a member function
	*/

	/*!
	* \brief Constructs a MemberWithoutArgs object with a member function and an object
	*
	* \param func Member function to execute
	* \param object Object to execute on
	*/
	template<typename C>
	MemberWithoutArgs<C>::MemberWithoutArgs(void (C::*func)(), C* object) :
	m_func(func),
	m_object(object)
	{
	}

	/*!
	* \brief Runs the function
	*/
	template<typename C>
	void MemberWithoutArgs<C>::Run()
	{
		(m_object->*m_func)();
	}
}
