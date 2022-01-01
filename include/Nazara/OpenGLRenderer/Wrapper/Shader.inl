// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/OpenGLRenderer/Wrapper/Shader.hpp>
#include <cassert>
#include <Nazara/OpenGLRenderer/Debug.hpp>

namespace Nz::GL
{
	inline void Shader::Compile()
	{
		assert(m_objectId);

		const Context& context = EnsureDeviceContext();
		context.glCompileShader(m_objectId);
	}

	inline bool Shader::GetCompilationStatus(std::string* error)
	{
		assert(m_objectId);
		const Context& context = EnsureDeviceContext();

		GLint success;
		context.glGetShaderiv(m_objectId, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			if (error)
			{
				GLint logLength;
				context.glGetShaderiv(m_objectId, GL_INFO_LOG_LENGTH, &logLength);

				error->resize(logLength);

				if (logLength > 0)
				{
					GLsizei dummy;
					context.glGetShaderInfoLog(m_objectId, logLength, &dummy, error->data());
				}
			}

			return false;
		}

		return true;
	}

	inline void Shader::SetBinarySource(GLenum binaryFormat, const void* binary, GLsizei length)
	{
		assert(m_objectId);

		m_device->GetReferenceContext().glShaderBinary(1U, &m_objectId.Get(), binaryFormat, binary, length);
	}

	inline void Shader::SetSource(const char* source, GLint length)
	{
		assert(m_objectId);

		m_device->GetReferenceContext().glShaderSource(m_objectId, 1U, &source, &length);
	}

	inline void Shader::SetSource(const std::string_view& source)
	{
		return SetSource(source.data(), GLint(source.size()));
	}

	inline void Shader::SpecializeShader(const GLchar* pEntryPoint, GLuint numSpecializationConstants, const GLuint* pConstantIndex, const GLuint* pConstantValue)
	{
		assert(m_objectId);
		const Context& context = EnsureDeviceContext();
		assert(context.glSpecializeShaderARB);

		context.glSpecializeShaderARB(m_objectId, pEntryPoint, numSpecializationConstants, pConstantIndex, pConstantValue);
	}

	inline GLuint Shader::CreateHelper(OpenGLDevice& /*device*/, const Context& context, GLenum shaderStage)
	{
		return context.glCreateShader(shaderStage);
	}

	inline void Shader::DestroyHelper(OpenGLDevice& /*device*/, const Context& context, GLuint objectId)
	{
		context.glDeleteShader(objectId);
	}
}

#include <Nazara/OpenGLRenderer/DebugOff.hpp>
