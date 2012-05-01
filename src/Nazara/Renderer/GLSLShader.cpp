// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/GLSLShader.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace
{
	GLenum shaderType[nzShaderType_Count] = {
		GL_FRAGMENT_SHADER,	// nzShaderType_Fragment
		GL_GEOMETRY_SHADER,	// nzShaderType_Geometry
		GL_VERTEX_SHADER	// nzShaderType_Vertex
	};
}

NzGLSLShader::NzGLSLShader(NzShader* parent) :
m_parent(parent)
{
}

NzGLSLShader::~NzGLSLShader()
{
}

bool NzGLSLShader::Bind()
{
	glUseProgram(m_program);

	return true; ///FIXME: Comment détecter une erreur d'OpenGL sans ralentir le programme ?
}

bool NzGLSLShader::Compile()
{
	m_idCache.clear();

	glLinkProgram(m_program);

	GLint success;
	glGetProgramiv(m_program, GL_LINK_STATUS, &success);

	if (success == GL_TRUE)
	{
		static NzString success("Linkage successful");
		m_log = success;

		return true;
	}
	else
	{
		// On remplit le log avec l'erreur de compilation
		GLint length = 0;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &length);
		if (length > 1)
		{
			m_log.Reserve(length+19-1); // La taille retournée est celle du buffer (Avec caractère de fin)
			m_log.Prepend("Linkage error: ");
			m_log.Resize(length+19-1); // Extension du buffer d'écriture pour ajouter le log

			glGetProgramInfoLog(m_program, length-1, nullptr, &m_log[19]);
		}
		else
			m_log = "Linkage failed but no info log found";

		NazaraError(m_log);

		return false;
	}
}

bool NzGLSLShader::Create()
{
	m_program = glCreateProgram();
	if (!m_program)
	{
		NazaraError("Failed to create program");
		return false;
	}

	glBindAttribLocation(m_program, 0, "Position");
	glBindAttribLocation(m_program, 1, "Normal");
	//glBindAttribLocation(m_program, 2, "Diffuse");
	glBindAttribLocation(m_program, 3, "Tangent");

	for (unsigned int i = 0; i < 8; ++i)
	{
		NzString uniformName = "TexCoord" + NzString::Number(i);
		glBindAttribLocation(m_program, 4+i, uniformName.GetConstBuffer());
	}

	for (int i = 0; i < nzShaderType_Count; ++i)
		m_shaders[i] = 0;

	return true;
}

void NzGLSLShader::Destroy()
{
	for (GLuint shader : m_shaders)
		if (shader)
			glDeleteShader(shader);

	if (m_program)
		glDeleteProgram(m_program);
}

NzString NzGLSLShader::GetLog() const
{
	return m_log;
}

nzShaderLanguage NzGLSLShader::GetLanguage() const
{
	return nzShaderLanguage_GLSL;
}

NzString NzGLSLShader::GetSourceCode(nzShaderType type) const
{
	NzString source;

	GLint length;
	glGetShaderiv(m_shaders[type], GL_SHADER_SOURCE_LENGTH, &length);
	if (length > 1)
	{
		source.Resize(length-1); // La taille retournée est celle du buffer (Avec caractère de fin)
		glGetShaderSource(m_shaders[type], length, nullptr, &source[0]);
	}

	return source;
}

GLint NzGLSLShader::GetUniformLocation(const NzString& name) const
{
	std::map<NzString, GLint>::const_iterator it = m_idCache.find(name);
	GLint id;
	if (it == m_idCache.end())
	{
		id = glGetUniformLocation(m_program, name.GetConstBuffer());
		m_idCache[name] = id;
	}
	else
		id = it->second;

	return id;
}

bool NzGLSLShader::IsLoaded(nzShaderType type) const
{
	return m_shaders[type] != 0;
}

bool NzGLSLShader::Load(nzShaderType type, const NzString& source)
{
	GLuint shader = glCreateShader(shaderType[type]);
	if (!shader)
	{
		m_log = "Failed to create shader object";
		NazaraError(m_log);

		return false;
	}

	const char* tmp = source.GetConstBuffer();
	GLint length = source.GetSize();
	glShaderSource(shader, 1, &tmp, &length);

	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (success == GL_TRUE)
	{
		glAttachShader(m_program, shader);
		m_shaders[type] = shader;

		static NzString success("Compilation successful");
		m_log = success;

		return true;
	}
	else
	{
		// On remplit le log avec l'erreur de compilation
		length = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		if (length > 1)
		{
			m_log.Reserve(length+19-1); // La taille retournée est celle du buffer (Avec caractère de fin)
			m_log.Prepend("Compilation error: ");
			m_log.Resize(length+19-1); // Extension du buffer d'écriture pour ajouter le log

			glGetShaderInfoLog(shader, length-1, nullptr, &m_log[19]);
		}
		else
			m_log = "Compilation failed but no info log found";

		NazaraError(m_log);

		glDeleteShader(shader);

		return false;
	}
}

bool NzGLSLShader::SendBoolean(const NzString& name, bool value)
{
	glUniform1i(GetUniformLocation(name), value);

	return true;
}

bool NzGLSLShader::SendDouble(const NzString& name, double value)
{
	glUniform1d(GetUniformLocation(name), value);

	return true;
}

bool NzGLSLShader::SendFloat(const NzString& name, float value)
{
	glUniform1f(GetUniformLocation(name), value);

	return true;
}

bool NzGLSLShader::SendInteger(const NzString& name, int value)
{
	glUniform1i(GetUniformLocation(name), value);

	return true;
}

bool NzGLSLShader::SendMatrix(const NzString& name, const NzMatrix4d& matrix)
{
	glUniformMatrix4dv(GetUniformLocation(name), 1, GL_FALSE, matrix);

	return true;
}

bool NzGLSLShader::SendMatrix(const NzString& name, const NzMatrix4f& matrix)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, matrix);

	return true;
}

void NzGLSLShader::Unbind()
{
	glUseProgram(0);
}
