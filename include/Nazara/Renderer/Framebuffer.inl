// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/Framebuffer.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	inline Framebuffer::Framebuffer(FramebufferType type) :
	m_type(type)
	{
	}

	inline FramebufferType Framebuffer::GetType() const
	{
		return m_type;
	}
}

#include <Nazara/Renderer/DebugOff.hpp>
