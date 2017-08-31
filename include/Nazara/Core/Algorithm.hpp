// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ALGORITHM_CORE_HPP
#define NAZARA_ALGORITHM_CORE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/SerializationContext.hpp>
#include <functional>
#include <string>
#include <tuple>
#include <type_traits>

namespace Nz
{
	class AbstractHash;
	class ByteArray;

	template<typename F, typename Tuple> decltype(auto) Apply(F&& fn, Tuple&& t);
	template<typename O, typename F, typename Tuple> decltype(auto) Apply(O& object, F&& fn, Tuple&& t);
	template<typename T> constexpr std::size_t BitCount();
	template<typename T> ByteArray ComputeHash(HashType hash, const T& v);
	template<typename T> ByteArray ComputeHash(AbstractHash* hash, const T& v);
	template<typename T, std::size_t N> constexpr std::size_t CountOf(T(&name)[N]) noexcept;
	template<typename T> std::size_t CountOf(const T& c);
	template<typename T> void HashCombine(std::size_t& seed, const T& v);
	template<typename T> T ReverseBits(T integer);

	template<typename T>
	struct PointedType
	{
		using type = void; //< FIXME: I can't make SFINAE work
	};

	template<typename T>
	struct TypeTag {};

	inline bool Serialize(SerializationContext& context, bool value);
	inline bool Serialize(SerializationContext& context, const std::string& value);

	template<typename T>
	std::enable_if_t<std::is_arithmetic<T>::value, bool> Serialize(SerializationContext& context, T value);

	inline bool Unserialize(SerializationContext& context, bool* value);
	inline bool Unserialize(SerializationContext& context, std::string* value);

	template<typename T>
	std::enable_if_t<std::is_arithmetic<T>::value, bool> Unserialize(SerializationContext& context, T* value);
}

#include <Nazara/Core/Algorithm.inl>

#endif // NAZARA_ALGORITHM_CORE_HPP
