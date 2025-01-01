// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <cassert>

namespace Nz::GL
{
	inline void Shader::Compile()
	{
		assert(m_objectId);

		const Context& context = EnsureDeviceContext();
		context.glCompileShader(m_objectId);
	}

	inline bool Shader::GetCompilationStatus(std::string* error) const
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

	inline std::string Shader::GetSource() const
	{
		assert(m_objectId);
		const Context& context = EnsureDeviceContext();

		GLint sourceLength;
		context.glGetShaderiv(m_objectId, GL_SHADER_SOURCE_LENGTH, &sourceLength);

		if (sourceLength <= 1)
			return {};

		std::string source;
		source.resize(sourceLength - 1);

		GLsizei length;
		context.glGetShaderSource(m_objectId, sourceLength, &length, &source[0]);

		assert(length == sourceLength - 1);

		return source;
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

	inline void Shader::SetSource(std::string_view source)
	{
		return SetSource(source.data(), GLint(source.size()));
	}

	inline void Shader::SpecializeShader(const GLchar* pEntryPoint, GLuint numSpecializationConstants, const GLuint* pConstantIndex, const GLuint* pConstantValue)
	{
		assert(m_objectId);
		const Context& context = EnsureDeviceContext();
		assert(context.glSpecializeShader);

		context.glSpecializeShader(m_objectId, pEntryPoint, numSpecializationConstants, pConstantIndex, pConstantValue);
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
