// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// http://stackoverflow.com/questions/687490/c0x-how-do-i-expand-a-tuple-into-variadic-template-function-arguments
// Merci à Ryan "FullMetal Alchemist" Lahfa
// Merci aussi à Freedom de siteduzero.com

#include <Nazara/Core/Debug.hpp>

// http://www.cppsamples.com/common-tasks/apply-tuple-to-function.html
template<typename F, typename Tuple, size_t... S>
auto NzApplyImplFunc(F&& fn, Tuple&& t, std::index_sequence<S...>)
{
	return std::forward<F>(fn)(std::get<S>(std::forward<Tuple>(t))...);
}

template<typename F, typename Tuple>
auto NzApply(F&& fn, Tuple&& t)
{
	constexpr std::size_t tSize = std::tuple_size<typename std::remove_reference<Tuple>::type>::value;

	return NzApplyImplFunc(std::forward<F>(fn), std::forward<Tuple>(t), std::make_index_sequence<tSize>());
}

template<typename O, typename F, typename Tuple, size_t... S>
auto NzApplyImplMethod(O& object, F&& fn, Tuple&& t, std::index_sequence<S...>)
{
	return (object .* std::forward<F>(fn))(std::get<S>(std::forward<Tuple>(t))...);
}

template<typename O, typename F, typename Tuple>
auto NzApply(O& object, F&& fn, Tuple&& t)
{
	constexpr std::size_t tSize = std::tuple_size<typename std::remove_reference<Tuple>::type>::value;

	return NzApplyImplMethod(object, std::forward<F>(fn), std::forward<Tuple>(t), std::make_index_sequence<tSize>());
}
// Algorithme venant de CityHash par Google
// http://stackoverflow.com/questions/8513911/how-to-create-a-good-hash-combine-with-64-bit-output-inspired-by-boosthash-co
template<typename T>
void NzHashCombine(std::size_t& seed, const T& v)
{
	const nzUInt64 kMul = 0x9ddfea08eb382d69ULL;

	std::hash<T> hasher;
	nzUInt64 a = (hasher(v) ^ seed) * kMul;
	a ^= (a >> 47);

	nzUInt64 b = (seed ^ a) * kMul;
	b ^= (b >> 47);

	seed = static_cast<std::size_t>(b * kMul);
}

#include <Nazara/Core/DebugOff.hpp>
