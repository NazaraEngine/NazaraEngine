// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	/*!
	* \brief Reset the current read bit cursor
	*/
	inline void SerializationContext::ResetReadBitPosition()
	{
		readBitPos = 8;
	}

	/*!
	* \brief Reset the current read bit cursor

	* \remark This function only reset the cursor position, it doesn't do any writing
	if you wish to write all bits and reset bit position, call FlushBits

	\see FlushBits
	*/
	inline void SerializationContext::ResetWriteBitPosition()
	{
		writeBitPos = 8;
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
	* \see Serialize, Deserialize
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
	* \see Serialize, Deserialize
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
	bool Deserialize(SerializationContext& context, T* value)
	{
		return Deserialize(context, value, TypeTag<T>());
	}

	/*!
	* \ingroup core
	* \brief Deserializes a boolean
	* \return true if unserialization succedeed
	*
	* \param context Context for the unserialization
	* \param value Pointer to boolean to deserialize
	*
	* \see Serialize, Deserialize
	*/
	inline bool Deserialize(SerializationContext& context, bool* value, TypeTag<bool>)
	{
		if (context.readBitPos == 8)
		{
			if (!Deserialize(context, &context.readByte, TypeTag<UInt8>()))
				return false;

			context.readBitPos = 0;
		}

		if (value)
			*value = (context.readByte & (1 << context.readBitPos)) != 0;

		context.readBitPos++;

		return true;
	}

	/*!
	* \brief Deserializes a string
	* \return true if successful
	*
	* \param context Context of unserialization
	* \param string std::string to deserialize
	*/
	bool Deserialize(SerializationContext& context, std::string* string, TypeTag<std::string>)
	{
		UInt32 size;
		if (!Deserialize(context, &size, TypeTag<UInt32>()))
			return false;

		string->resize(size);
		return context.stream->Read(&(*string)[0], size) == size;
	}

	/*!
	* \ingroup core
	* \brief Deserializes an arithmetic type
	* \return true if unserialization succedeed
	*
	* \param context Context for the unserialization
	* \param value Pointer to arithmetic type to serialize
	*
	* \remark Produce a NazaraAssert if pointer to value is invalid
	*
	* \see Serialize, Deserialize
	*/
	template<typename T>
	std::enable_if_t<std::is_arithmetic<T>::value, bool> Deserialize(SerializationContext& context, T* value, TypeTag<T>)
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

