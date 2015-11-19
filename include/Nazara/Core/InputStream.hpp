// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_INPUTSTREAM_HPP
#define NAZARA_INPUTSTREAM_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Serialization.hpp>
#include <Nazara/Core/Stream.hpp>

namespace Nz
{
	class NAZARA_CORE_API InputStream : virtual public Stream
	{
		public:
			inline InputStream(const InputStream& stream);
			inline InputStream(InputStream&& stream) noexcept;
			virtual ~InputStream();

			virtual bool EndOfStream() const = 0;

			virtual UInt64 GetSize() const = 0;

			virtual std::size_t Read(void* buffer, std::size_t size) = 0;
			virtual String ReadLine(unsigned int lineSize = 0);

			template<typename T>
			InputStream& operator>>(T& value);

			inline InputStream& operator=(const InputStream& stream);
			inline InputStream& operator=(InputStream&& stream) noexcept;

		protected:
			inline InputStream();

			UnserializationContext m_unserializationContext;
	};
}

#include <Nazara/Core/InputStream.inl>

#endif // NAZARA_INPUTSTREAM_HPP
