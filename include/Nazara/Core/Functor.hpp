// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
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

template<typename F> struct NzFunctorWithoutArgs : NzFunctor
{
	NzFunctorWithoutArgs(F func);

	void Run();

	F function;
};

template<typename F, typename... Args> struct NzFunctorWithArgs : NzFunctor
{
	NzFunctorWithArgs(F func, Args&... args);

	void Run();

	F function;
	std::tuple<Args...> arguments;
};

template<typename F> struct NzFunctorWithoutArgs;
template<typename F, typename... Args> struct NzFunctorWithArgs;

#include <Nazara/Core/Functor.inl>

#endif // NAZARA_FUNCTOR_HPP
