// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ALGORITHM_CORE_HPP
#define NAZARA_ALGORITHM_CORE_HPP

#include <Nazara/Prerequesites.hpp>
#include <functional>
#include <tuple>

template<typename F, typename Tuple> auto NzApply(F&& fn, Tuple&& t);
template<typename O, typename F, typename Tuple> auto NzApply(O& object, F&& fn, Tuple&& t);
template<typename T> void NzHashCombine(std::size_t& seed, const T& v);

template<typename T>
struct NzTypeTag {};

#include <Nazara/Core/Algorithm.inl>

#endif // NAZARA_ALGORITHM_CORE_HPP
