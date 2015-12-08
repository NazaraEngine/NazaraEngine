// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SERIALIZATION_HPP
#define NAZARA_SERIALIZATION_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <functional>
#include <tuple>
#include <type_traits>

namespace Nz
{
	class Stream;

	struct SerializationContext
	{
		Stream* stream;
		Endianness endianness;
		UInt8 currentBitPos;
		UInt8 currentByte;
	};

	struct UnserializationContext
	{
		Stream* stream;
		Endianness endianness;
		UInt8 currentBitPos;
		UInt8 currentByte;
	};
}

#endif // NAZARA_SERIALIZATION_HPP
