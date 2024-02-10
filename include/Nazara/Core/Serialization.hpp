// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_SERIALIZATION_HPP
#define NAZARA_CORE_SERIALIZATION_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Stream.hpp>
#include <NazaraUtils/Endianness.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <NazaraUtils/TypeTag.hpp>
#include <string>

namespace Nz
{
	struct NAZARA_CORE_API SerializationContext
	{
		MovablePtr<Stream> stream;
		Endianness endianness = Endianness::BigEndian; //< Default to Big Endian encoding
		UInt8 readBitPos = 8; //< 8 means no bit is currently read
		UInt8 readByte; //< Undefined value, will be initialized at the first bit read
		UInt8 writeBitPos = 8; //< 8 means no bit is currently wrote
		UInt8 writeByte; //< Undefined value, will be initialized at the first bit write

		void FlushBits();
		inline void ResetReadBitPosition();
		inline void ResetWriteBitPosition();
	};

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

#include <Nazara/Core/Serialization.inl>

#endif // NAZARA_CORE_SERIALIZATION_HPP
