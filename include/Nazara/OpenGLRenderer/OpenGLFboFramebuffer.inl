// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLFboFramebuffer.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline const Vector2ui& OpenGLFboFramebuffer::GetAttachmentSize(std::size_t i) const
	{
		return m_attachmentSizes[i];
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
