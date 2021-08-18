// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/Sampler.hpp>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	inline void Sampler::SetParameterf(GLenum pname, GLfloat param)
	{
		assert(m_objectId);

		const Context& context = EnsureDeviceContext();
		context.glSamplerParameterf(m_objectId, pname, param);
	}

	inline void Sampler::SetParameteri(GLenum pname, GLint param)
	{
		assert(m_objectId);

		const Context& context = EnsureDeviceContext();
		context.glSamplerParameteri(m_objectId, pname, param);
	}

	inline void Sampler::SetParameterfv(GLenum pname, const GLfloat* param)
	{
		assert(m_objectId);

		const Context& context = EnsureDeviceContext();
		context.glSamplerParameterfv(m_objectId, pname, param);
	}

	inline void Sampler::SetParameteriv(GLenum pname, const GLint* param)
	{
		assert(m_objectId);

		const Context& context = EnsureDeviceContext();
		context.glSamplerParameteriv(m_objectId, pname, param);
	}

	inline GLuint Sampler::CreateHelper(OpenGLDevice& /*device*/, const Context& context)
	{
		GLuint sampler = 0;
		context.glGenSamplers(1U, &sampler);

		return sampler;
	}

	inline void Sampler::DestroyHelper(OpenGLDevice& device, const Context& context, GLuint objectId)
	{
		context.glDeleteSamplers(1U, &objectId);

		device.NotifySamplerDestruction(objectId);
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
