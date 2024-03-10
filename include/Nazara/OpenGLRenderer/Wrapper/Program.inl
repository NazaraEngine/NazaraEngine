// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <cassert>

namespace Nz::GL
{
	inline void Program::AttachShader(GLuint shader)
	{
		assert(m_objectId);
		const Context& context = EnsureDeviceContext();

		context.glAttachShader(m_objectId, shader);
	}

	inline void Program::Get(GLenum pname, GLint* params) const
	{
		assert(m_objectId);
		const Context& context = EnsureDeviceContext();

		return context.glGetProgramiv(m_objectId, pname, params);
	}

	inline void Program::GetActiveUniform(GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name) const
	{
		assert(m_objectId);
		const Context& context = EnsureDeviceContext();

		return context.glGetActiveUniform(m_objectId, index, bufSize, length, size, type, name);
	}

	inline void Program::GetActiveUniformBlock(GLuint uniformBlockIndex, GLenum pname, GLint* params) const
	{
		assert(m_objectId);
		const Context& context = EnsureDeviceContext();

		return context.glGetActiveUniformBlockiv(m_objectId, uniformBlockIndex, pname, params);
	}

	inline std::vector<GLint> Program::GetActiveUniformBlockUniformIndices(GLuint uniformBlockIndex) const
	{
		assert(m_objectId);
		const Context& context = EnsureDeviceContext();

		std::vector<GLint> uniformIndices;

		GLint activeUniformCount = 0;
		context.glGetActiveUniformBlockiv(m_objectId, uniformBlockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &activeUniformCount);

		if (activeUniformCount > 0)
		{
			uniformIndices.resize(SafeCaster(activeUniformCount));
			context.glGetActiveUniformBlockiv(m_objectId, uniformBlockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, uniformIndices.data());
		}

		return uniformIndices;
	}

	inline std::vector<GLint> Program::GetActiveVariableIndices(GLenum programInterface, GLuint index) const
	{
		assert(m_objectId);
		const Context& context = EnsureDeviceContext();

		std::vector<GLint> varIndices;

		GLint activeVariableCount = 0;
		GetResource(programInterface, index, GL_NUM_ACTIVE_VARIABLES, 1, nullptr, &activeVariableCount);

		if (activeVariableCount > 0)
		{
			varIndices.resize(SafeCaster(activeVariableCount));
			GetResource(programInterface, index, GL_ACTIVE_VARIABLES, activeVariableCount, nullptr, varIndices.data());
		}

		return varIndices;
	}

	inline void Program::GetActiveUniformBlockName(GLuint uniformBlockIndex, GLsizei bufSize, GLsizei* length, GLchar* uniformBlockName) const
	{
		assert(m_objectId);
		const Context& context = EnsureDeviceContext();

		return context.glGetActiveUniformBlockName(m_objectId, uniformBlockIndex, bufSize, length, uniformBlockName);
	}

	inline std::vector<GLint> Program::GetActiveUniforms(GLsizei uniformCount, const GLuint* uniformIndices, GLenum pname) const
	{
		assert(m_objectId);
		const Context& context = EnsureDeviceContext();

		std::vector<GLint> values(uniformCount);
		context.glGetActiveUniformsiv(m_objectId, uniformCount, uniformIndices, pname, values.data());

		return values;
	}

	inline void Program::GetActiveUniforms(GLsizei uniformCount, const GLuint* uniformIndices, GLenum pname, GLint* params) const
	{
		assert(m_objectId);
		const Context& context = EnsureDeviceContext();

		return context.glGetActiveUniformsiv(m_objectId, uniformCount, uniformIndices, pname, params);
	}

	inline std::string Program::GetActiveUniformBlockName(GLuint uniformBlockIndex) const
	{
		assert(m_objectId);
		const Context& context = EnsureDeviceContext();

		std::string name;

		GLint nameLength = 0;
		context.glGetActiveUniformBlockiv(m_objectId, uniformBlockIndex, GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLength);

		if (nameLength > 0)
		{
			name.resize(nameLength);

			context.glGetActiveUniformBlockName(m_objectId, uniformBlockIndex, nameLength + 1, nullptr, name.data());
		}

		return name;
	}

	inline std::string Program::GetActiveUniformName(GLuint index) const
	{
		assert(m_objectId);
		const Context& context = EnsureDeviceContext();

		std::string name;

		GLint maxNameLength = 0;
		context.glGetProgramiv(m_objectId, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLength);

		if (maxNameLength > 0)
		{
			name.resize(maxNameLength);

			GLsizei length;
			GLint size;
			GLenum type;
			context.glGetActiveUniform(m_objectId, index, maxNameLength, &length, &size, &type, name.data());

			name.resize(length);
		}

		return name;
	}

