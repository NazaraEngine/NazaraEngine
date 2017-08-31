// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SERIALIZATION_HPP
#define NAZARA_SERIALIZATION_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <functional>
#include <tuple>
#include <type_traits>

namespace Nz
{
	class Stream;

	struct NAZARA_CORE_API SerializationContext
	{
		Stream* stream;
		Endianness endianness = Endianness_BigEndian; //< Default to Big Endian encoding
		UInt8 currentBitPos = 8; //< 8 means no bit is currently wrote
		UInt8 currentByte; //< Undefined value, will be initialized at the first bit write

		void FlushBits();
		inline void ResetBitPosition();
	};
}

#include <Nazara/Core/SerializationContext.inl>

#endif // NAZARA_SERIALIZATION_HPP
