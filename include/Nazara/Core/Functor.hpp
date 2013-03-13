// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FUNCTOR_HPP
#define NAZARA_FUNCTOR_HPP

#include <Nazara/Core/Tuple.hpp>

// Inspiré du code de la SFML par Laurent Gomila

struct NzFunctor
{
	virtual ~NzFunctor() {}

	virtual void Run() = 0;
};

template<typename F>
struct NzFunctorWithoutArgs : NzFunctor
{
	NzFunctorWithoutArgs(F func);

	void Run();

	private:
		F m_func;
};

template<typename F, typename... Args>
struct NzFunctorWithArgs : NzFunctor
{
	NzFunctorWithArgs(F func, Args&... args);

	void Run();

	private:
		F m_func;
		std::tuple<Args...> m_args;
};

template<typename C>
struct NzMemberWithoutArgs : NzFunctor
{
	NzMemberWithoutArgs(void (C::*func)(), C* object);

	void Run();

	private:
		void (C::*m_func)();
		C* m_object;
};

#include <Nazara/Core/Functor.inl>

#endif // NAZARA_FUNCTOR_HPP
