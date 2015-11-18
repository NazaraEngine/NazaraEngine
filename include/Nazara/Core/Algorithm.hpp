// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ALGORITHM_CORE_HPP
#define NAZARA_ALGORITHM_CORE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/OutputStream.hpp>
#include <functional>
#include <tuple>
#include <type_traits>

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

	inline bool Serialize(OutputStream* output, bool value);

	template<typename T>
	std::enable_if_t<std::is_arithmetic<T>::value, bool> Serialize(OutputStream* output, T value);

	inline bool Unserialize(InputStream* input, bool* value);

	template<typename T>
	std::enable_if_t<std::is_arithmetic<T>::value, bool> Unserialize(InputStream* input, T* value);
}

#include <Nazara/Core/Algorithm.inl>

#endif // NAZARA_ALGORITHM_CORE_HPP
