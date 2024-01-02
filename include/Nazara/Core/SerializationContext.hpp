// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CORE_SERIALIZATIONCONTEXT_HPP
#define NAZARA_CORE_SERIALIZATIONCONTEXT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Config.hpp>
#include <NazaraUtils/Endianness.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <NazaraUtils/TypeTag.hpp>

namespace Nz
{
	class Stream;

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
}

#include <Nazara/Core/SerializationContext.inl>

#endif // NAZARA_CORE_SERIALIZATIONCONTEXT_HPP
