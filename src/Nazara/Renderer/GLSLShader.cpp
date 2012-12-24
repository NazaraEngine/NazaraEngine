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
	GLuint lockedPrevious = 0;
	nzUInt8 lockedLevel = 0;
}

NzGLSLShader::NzGLSLShader(NzShader* parent) :
m_parent(parent)
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
		if (slot.enabled)
			NzRenderer::SetTexture(slot.unit, slot.texture);
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
			m_log.Reserve(length+15-2); // La taille retournée est celle du buffer (Avec caractère de fin)
			m_log.Prepend("Linkage error: ");
			m_log.Resize(length+15-2); // Extension du buffer d'écriture pour ajouter le log

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

	glBindAttribLocation(m_program, NzOpenGL::AttributeIndex[nzElementUsage_Position], "Position");
	glBindAttribLocation(m_program, NzOpenGL::AttributeIndex[nzElementUsage_Normal], "Normal");
	glBindAttribLocation(m_program, NzOpenGL::AttributeIndex[nzElementUsage_Diffuse], "Diffuse");
	glBindAttribLocation(m_program, NzOpenGL::AttributeIndex[nzElementUsage_Tangent], "Tangent");

	NzString uniform;

	static const unsigned int maxTexCoords = NzRenderer::GetMaxTextureUnits();

	uniform.Reserve(10); // 8 + 2
	uniform = "TexCoord";
	for (unsigned int i = 0; i < maxTexCoords; ++i)
	{
		NzString uniformName = uniform + NzString::Number(i);
		glBindAttribLocation(m_program, NzOpenGL::AttributeIndex[nzElementUsage_TexCoord]+i, uniformName.GetConstBuffer());
	}

	static const bool mrtSupported = NzRenderer::HasCapability(nzRendererCap_MultipleRenderTargets);
	if (mrtSupported)
	{
		static const unsigned int maxRenderTargets = NzRenderer::GetMaxRenderTargets();

		uniform.Reserve(14); // 12 + 2
		uniform = "RenderTarget";
		for (unsigned int i = 0; i < maxRenderTargets; ++i)
		{
			NzString uniformName = uniform + NzString::Number(i);
			glBindFragDataLocation(m_program, i, uniformName.GetConstBuffer());
		}
	}

	for (int i = 0; i <= nzShaderType_Max; ++i)
		m_shaders[i] = 0;

	return true;
}

void NzGLSLShader::Destroy()
{
	NzContext::EnsureContext();

	for (auto it = m_textures.begin(); it != m_textures.end(); ++it)
		it->second.texture->RemoveResourceListener(this);

	for (GLuint shader : m_shaders)
	{
		if (shader)
			glDeleteShader(shader);
	}

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

	GLuint shader = glCreateShader(NzOpenGL::ShaderType[type]);
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
			m_log.Reserve(length+19-2); // La taille retournée est celle du buffer (Avec caractère de fin)
			m_log.Prepend("Compilation error: ");
			m_log.Resize(length+19-2); // Extension du buffer d'écriture pour ajouter le log

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
		if (!Lock())
		{
			NazaraError("Failed to lock shader");
			return false;
		}

		glUniform1i(location, value);
		Unlock();
	}

	return true;
}

bool NzGLSLShader::SendColor(int location, const NzColor& color)
{
	NzVector3f vecColor(color.r/255.f, color.g/255.f, color.b/255.f);

	if (glProgramUniform3fv)
		glProgramUniform3fv(m_program, location, 1, vecColor);
	else
	{
		if (!Lock())
		{
			NazaraError("Failed to lock shader");
			return false;
		}

		glUniform3fv(location, 1, vecColor);
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
		if (!Lock())
		{
			NazaraError("Failed to lock shader");
			return false;
		}

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
		if (!Lock())
		{
			NazaraError("Failed to lock shader");
			return false;
		}

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
		if (!Lock())
		{
			NazaraError("Failed to lock shader");
			return false;
		}

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
		if (!Lock())
		{
			NazaraError("Failed to lock shader");
			return false;
		}

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
		if (!Lock())
		{
			NazaraError("Failed to lock shader");
			return false;
		}

		glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
		Unlock();
	}

	return true;
}

