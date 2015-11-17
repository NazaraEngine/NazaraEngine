// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_INPUTSTREAM_HPP
#define NAZARA_INPUTSTREAM_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Stream.hpp>

namespace Nz
{
	class NAZARA_CORE_API InputStream : virtual public Stream
	{
		public:
			virtual ~InputStream();

			virtual bool EndOfStream() const = 0;

			virtual UInt64 GetSize() const = 0;

			virtual std::size_t Read(void* buffer, std::size_t size) = 0;
			virtual String ReadLine(unsigned int lineSize = 0);

		protected:
			inline InputStream();
	};
}

#include <Nazara/Core/InputStream.inl>

#endif // NAZARA_INPUTSTREAM_HPP
