// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_ALGORITHM_HPP
#define NAZARA_CORE_ALGORITHM_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/SerializationContext.hpp>
#include <Nazara/Utils/Algorithm.hpp>
#include <Nazara/Utils/TypeTag.hpp>
#include <functional>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>

namespace Nz
{
	class ByteArray;

	template<typename T> ByteArray ComputeHash(HashType hash, const T& v);
	template<typename T> ByteArray ComputeHash(AbstractHash& hash, const T& v);

	inline bool HashAppend(AbstractHash* hash, const std::string_view& v);

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