bool NzGLSLShader::SendTexture(int location, const NzTexture* texture, nzUInt8* textureUnit)
{
	auto it = m_textures.find(location);
	if (it != m_textures.end())
	{
		// Slot déjà utilisé
		TextureSlot& slot = it->second;
		if (slot.texture != texture)
		{
			slot.texture->RemoveResourceListener(this);

			if (texture)
			{
				slot.texture = texture;
				slot.texture->AddResourceListener(this, location);

				slot.updated = false;
			}
			else
				m_textures.erase(it); // On supprime le slot
		}

		if (textureUnit)
			*textureUnit = slot.unit;
	}
	else
	{
		static const unsigned int maxUnits = NzRenderer::GetMaxTextureUnits();

		unsigned int unitUsed = m_textures.size();
		if (unitUsed >= maxUnits)
		{
			NazaraError("Unable to use texture for shader: all available texture units are in use");
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
		slot.enabled = texture->IsValid();
		slot.unit = unit;
		slot.texture = texture;

		if (slot.enabled)
		{
			if (glProgramUniform1i)
				glProgramUniform1i(m_program, location, unit);
			else
			{
				if (!Lock())
				{
					NazaraError("Failed to lock shader");
					return false;
				}

				glUniform1i(location, unit);
				Unlock();
			}
		}

		m_textures[location] = slot;
		texture->AddResourceListener(this, location);

		if (textureUnit)
			*textureUnit = unit;
	}

	return true;
}

bool NzGLSLShader::SendVector(int location, const NzVector2d& vector)
{
	if (glProgramUniform2dv)
		glProgramUniform2dv(m_program, location, 1, vector);
	else
	{
		if (!Lock())
		{
			NazaraError("Failed to lock shader");
			return false;
		}

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
		if (!Lock())
		{
			NazaraError("Failed to lock shader");
			return false;
		}

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
		if (!Lock())
		{
			NazaraError("Failed to lock shader");
			return false;
		}

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
		if (!Lock())
		{
			NazaraError("Failed to lock shader");
			return false;
		}

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
		if (!Lock())
		{
			NazaraError("Failed to lock shader");
			return false;
		}

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
		if (!Lock())
		{
			NazaraError("Failed to lock shader");
			return false;
		}

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

void NzGLSLShader::OnResourceCreated(const NzResource* resource, int index)
{
	NazaraUnused(resource);

	auto it = m_textures.find(index);

	#ifdef NAZARA_DEBUG
	if (it == m_textures.end())
	{
		NazaraInternalError("Invalid index (" + NzString::Number(index) + ')');
		return;
	}
	#endif

	TextureSlot& slot = it->second;

	#ifdef NAZARA_DEBUG
	if (slot.texture != resource)
	{
		NazaraInternalError("Wrong texture at location #" + NzString::Number(index));
		return;
	}
	#endif

	slot.enabled = true;
	slot.updated = false;
}

void NzGLSLShader::OnResourceDestroy(const NzResource* resource, int index)
{
	NazaraUnused(resource);

	auto it = m_textures.find(index);

	#ifdef NAZARA_DEBUG
	if (it == m_textures.end())
	{
		NazaraInternalError("Invalid index (" + NzString::Number(index) + ')');
		return;
	}
	#endif

	TextureSlot& slot = it->second;

	#ifdef NAZARA_DEBUG
	if (slot.texture != resource)
	{
		NazaraInternalError("Wrong texture at location #" + NzString::Number(index));
		return;
	}
	#endif

	slot.enabled = false;
}

void NzGLSLShader::OnResourceReleased(const NzResource* resource, int index)
{
	if (m_textures.erase(index) == 0)
		NazaraInternalError("Texture " + NzString::Pointer(resource) + " not found");

	resource->RemoveResourceListener(this);
}
