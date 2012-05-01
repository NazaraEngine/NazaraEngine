// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TUPLE_HPP
#define NAZARA_TUPLE_HPP

#include <tuple>

template<typename F, typename... ArgsT> void NzUnpackTuple(F func, const std::tuple<ArgsT...>& t);

#include <Nazara/Utility/Tuple.inl>

#endif // NAZARA_TUPLE_HPP
