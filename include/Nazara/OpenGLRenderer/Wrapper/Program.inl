// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/Program.hpp>
#include <cassert>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	inline void Program::AttachShader(GLuint shader)
	{
		assert(m_objectId);

		const Context& context = EnsureDeviceContext();
		context.glAttachShader(m_objectId, shader);
	}

	inline bool Program::GetLinkStatus(std::string* error)
	{
		assert(m_objectId);
		const Context& context = EnsureDeviceContext();

		GLint success;
		context.glGetProgramiv(m_objectId, GL_LINK_STATUS, &success);
		if (!success)
		{
			if (error)
			{
				GLint logLength;
				context.glGetProgramiv(m_objectId, GL_INFO_LOG_LENGTH, &logLength);

				error->resize(logLength);

				if (logLength > 0)
				{
					GLsizei dummy;
					context.glGetProgramInfoLog(m_objectId, logLength, &dummy, error->data());
				}
			}

			return false;
		}

		return true;
	}

	inline void Program::Link()
	{
		assert(m_objectId);

		const Context& context = EnsureDeviceContext();
		context.glLinkProgram(m_objectId);
	}

	inline GLuint Program::CreateHelper(OpenGLDevice& /*device*/, const Context& context)
	{
		return context.glCreateProgram();
	}

	inline void Program::DestroyHelper(OpenGLDevice& device, const Context& context, GLuint objectId)
	{
		context.glDeleteProgram(objectId);

		device.NotifyProgramDestruction(objectId);
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
