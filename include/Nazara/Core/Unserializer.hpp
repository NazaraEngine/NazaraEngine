// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UNSERIALIZER_HPP
#define NAZARA_UNSERIALIZER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Serialization.hpp>

namespace Nz
{
	class Stream;

	class Unserializer
	{
		public:
			inline Unserializer(Stream& stream);
			Unserializer(const Unserializer&) = default;
			Unserializer(Unserializer&&) = default;
			~Unserializer() = default;

			inline Endianness GetDataEndianness() const;
			inline Stream& GetStream() const;

			inline void SetDataEndianness(Endianness endiannes);
			inline void SetStream(Stream& stream);

			template<typename T>
			Unserializer& operator>>(T& value);

			Unserializer& operator=(const Unserializer&) = default;
			Unserializer& operator=(Unserializer&&) = default;

		private:
			UnserializationContext m_unserializationContext;
	};
}

#include <Nazara/Core/Unserializer.inl>

#endif // NAZARA_UNSERIALIZER_HPP
