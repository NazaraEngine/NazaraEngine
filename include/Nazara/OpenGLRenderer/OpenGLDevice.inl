// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz
{
	inline const GL::Context& OpenGLDevice::GetReferenceContext() const
	{
		return *m_referenceContext;
	}

	inline void OpenGLDevice::NotifyTextureDestruction(GLuint texture) const
	{
		for (const GL::Context* context : m_contexts)
			context->NotifyTextureDestruction(texture);
	}

	inline void OpenGLDevice::NotifyContextDestruction(const GL::Context& context) const
	{
		m_contexts.erase(&context);
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
