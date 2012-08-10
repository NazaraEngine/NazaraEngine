// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

template<typename F> NzFunctorWithoutArgs<F>::NzFunctorWithoutArgs(F func) :
function(func)
{
}

template<typename F> void NzFunctorWithoutArgs<F>::Run()
{
	function();
}

template<typename F, typename... Args> NzFunctorWithArgs<F, Args...>::NzFunctorWithArgs(F func, Args&... args) :
function(func),
arguments(args...)
{
}

template<typename F, typename... Args> void NzFunctorWithArgs<F, Args...>::Run()
{
	NzUnpackTuple(function, arguments);
}
