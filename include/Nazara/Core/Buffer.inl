// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline Buffer::Buffer(DataStorage storage, BufferType type, UInt64 size, BufferUsageFlags usage) :
	m_type(type),
	m_usage(usage),
	m_storage(storage),
	m_size(size)
	{
	}

	inline UInt64 Nz::Buffer::GetSize() const
	{
		return m_size;
	}

	inline DataStorage Buffer::GetStorage() const
	{
		return m_storage;
	}

	inline BufferType Buffer::GetType() const
	{
		return m_type;
	}

	inline BufferUsageFlags Buffer::GetUsageFlags() const
	{
		return m_usage;
	}
}

#include <Nazara/Core/DebugOff.hpp>
