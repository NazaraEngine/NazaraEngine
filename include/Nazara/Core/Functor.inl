// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <utility>

namespace Nz
{
	template<typename F>
	FunctorWithoutArgs<F>::FunctorWithoutArgs(F func) :
	m_func(func)
	{
	}

	template<typename F>
	void FunctorWithoutArgs<F>::Run()
	{
		m_func();
	}

	template<typename F, typename... Args>
	FunctorWithArgs<F, Args...>::FunctorWithArgs(F func, Args&&... args) :
	m_func(func),
	m_args(std::forward<Args>(args)...)
	{
	}

	template<typename F, typename... Args>
	void FunctorWithArgs<F, Args...>::Run()
	{
		Apply(m_func, m_args);
	}


	template<typename C>
	MemberWithoutArgs<C>::MemberWithoutArgs(void (C::*func)(), C* object) :
	m_func(func),
	m_object(object)
	{
	}

	template<typename C>
	void MemberWithoutArgs<C>::Run()
	{
		(m_object->*m_func)();
	}
}
