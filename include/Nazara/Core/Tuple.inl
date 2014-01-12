// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// http://stackoverflow.com/questions/687490/c0x-how-do-i-expand-a-tuple-into-variadic-template-function-arguments
// Merci à Ryan "FullMetal Alchemist" Lahfa
// Merci aussi à Freedom de siteduzero.com

#include <Nazara/Core/Debug.hpp>

template<unsigned int N>
struct NzImplTupleUnpack
{
	template <typename F, typename... ArgsT, typename... Args>
	void operator()(F func, const std::tuple<ArgsT...>& t,  Args&... args)
	{
		NzImplTupleUnpack<N-1>()(func, t, std::get<N-1>(t), args...);
	}
};

template<>
struct NzImplTupleUnpack<0>
{
	template <typename F, typename... ArgsT, typename... Args>
	void operator()(F func, const std::tuple<ArgsT...>&, Args&... args)
	{
		func(args...);
	}
};

template<typename F, typename... ArgsT>
void NzUnpackTuple(F func, const std::tuple<ArgsT...>& t)
{
	NzImplTupleUnpack<sizeof...(ArgsT)>()(func, t);
}

#include <Nazara/Core/DebugOff.hpp>
