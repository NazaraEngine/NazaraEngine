// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ABSTRACTBUFFER_HPP
#define NAZARA_ABSTRACTBUFFER_HPP

#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Enums.hpp>

namespace Nz
{
	class NAZARA_UTILITY_API AbstractBuffer
	{
		public:
			AbstractBuffer() = default;
			virtual ~AbstractBuffer();

			virtual bool Fill(const void* data, UInt32 offset, UInt32 size) = 0;

			virtual bool Initialize(UInt32 size, BufferUsageFlags usage) = 0;

			virtual DataStorage GetStorage() const = 0;

			virtual void* Map(BufferAccess access, UInt32 offset = 0, UInt32 size = 0) = 0;
			virtual bool Unmap() = 0;
	};
}

#endif // NAZARA_ABSTRACTBUFFER_HPP
