// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ALGORITHM_CORE_HPP
#define NAZARA_ALGORITHM_CORE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <functional>
#include <tuple>

namespace Nz
{
	class AbstractHash;
	class ByteArray;

	template<typename F, typename Tuple> auto Apply(F&& fn, Tuple&& t);
	template<typename O, typename F, typename Tuple> auto Apply(O& object, F&& fn, Tuple&& t);
	template<typename T> ByteArray ComputeHash(HashType hash, const T& v);
	template<typename T> ByteArray ComputeHash(AbstractHash* hash, const T& v);
	template<typename T> void HashCombine(std::size_t& seed, const T& v);

	template<typename T>
	struct TypeTag {};


	template<typename T>
}

#include <Nazara/Core/Algorithm.inl>

#endif // NAZARA_ALGORITHM_CORE_HPP
