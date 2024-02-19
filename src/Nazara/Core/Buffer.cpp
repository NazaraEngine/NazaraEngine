// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Buffer.hpp>
#include <Nazara/Core/BufferMapper.hpp>
#include <stdexcept>
#include <vector>

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
			throw std::runtime_error("buffer is not mappable: not implemented");
		}
	}
}
