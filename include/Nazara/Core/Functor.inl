// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

template<typename F>
NzFunctorWithoutArgs<F>::NzFunctorWithoutArgs(F func) :
m_func(func)
{
}

template<typename F>
void NzFunctorWithoutArgs<F>::Run()
{
	m_func();
}

template<typename F, typename... Args>
NzFunctorWithArgs<F, Args...>::NzFunctorWithArgs(F func, Args&... args) :
m_func(func),
m_args(args...)
{
}

template<typename F, typename... Args>
void NzFunctorWithArgs<F, Args...>::Run()
{
	NzUnpackTuple(m_func, m_args);
}


template<typename C>
NzMemberWithoutArgs<C>::NzMemberWithoutArgs(void (C::*func)(), C* object) :
m_func(func),
m_object(object)
{
}

template<typename C>
void NzMemberWithoutArgs<C>::Run()
{
	(m_object->*m_func)();
}
