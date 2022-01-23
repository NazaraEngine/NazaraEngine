// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Buffer.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <stdexcept>
#include <vector>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	Buffer::~Buffer() = default;

	std::shared_ptr<Buffer> Buffer::CopyContent(const BufferFactory& bufferFactory)
	{
		if (GetUsageFlags() & BufferUsage::DirectMapping)
		{
			BufferMapper<Buffer> mapper(*this, 0, GetSize());
			return bufferFactory(GetType(), GetSize(), GetUsageFlags(), mapper.GetPointer());
		}
		else
		{
			// TODO: Implement GPU to CPU
			throw std::runtime_error("buffer is not mappable not implemented");
		}
	}
}
