// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_ABSTRACTBUFFER_HPP
#define NAZARA_UTILITY_ABSTRACTBUFFER_HPP

#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Enums.hpp>

namespace Nz
{
	class NAZARA_UTILITY_API AbstractBuffer
	{
		public:
			AbstractBuffer() = default;
			virtual ~AbstractBuffer();

			virtual bool Fill(const void* data, UInt64 offset, UInt64 size) = 0;

			virtual bool Initialize(UInt64 size, BufferUsageFlags usage) = 0;

			virtual UInt64 GetSize() const = 0;
			virtual DataStorage GetStorage() const = 0;

			virtual void* Map(BufferAccess access, UInt64 offset = 0, UInt64 size = 0) = 0;
			virtual bool Unmap() = 0;
	};
}

#endif // NAZARA_UTILITY_ABSTRACTBUFFER_HPP
