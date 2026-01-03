// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline const GL::Context& OpenGLDevice::GetReferenceContext() const
	{
		return *m_referenceContext;
	}

	inline void OpenGLDevice::NotifyBufferDestruction(GLuint buffer) const
	{
		for (const GL::Context* context : m_contexts)
			context->NotifyBufferDestruction(buffer);
	}

	inline void OpenGLDevice::NotifyProgramDestruction(GLuint program) const
	{
		for (const GL::Context* context : m_contexts)
			context->NotifyProgramDestruction(program);
	}

	inline void OpenGLDevice::NotifySamplerDestruction(GLuint sampler) const
	{
		for (const GL::Context* context : m_contexts)
			context->NotifySamplerDestruction(sampler);
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
