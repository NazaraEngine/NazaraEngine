// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/ByteArray.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Stream.hpp>
#include <array>
#include <cassert>
#include <climits>
#include <cmath>
#include <limits>
#include <utility>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \brief Computes the hash of a hashable object
	* \return A bytearray which represents the hash
	*
	* \param hash Enumeration of type HashType
	* \param v Object to hash
	*
	* \remark a HashAppend specialization for type T is required
	*
	* \see ComputeHash
	*/
	template<typename T>
	ByteArray ComputeHash(HashType hash, T&& v)
	{
		return ComputeHash(*AbstractHash::Get(hash), std::forward<T>(v));
	}

	/*!
	* \ingroup core
	* \brief Computes the hash of a hashable object
	* \return A bytearray which represents the hash
	*
	* \param hash Pointer to abstract hash
	* \param v Object to hash
	*
	* \remark Produce a NazaraAssert if pointer to Abstracthash is invalid
	* \remark a HashAppend specialization for type T is required
	*
	* \see ComputeHash
	*/
	template<typename T>
	ByteArray ComputeHash(AbstractHash& hash, T&& v)
	{
		hash.Begin();

		HashAppend(hash, std::forward<T>(v));

		return hash.End();
	}

	inline bool HashAppend(AbstractHash& hash, std::string_view v)
	{
		hash.Append(reinterpret_cast<const UInt8*>(v.data()), v.size());
		return true;
	}

	template<typename T>
	bool Serialize(SerializationContext& context, T&& value)
	{
		return Serialize(context, std::forward<T>(value), TypeTag<std::decay_t<T>>());
	}

	/*!
	* \ingroup core
	* \brief Serializes a boolean
	* \return true if serialization succeeded
	*
	* \param context Context for the serialization
	* \param value Boolean to serialize
	*
	* \see Serialize, Unserialize
	*/
	inline bool Serialize(SerializationContext& context, bool value, TypeTag<bool>)
	{
		if (context.writeBitPos == 8)
		{
			context.writeBitPos = 0;
			context.writeByte = 0;
		}

		if (value)
			context.writeByte |= 1 << context.writeBitPos;

		if (++context.writeBitPos >= 8)
			return Serialize(context, context.writeByte, TypeTag<UInt8>());
		else
			return true;
	}

	/*!
	* \ingroup core
	* \brief Serializes a std::string
	* \return true if successful
	*
	* \param context Context for the serialization
	* \param value String to serialize
	*/
	bool Serialize(SerializationContext& context, const std::string& value, TypeTag<std::string>)
	{
		if (!Serialize(context, SafeCast<UInt32>(value.size()), TypeTag<UInt32>()))
			return false;

		return context.stream->Write(value.data(), value.size()) == value.size();
	}

	/*!
	* \ingroup core
	* \brief Serializes an arithmetic type
	* \return true if serialization succeeded
	*
	* \param context Context for the serialization
	* \param value Arithmetic type to serialize
	*
	* \see Serialize, Unserialize
	*/
	template<typename T>
	std::enable_if_t<std::is_arithmetic<T>::value, bool> Serialize(SerializationContext& context, T value, TypeTag<T>)
	{
		// Flush bits in case a writing is in progress
		context.FlushBits();

		if (context.endianness != Endianness::Unknown && context.endianness != PlatformEndianness)
			value = ByteSwap(value);

		return context.stream->Write(&value, sizeof(T)) == sizeof(T);
	}


	template<typename T>
	bool Unserialize(SerializationContext& context, T* value)
	{
		return Unserialize(context, value, TypeTag<T>());
	}

	/*!
	* \ingroup core
	* \brief Unserializes a boolean
	* \return true if unserialization succedeed
	*
	* \param context Context for the unserialization
	* \param value Pointer to boolean to unserialize
	*
	* \see Serialize, Unserialize
	*/
	inline bool Unserialize(SerializationContext& context, bool* value, TypeTag<bool>)
	{
		if (context.readBitPos == 8)
		{
			if (!Unserialize(context, &context.readByte, TypeTag<UInt8>()))
				return false;

			context.readBitPos = 0;
		}

		if (value)
			*value = (context.readByte & (1 << context.readBitPos)) != 0;

		context.readBitPos++;

		return true;
	}

	/*!
	* \brief Unserializes a string
	* \return true if successful
	*
	* \param context Context of unserialization
	* \param string std::string to unserialize
	*/
	bool Unserialize(SerializationContext& context, std::string* string, TypeTag<std::string>)
	{
		UInt32 size;
		if (!Unserialize(context, &size, TypeTag<UInt32>()))
			return false;

		string->resize(size);
		return context.stream->Read(&(*string)[0], size) == size;
	}

	/*!
	* \ingroup core
	* \brief Unserializes an arithmetic type
	* \return true if unserialization succedeed
	*
	* \param context Context for the unserialization
	* \param value Pointer to arithmetic type to serialize
	*
	* \remark Produce a NazaraAssert if pointer to value is invalid
	*
	* \see Serialize, Unserialize
	*/
	template<typename T>
	std::enable_if_t<std::is_arithmetic<T>::value, bool> Unserialize(SerializationContext& context, T* value, TypeTag<T>)
	{
		NazaraAssert(value, "Invalid data pointer");

		context.ResetReadBitPosition();

		if (context.stream->Read(value, sizeof(T)) == sizeof(T))
		{
			if (context.endianness != Endianness::Unknown && context.endianness != PlatformEndianness)
				*value = ByteSwap(*value);

			return true;
		}
		else
			return false;
	}
}

#include <Nazara/Core/DebugOff.hpp>
