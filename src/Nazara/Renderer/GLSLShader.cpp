// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/GLSLShader.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <Nazara/Renderer/Debug.hpp>

namespace
{
	///FIXME: Déclaré deux fois (ici et dans Renderer.cpp)
	const nzUInt8 attribIndex[] =
	{
		2,	// nzElementUsage_Diffuse
		1,	// nzElementUsage_Normal
		0,	// nzElementUsage_Position
		3,	// nzElementUsage_Tangent
		4	// nzElementUsage_TexCoord
	};

	const GLenum shaderType[nzShaderType_Max+1] = {
		GL_FRAGMENT_SHADER,	// nzShaderType_Fragment
		GL_GEOMETRY_SHADER,	// nzShaderType_Geometry
		GL_VERTEX_SHADER	// nzShaderType_Vertex
	};

	GLuint lockedPrevious = 0;
	nzUInt8 lockedLevel = 0;
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
	#if NAZARA_RENDERER_SAFE
	if (lockedLevel > 0)
	{
		NazaraError("Cannot bind shader while a shader is locked");
		return false;
	}
	#endif

	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return false;
	}
	#endif

	glUseProgram(m_program);

	return true;
}

bool NzGLSLShader::BindTextures()
{
	for (auto it = m_textures.begin(); it != m_textures.end(); ++it)
	{
		TextureSlot& slot = it->second;
		if (!slot.updated)
		{
			glActiveTexture(GL_TEXTURE0 + slot.unit);
			if (!slot.texture->Bind())
				NazaraWarning("Failed to bind texture");

			slot.updated = true;
		}
	}

	return true;
}

