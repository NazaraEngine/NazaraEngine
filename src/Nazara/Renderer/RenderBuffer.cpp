// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderBuffer.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	bool RenderBuffer::Initialize(UInt32 size, BufferUsageFlags usage)
	{
		m_softwareBuffer.Initialize(size, usage);
		return true;
	}

	DataStorage Nz::RenderBuffer::GetStorage() const
	{
		return DataStorage::DataStorage_Hardware;
	}
}
