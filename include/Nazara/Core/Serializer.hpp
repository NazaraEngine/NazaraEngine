// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SERIALIZER_HPP
#define NAZARA_SERIALIZER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Serialization.hpp>

namespace Nz
{
	class Stream;

	class Serializer
	{
		public:
			inline Serializer(Stream& stream);
			Serializer(const Serializer&) = default;
			Serializer(Serializer&&) = default;
			~Serializer();

			inline Endianness GetDataEndianness() const;
			inline Stream& GetStream() const;

			inline void SetDataEndianness(Endianness endiannes);
			inline void SetStream(Stream& stream);

			template<typename T>
			Serializer& operator<<(const T& value);

			Serializer& operator=(const Serializer&) = default;
			Serializer& operator=(Serializer&&) = default;

		private:
			SerializationContext m_serializationContext;
	};
}

#include <Nazara/Core/Serializer.inl>

#endif // NAZARA_SERIALIZER_HPP