	inline bool Program::GetLinkStatus(std::string* error) const
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

	inline GLint Program::GetInterface(GLenum programInterface, GLenum name) const
	{
		assert(m_objectId);
		const Context& context = EnsureDeviceContext();
		assert(context.glGetProgramInterfaceiv);

		GLint value;
		context.glGetProgramInterfaceiv(m_objectId, programInterface, name, &value);

		return value;
	}

	inline void Program::GetResource(GLenum programInterface, GLuint index, GLenum property, GLsizei bufSize, GLsizei* length, GLint* params) const
	{
		GLenum properties[] = { property };
		return GetResource(programInterface, index, 1, properties, bufSize, length, params);
	}

	inline void Program::GetResource(GLenum programInterface, GLuint index, GLsizei propCount, const GLenum* props, GLsizei bufSize, GLsizei* length, GLint* params) const
	{
		assert(m_objectId);
		const Context& context = EnsureDeviceContext();
		assert(context.glGetProgramResourceiv);

		GLint value;
		context.glGetProgramResourceiv(m_objectId, programInterface, index, propCount, props, bufSize, length, params);
	}

	inline GLuint Program::GetResourceIndex(GLenum programInterface, const char* name) const
	{
		assert(m_objectId);
		const Context& context = EnsureDeviceContext();
		assert(context.glGetProgramResourceIndex);

		return context.glGetProgramResourceIndex(m_objectId, programInterface, name);
	}

	inline GLint Program::GetResourceLocation(GLenum programInterface, const char* name) const
	{
		assert(m_objectId);
		const Context& context = EnsureDeviceContext();
		assert(context.glGetProgramResourceiv);

		return context.glGetProgramResourceLocation(m_objectId, programInterface, name);
	}

	inline std::string Program::GetResourceName(GLenum programInterface, GLuint index) const
	{
		GLint nameLength = 0;
		GetResource(programInterface, index, GL_NAME_LENGTH, 1, nullptr, &nameLength);

		std::string name;
		if (nameLength > 0)
		{
			name.resize(nameLength);
			GetResourceName(programInterface, index, nameLength + 1, nullptr, name.data());
		}

		return name;
	}

	inline void Program::GetResourceName(GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei* length, char* name) const
	{
		assert(m_objectId);
		const Context& context = EnsureDeviceContext();
		assert(context.glGetProgramResourceName);

		GLint value;
		context.glGetProgramResourceName(m_objectId, programInterface, index, bufSize, length, name);
	}

	inline GLuint Program::GetUniformBlockIndex(const char* uniformBlockName) const
	{
		assert(m_objectId);
		const Context& context = EnsureDeviceContext();

		return context.glGetUniformBlockIndex(m_objectId, uniformBlockName);
	}

	inline GLuint Program::GetUniformBlockIndex(const std::string& uniformBlockName) const
	{
		return GetUniformBlockIndex(uniformBlockName.c_str());
	}

	inline GLint Program::GetUniformLocation(const char* uniformName) const
	{
		assert(m_objectId);
		const Context& context = EnsureDeviceContext();

		return context.glGetUniformLocation(m_objectId, uniformName);
	}

	inline GLint Program::GetUniformLocation(const std::string& uniformName) const
	{
		return GetUniformLocation(uniformName.c_str());
	}

	inline void Program::Link()
	{
		assert(m_objectId);

		const Context& context = EnsureDeviceContext();
		context.glLinkProgram(m_objectId);
	}

	inline void Program::Uniform(GLint uniformLocation, float value) const
	{
		assert(m_objectId);

		const Context& context = EnsureDeviceContext();
		context.BindProgram(m_objectId);
		context.glUniform1f(uniformLocation, value);
	}

	inline void Program::Uniform(GLint uniformLocation, int value) const
	{
		assert(m_objectId);

		const Context& context = EnsureDeviceContext();
		context.BindProgram(m_objectId);
		context.glUniform1i(uniformLocation, value);
	}

	inline void Program::UniformBlockBinding(GLuint uniformBlockIndex, GLuint uniformBlockBinding) const
	{
		assert(m_objectId);

		const Context& context = EnsureDeviceContext();
		context.glUniformBlockBinding(m_objectId, uniformBlockIndex, uniformBlockBinding);
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