bool NzGLSLShader::Compile()
{
	NzContext::EnsureContext();

	m_idCache.clear();
	m_textures.clear();

	glLinkProgram(m_program);

	GLint success;
	glGetProgramiv(m_program, GL_LINK_STATUS, &success);

	if (success == GL_TRUE)
	{
		static NzString successStr("Linkage successful");
		m_log = successStr;

		return true;
	}
	else
	{
		// On remplit le log avec l'erreur de compilation
		GLint length = 0;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &length);
		if (length > 1)
		{
			m_log.Clear(true);
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
	NzContext::EnsureContext();

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

	NzString uniform = "TexCoord";
	unsigned int maxTexCoords = NzRenderer::GetMaxTextureUnits();
	for (unsigned int i = 0; i < maxTexCoords; ++i)
	{
		NzString uniformName = uniform + NzString::Number(i);
		glBindAttribLocation(m_program, attribIndex[nzElementUsage_TexCoord]+i, uniformName.GetConstBuffer());
	}

	for (int i = 0; i <= nzShaderType_Max; ++i)
		m_shaders[i] = 0;

	return true;
}

void NzGLSLShader::Destroy()
{
	NzContext::EnsureContext();

	for (auto it = m_textures.begin(); it != m_textures.end(); ++it)
		it->second.texture->RemoveResourceReference();

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
	NzContext::EnsureContext();

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

int NzGLSLShader::GetUniformLocation(const NzString& name) const
{
	std::map<NzString, GLint>::const_iterator it = m_idCache.find(name);
	GLint id;
	if (it == m_idCache.end())
	{
		NzContext::EnsureContext();

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
	NzContext::EnsureContext();

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

		static NzString successStr("Compilation successful");
		m_log = successStr;

		return true;
	}
	else
	{
		// On remplit le log avec l'erreur de compilation
		length = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		if (length > 1)
		{
			m_log.Clear(true);
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

bool NzGLSLShader::Lock()
{
	if (lockedLevel++ == 0)
	{
		NzContext::EnsureContext();

		GLint previous;
		glGetIntegerv(GL_CURRENT_PROGRAM, &previous);

		lockedPrevious = previous;

		if (lockedPrevious != m_program)
			glUseProgram(m_program);
	}

	return true;
}

bool NzGLSLShader::SendBoolean(int location, bool value)
{
	if (glProgramUniform1i)
		glProgramUniform1i(m_program, location, value);
	else
	{
		Lock();
		glUniform1i(location, value);
		Unlock();
	}

	return true;
}

bool NzGLSLShader::SendDouble(int location, double value)
{
	if (glProgramUniform1d)
		glProgramUniform1d(m_program, location, value);
	else
	{
		Lock();
		glUniform1d(location, value);
		Unlock();
	}

	return true;
}

bool NzGLSLShader::SendFloat(int location, float value)
{
	if (glProgramUniform1f)
		glProgramUniform1f(m_program, location, value);
	else
	{
		Lock();
		glUniform1f(location, value);
		Unlock();
	}

	return true;
}

bool NzGLSLShader::SendInteger(int location, int value)
{
	if (glProgramUniform1i)
		glProgramUniform1i(m_program, location, value);
	else
	{
		Lock();
		glUniform1i(location, value);
		Unlock();
	}

	return true;
}

bool NzGLSLShader::SendMatrix(int location, const NzMatrix4d& matrix)
{
	if (glProgramUniformMatrix4dv)
		glProgramUniformMatrix4dv(m_program, location, 1, GL_FALSE, matrix);
	else
	{
		Lock();
		glUniformMatrix4dv(location, 1, GL_FALSE, matrix);
		Unlock();
	}

	return true;
}

bool NzGLSLShader::SendMatrix(int location, const NzMatrix4f& matrix)
{
	if (glProgramUniformMatrix4fv)
		glProgramUniformMatrix4fv(m_program, location, 1, GL_FALSE, matrix);
	else
	{
		Lock();
		glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
		Unlock();
	}

	return true;
}

bool NzGLSLShader::SendTexture(int location, const NzTexture* texture)
{
	auto it = m_textures.find(location);
	if (it != m_textures.end())
	{
		// Slot déjà utilisé
		TextureSlot& slot = it->second;
		if (slot.texture != texture)
		{
			slot.texture->RemoveResourceReference();

			if (texture)
			{
				slot.texture = texture;
				slot.texture->AddResourceReference();

				slot.updated = false;
			}
			else
				m_textures.erase(it); // On supprime le slot
		}
	}
	else
	{
		static const unsigned int maxUnits = NzRenderer::GetMaxTextureUnits();

		unsigned int unitUsed = m_textures.size();
		if (unitUsed >= maxUnits)
		{
			NazaraError("Unable to use texture for shader: all available texture units are used");
			return false;
		}

		// À partir d'ici nous savons qu'il y a au moins un identifiant de texture libre
		nzUInt8 unit;
		if (unitUsed == 0)
			// Pas d'unité utilisée, la tâche est simple
			unit = 0;
		else
		{
			auto it2 = m_textures.rbegin(); // Itérateur vers la fin de la map
			unit = it2->second.unit;
			if (unit == maxUnits-1)
			{
				// Il y a une place libre, mais pas à la fin
				for (; it2 != m_textures.rend(); ++it2)
				{
					if (unit - it2->second.unit > 1) // Si l'espace entre les indices est supérieur à 1, alors il y a une place libre
					{
						unit--;
						break;
					}
				}
			}
			else
				// Il y a une place libre à la fin
				unit++;
		}

		TextureSlot slot;
		slot.unit = unit;
		slot.texture = texture;
		texture->AddResourceReference();

		m_textures[location] = slot;

		if (glProgramUniform1i)
			glProgramUniform1i(m_program, location, unit);
		else
		{
			Lock();
			glUniform1i(location, unit);
			Unlock();
		}
	}

	return true;
}

bool NzGLSLShader::SendVector(int location, const NzVector2d& vector)
{
	if (glProgramUniform2dv)
		glProgramUniform2dv(m_program, location, 1, vector);
	else
	{
		Lock();
		glUniform2dv(location, 1, vector);
		Unlock();
	}

	return true;
}

bool NzGLSLShader::SendVector(int location, const NzVector2f& vector)
{
	if (glProgramUniform2fv)
		glProgramUniform2fv(m_program, location, 1, vector);
	else
	{
		Lock();
		glUniform2fv(location, 1, vector);
		Unlock();
	}

	return true;
}

bool NzGLSLShader::SendVector(int location, const NzVector3d& vector)
{
	if (glProgramUniform3dv)
		glProgramUniform3dv(m_program, location, 1, vector);
	else
	{
		Lock();
		glUniform3dv(location, 1, vector);
		Unlock();
	}

	return true;
}

bool NzGLSLShader::SendVector(int location, const NzVector3f& vector)
{
	if (glProgramUniform3fv)
		glProgramUniform3fv(m_program, location, 1, vector);
	else
	{
		Lock();
		glUniform3fv(location, 1, vector);
		Unlock();
	}

	return true;
}

bool NzGLSLShader::SendVector(int location, const NzVector4d& vector)
{
	if (glProgramUniform4dv)
		glProgramUniform4dv(m_program, location, 1, vector);
	else
	{
		Lock();
		glUniform4dv(location, 1, vector);
		Unlock();
	}

	return true;
}

bool NzGLSLShader::SendVector(int location, const NzVector4f& vector)
{
	if (glProgramUniform4fv)
		glProgramUniform4fv(m_program, location, 1, vector);
	else
	{
		Lock();
		glUniform4fv(location, 1, vector);
		Unlock();
	}

	return true;
}

void NzGLSLShader::Unbind()
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}
	#endif

	glUseProgram(0);
}

void NzGLSLShader::Unlock()
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return;
	}
	#endif

	#if NAZARA_RENDERER_SAFE
	if (lockedLevel == 0)
	{
		NazaraWarning("Unlock called on non-locked texture");
		return;
	}
	#endif

	if (--lockedLevel == 0 && lockedPrevious != m_program)
		glUseProgram(lockedPrevious);
}
