// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OUTPUTSTREAM_HPP
#define NAZARA_OUTPUTSTREAM_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Stream.hpp>

namespace Nz
{
	class ByteArray;
	class String;

	class NAZARA_CORE_API OutputStream : virtual public Stream
	{
		public:
			virtual ~OutputStream();

			virtual void Flush() = 0;

			bool Write(const ByteArray& byteArray);
			bool Write(const String& string);
			virtual std::size_t Write(const void* buffer, std::size_t size) = 0;

			template<typename T>
			OutputStream& operator<<(const T& value);

		protected:
			inline OutputStream();
	};
}

#include <Nazara/Core/OutputStream.inl>

#endif // NAZARA_OUTPUTSTREAM_HPP
