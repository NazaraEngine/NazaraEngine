// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACTBUFFER_HPP
#define NAZARA_ABSTRACTBUFFER_HPP

#include <Nazara/Utility/Buffer.hpp>

namespace Nz
{
	class NAZARA_UTILITY_API AbstractBuffer
	{
		public:
			AbstractBuffer() = default;
			virtual ~AbstractBuffer();

			virtual bool Create(unsigned int size, BufferUsage usage = BufferUsage_Static) = 0;
			virtual void Destroy() = 0;

			virtual bool Fill(const void* data, unsigned int offset, unsigned int size, bool forceDiscard = false) = 0;

			virtual bool IsHardware() const = 0;

			virtual void* Map(BufferAccess access, unsigned int offset = 0, unsigned int size = 0) = 0;
			virtual bool Unmap() = 0;
	};
}

#endif // NAZARA_ABSTRACTBUFFER_HPP
