// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderBuffer.hpp>
#include <memory>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	inline RenderBuffer::RenderBuffer(Buffer* parent, BufferType type) :
	m_softwareBuffer(parent, type),
	m_parent(parent),
	m_type(type)
	{
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
