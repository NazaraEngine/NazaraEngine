// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_ALGORITHM_HPP
#define NAZARA_CORE_ALGORITHM_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/SerializationContext.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <NazaraUtils/TypeTag.hpp>
#include <functional>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>

namespace Nz
{
	class ByteArray;

	template<typename T> ByteArray ComputeHash(HashType hash, T&& v);
	template<typename T> ByteArray ComputeHash(AbstractHash& hash, T&& v);

	inline bool HashAppend(AbstractHash* hash, std::string_view v);

	template<typename T>
	bool Serialize(SerializationContext& context, T&& value);

	inline bool Serialize(SerializationContext& context, bool value, TypeTag<bool>);
	inline bool Serialize(SerializationContext& context, const std::string& value, TypeTag<std::string>);

	template<typename T>
	std::enable_if_t<std::is_arithmetic<T>::value, bool> Serialize(SerializationContext& context, T value, TypeTag<T>);

	template<typename T>
	bool Unserialize(SerializationContext& context, T* value);

	inline bool Unserialize(SerializationContext& context, bool* value, TypeTag<bool>);
	inline bool Unserialize(SerializationContext& context, std::string* value, TypeTag<std::string>);

	template<typename T>
	std::enable_if_t<std::is_arithmetic<T>::value, bool> Unserialize(SerializationContext& context, T* value, TypeTag<T>);
}

#include <Nazara/Core/Algorithm.inl>

#endif // NAZARA_CORE_ALGORITHM_HPP
