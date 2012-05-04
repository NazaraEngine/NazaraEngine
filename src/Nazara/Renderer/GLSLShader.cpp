// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/GLSLShader.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Renderer/BufferImpl.hpp>
#include <Nazara/Renderer/VertexBuffer.hpp>
#include <Nazara/Renderer/VertexDeclaration.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace
{
	nzUInt8 attribIndex[] =
	{
		2,	// nzElementUsage_Diffuse
		1,	// nzElementUsage_Normal
		0,	// nzElementUsage_Position
		3,	// nzElementUsage_Tangent
		4	// nzElementUsage_TexCoord
	};

	const GLenum shaderType[nzShaderType_Count] = {
		GL_FRAGMENT_SHADER,	// nzShaderType_Fragment
		GL_GEOMETRY_SHADER,	// nzShaderType_Geometry
		GL_VERTEX_SHADER	// nzShaderType_Vertex
	};

	const nzUInt8 size[] =
	{
		4, // nzElementType_Color
		1, // nzElementType_Double1
		2, // nzElementType_Double2
		3, // nzElementType_Double3
		4, // nzElementType_Double4
		1, // nzElementType_Float1
		2, // nzElementType_Float2
		3, // nzElementType_Float3
		4  // nzElementType_Float4
	};

	const GLenum type[] =
	{
		GL_UNSIGNED_BYTE, // nzElementType_Color
		GL_DOUBLE,		  // nzElementType_Double1
		GL_DOUBLE,		  // nzElementType_Double2
		GL_DOUBLE,		  // nzElementType_Double3
		GL_DOUBLE,		  // nzElementType_Double4
		GL_FLOAT,		  // nzElementType_Float1
		GL_FLOAT,		  // nzElementType_Float2
		GL_FLOAT,		  // nzElementType_Float3
		GL_FLOAT		  // nzElementType_Float4
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

	glBindAttribLocation(m_program, attribIndex[nzElementUsage_Position], "Position");
	glBindAttribLocation(m_program, attribIndex[nzElementUsage_Normal], "Normal");
	glBindAttribLocation(m_program, attribIndex[nzElementUsage_Diffuse], "Diffuse");
	glBindAttribLocation(m_program, attribIndex[nzElementUsage_Tangent], "Tangent");

	NzString uniformName = "TexCoord0";
	for (unsigned int i = 0; i < 8; ++i)
	{
		uniformName[8] = '0'+i;
		glBindAttribLocation(m_program, attribIndex[nzElementUsage_TexCoord]+i, uniformName.GetConstBuffer());
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

bool NzGLSLShader::Lock() const
{
	if (m_lockedLevel++ == 0)
	{
		GLint previous;
		glGetIntegerv(GL_CURRENT_PROGRAM, &previous);

		m_lockedPrevious = previous;

		if (m_lockedPrevious != m_program)
			glUseProgram(m_program);
	}

	return true;
}

bool NzGLSLShader::SendBoolean(const NzString& name, bool value)
{
	Lock();
	glUniform1i(GetUniformLocation(name), value);
	Unlock();

	return true;
}

bool NzGLSLShader::SendDouble(const NzString& name, double value)
{
	Lock();
	glUniform1d(GetUniformLocation(name), value);
	Unlock();

	return true;
}

bool NzGLSLShader::SendFloat(const NzString& name, float value)
{
	Lock();
	glUniform1f(GetUniformLocation(name), value);
	Unlock();

	return true;
}

bool NzGLSLShader::SendInteger(const NzString& name, int value)
{
	Lock();
	glUniform1i(GetUniformLocation(name), value);
	Unlock();

	return true;
}

bool NzGLSLShader::SendMatrix(const NzString& name, const NzMatrix4d& matrix)
{
	Lock();
	glUniformMatrix4dv(GetUniformLocation(name), 1, GL_FALSE, matrix);
	Unlock();

	return true;
}

bool NzGLSLShader::SendMatrix(const NzString& name, const NzMatrix4f& matrix)
{
	Lock();
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, matrix);
	Unlock();

	return true;
}

void NzGLSLShader::Unbind()
{
	glUseProgram(0);
}

void NzGLSLShader::Unlock() const
{
	if (m_lockedLevel == 0)
	{
		NazaraWarning("Unlock called on non-locked texture");
		return;
	}

	if (--m_lockedLevel == 0 && m_lockedPrevious != m_program)
		glUseProgram(m_lockedPrevious);
}

bool NzGLSLShader::UpdateVertexBuffer(const NzVertexBuffer* vertexBuffer, const NzVertexDeclaration* vertexDeclaration)
{
	vertexBuffer->GetBuffer()->GetImpl()->Bind();
	const nzUInt8* buffer = reinterpret_cast<const nzUInt8*>(vertexBuffer->GetBufferPtr());

	///FIXME: Améliorer les déclarations pour permettre de faire ça plus simplement
	for (int i = 0; i < 12; ++i) // Solution temporaire, à virer
		glDisableVertexAttribArray(i); // Chaque itération tue un chaton :(

	unsigned int stride = vertexDeclaration->GetStride();
	unsigned int elementCount = vertexDeclaration->GetElementCount();
	for (unsigned int i = 0; i < elementCount; ++i)
	{
		const NzVertexDeclaration::Element* element = vertexDeclaration->GetElement(i);
		glEnableVertexAttribArray(attribIndex[element->usage]+element->usageIndex);
		glVertexAttribPointer(attribIndex[element->usage]+element->usageIndex,
							  size[element->type],
							  type[element->type],
							  (element->type == nzElementType_Color) ? GL_TRUE : GL_FALSE,
							  stride,
							  &buffer[element->offset]);
	}

	return true;
}
