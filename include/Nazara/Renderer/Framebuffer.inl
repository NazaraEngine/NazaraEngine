// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp


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
