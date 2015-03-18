// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ALGORITHM_CORE_HPP
#define NAZARA_ALGORITHM_CORE_HPP

#include <Nazara/Prerequesites.hpp>
#include <functional>
#include <tuple>

template<typename T> void NzHashCombine(std::size_t& seed, const T& v);
template<typename F, typename... ArgsT> void NzUnpackTuple(F func, const std::tuple<ArgsT...>& t);

#include <Nazara/Core/Algorithm.inl>

#endif // NAZARA_ALGORITHM_CORE_HPP
