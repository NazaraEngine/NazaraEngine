// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/GLSLProgram.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>
#include <Nazara/Renderer/Debug.hpp>

NzGLSLProgram::NzGLSLProgram(NzShaderProgram* parent) :
m_parent(parent)
{
}

bool NzGLSLProgram::Bind()
{
	#ifdef NAZARA_DEBUG
	if (NzContext::GetCurrent() == nullptr)
	{
		NazaraError("No active context");
		return false;
	}
	#endif

	NzOpenGL::BindProgram(m_program);

	return true;
}

bool NzGLSLProgram::BindTextures()
{
	for (const std::pair<GLint, TextureSlot>& pair : m_textures)
	{
		const TextureSlot& slot = pair.second;
		if (slot.enabled)
			NzRenderer::SetTexture(slot.unit, slot.texture);
	}

	return true;
}

bool NzGLSLProgram::Compile()
{
	NzContext::EnsureContext();

	PreLinkage();

	glLinkProgram(m_program);

	return PostLinkage();
}

bool NzGLSLProgram::Create()
{
	NzContext::EnsureContext();

	m_program = glCreateProgram();
	if (!m_program)
	{
		NazaraError("Failed to create program");
		return false;
	}

	glBindAttribLocation(m_program, NzOpenGL::AttributeIndex[nzAttributeUsage_InstanceData0], "InstanceData0");
	glBindAttribLocation(m_program, NzOpenGL::AttributeIndex[nzAttributeUsage_InstanceData1], "InstanceData1");
	glBindAttribLocation(m_program, NzOpenGL::AttributeIndex[nzAttributeUsage_InstanceData2], "InstanceData2");
	glBindAttribLocation(m_program, NzOpenGL::AttributeIndex[nzAttributeUsage_InstanceData3], "InstanceData3");
	glBindAttribLocation(m_program, NzOpenGL::AttributeIndex[nzAttributeUsage_InstanceData4], "InstanceData4");
	glBindAttribLocation(m_program, NzOpenGL::AttributeIndex[nzAttributeUsage_InstanceData5], "InstanceData5");
	glBindAttribLocation(m_program, NzOpenGL::AttributeIndex[nzAttributeUsage_Normal], "VertexNormal");
	glBindAttribLocation(m_program, NzOpenGL::AttributeIndex[nzAttributeUsage_Position], "VertexPosition");
	glBindAttribLocation(m_program, NzOpenGL::AttributeIndex[nzAttributeUsage_Tangent], "VertexTangent");
	glBindAttribLocation(m_program, NzOpenGL::AttributeIndex[nzAttributeUsage_TexCoord], "VertexTexCoord");
	glBindAttribLocation(m_program, NzOpenGL::AttributeIndex[nzAttributeUsage_Userdata0], "VertexUserdata0");
	glBindAttribLocation(m_program, NzOpenGL::AttributeIndex[nzAttributeUsage_Userdata1], "VertexUserdata1");
	glBindAttribLocation(m_program, NzOpenGL::AttributeIndex[nzAttributeUsage_Userdata2], "VertexUserdata2");
	glBindAttribLocation(m_program, NzOpenGL::AttributeIndex[nzAttributeUsage_Userdata3], "VertexUserdata3");
	glBindAttribLocation(m_program, NzOpenGL::AttributeIndex[nzAttributeUsage_Userdata4], "VertexUserdata4");
	glBindAttribLocation(m_program, NzOpenGL::AttributeIndex[nzAttributeUsage_Userdata5], "VertexUserdata5");

	if (NzRenderer::HasCapability(nzRendererCap_MultipleRenderTargets))
	{
		NzString uniform;
		uniform = "RenderTarget";

		unsigned int maxRenderTargets = NzRenderer::GetMaxRenderTargets();
		for (unsigned int i = 0; i < maxRenderTargets; ++i)
		{
			NzString uniformName = uniform + NzString::Number(i);
			glBindFragDataLocation(m_program, i, uniformName.GetConstBuffer());
		}
	}

	for (int i = 0; i <= nzShaderType_Max; ++i)
		m_shaders[i] = 0;

	if (NzOpenGL::IsSupported(nzOpenGLExtension_GetProgramBinary))
		glProgramParameteri(m_program, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);

	return true;
}

void NzGLSLProgram::Destroy()
{
	NzContext::EnsureContext();

	for (auto it = m_textures.begin(); it != m_textures.end(); ++it)
		it->second.texture->RemoveResourceListener(this);

	NzOpenGL::DeleteProgram(m_program);
}

NzByteArray NzGLSLProgram::GetBinary() const
{
	NzByteArray byteArray;

	NzContext::EnsureContext();

	GLint binaryLength = 0;
	glGetProgramiv(m_program, GL_PROGRAM_BINARY_LENGTH, &binaryLength);

	if (binaryLength > 0)
	{
		byteArray.Resize(sizeof(nzUInt64) + binaryLength);

		nzUInt8* buffer = byteArray.GetBuffer();

		GLenum binaryFormat;
		glGetProgramBinary(m_program, binaryLength, nullptr, &binaryFormat, &buffer[sizeof(nzUInt64)]);

		// On stocke le format au début du binaire
		*reinterpret_cast<nzUInt64*>(&buffer[0]) = binaryFormat;
	}

	return byteArray;
}

NzString NzGLSLProgram::GetLog() const
{
	return m_log;
}

nzShaderLanguage NzGLSLProgram::GetLanguage() const
{
	return nzShaderLanguage_GLSL;
}

NzString NzGLSLProgram::GetSourceCode(nzShaderType type) const
{
	NzContext::EnsureContext();

	NzString source;

	GLint length = 0;
	glGetShaderiv(m_shaders[type], GL_SHADER_SOURCE_LENGTH, &length);
	if (length > 1)
	{
		source.Resize(length-1); // La taille retournée est celle du buffer (Avec caractère de fin)
		glGetShaderSource(m_shaders[type], length, nullptr, &source[0]);
	}

	return source;
}

int NzGLSLProgram::GetUniformLocation(const NzString& name) const
{
	auto it = m_idCache.find(name);
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

int NzGLSLProgram::GetUniformLocation(nzShaderUniform uniform) const
{
	return m_uniformLocations[uniform];
}

bool NzGLSLProgram::IsBinaryRetrievable() const
{
	return NzOpenGL::IsSupported(nzOpenGLExtension_GetProgramBinary);
}

bool NzGLSLProgram::IsLoaded(nzShaderType type) const
{
	return m_shaders[type] != 0;
}

bool NzGLSLProgram::LoadFromBinary(const void* buffer, unsigned int size)
{
	#if NAZARA_RENDERER_SAFE
	if (!glProgramBinary)
	{
		NazaraError("GL_ARB_get_program_binary not supported");
		return false;
	}
	#endif

	NzContext::EnsureContext();

	const nzUInt8* ptr = reinterpret_cast<const nzUInt8*>(buffer);

	// On récupère le format au début du binaire
	///TODO: ByteStream ?
	GLenum binaryFormat = static_cast<GLenum>(*reinterpret_cast<const nzUInt64*>(&ptr[0]));
	ptr += sizeof(nzUInt64);

	PreLinkage();

	glProgramBinary(m_program, binaryFormat, ptr, size-sizeof(nzUInt64));

	return PostLinkage();
}

bool NzGLSLProgram::LoadShader(nzShaderType type, const NzString& source)
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
		glAttachShader(m_program, shader); // On attache le shader au programme
		glDeleteShader(shader); // On le marque pour suppression (Lors de l'appel à glDeleteProgram)

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

bool NzGLSLProgram::SendBoolean(int location, bool value)
{
	if (glProgramUniform1i)
		glProgramUniform1i(m_program, location, value);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform1i(location, value);
	}

	return true;
}

bool NzGLSLProgram::SendColor(int location, const NzColor& color)
{
	NzVector4f vecColor(color.r/255.f, color.g/255.f, color.b/255.f, color.a/255.f);

	if (glProgramUniform4fv)
		glProgramUniform4fv(m_program, location, 1, vecColor);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform4fv(location, 1, vecColor);
	}

	return true;
}

bool NzGLSLProgram::SendDouble(int location, double value)
{
	if (glProgramUniform1d)
		glProgramUniform1d(m_program, location, value);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform1d(location, value);
	}

	return true;
}

bool NzGLSLProgram::SendFloat(int location, float value)
{
	if (glProgramUniform1f)
		glProgramUniform1f(m_program, location, value);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform1f(location, value);
	}

	return true;
}

bool NzGLSLProgram::SendInteger(int location, int value)
{
	if (glProgramUniform1i)
		glProgramUniform1i(m_program, location, value);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform1i(location, value);
	}

	return true;
}

bool NzGLSLProgram::SendMatrix(int location, const NzMatrix4d& matrix)
{
	if (glProgramUniformMatrix4dv)
		glProgramUniformMatrix4dv(m_program, location, 1, GL_FALSE, matrix);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniformMatrix4dv(location, 1, GL_FALSE, matrix);
	}

	return true;
}

bool NzGLSLProgram::SendMatrix(int location, const NzMatrix4f& matrix)
{
	if (glProgramUniformMatrix4fv)
		glProgramUniformMatrix4fv(m_program, location, 1, GL_FALSE, matrix);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
	}

	return true;
}

bool NzGLSLProgram::SendTexture(int location, const NzTexture* texture, nzUInt8* textureUnit)
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

				 if (textureUnit)
					*textureUnit = slot.unit;
			}
			else
				slot.enabled = false;
		}
		else if (textureUnit)
			*textureUnit = slot.unit;
	}
	else if (texture)
	{
		unsigned int maxUnits = NzRenderer::GetMaxTextureUnits();
		unsigned int unitUsed = m_textures.size();
		if (unitUsed >= maxUnits)
		{
			NazaraError("Unable to use texture for shader: all available texture units are in use");
			return false;
		}

		// À partir d'ici nous savons qu'il y a au moins un identifiant de texture libre

		TextureSlot slot;
		slot.enabled = texture->IsValid();
		slot.unit = unitUsed+1;
		slot.texture = texture;

		if (slot.enabled)
		{
			if (glProgramUniform1i)
				glProgramUniform1i(m_program, location, slot.unit);
			else
			{
				NzOpenGL::BindProgram(m_program);
				glUniform1i(location, slot.unit);
			}
		}

		m_textures[location] = slot;
		texture->AddResourceListener(this, location);

		if (textureUnit)
			*textureUnit = slot.unit;
	}

	return true;
}

bool NzGLSLProgram::SendVector(int location, const NzVector2d& vector)
{
	if (glProgramUniform2dv)
		glProgramUniform2dv(m_program, location, 1, vector);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform2dv(location, 1, vector);
	}

	return true;
}

bool NzGLSLProgram::SendVector(int location, const NzVector2f& vector)
{
	if (glProgramUniform2fv)
		glProgramUniform2fv(m_program, location, 1, vector);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform2fv(location, 1, vector);
	}

	return true;
}

bool NzGLSLProgram::SendVector(int location, const NzVector3d& vector)
{
	if (glProgramUniform3dv)
		glProgramUniform3dv(m_program, location, 1, vector);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform3dv(location, 1, vector);
	}

	return true;
}

bool NzGLSLProgram::SendVector(int location, const NzVector3f& vector)
{
	if (glProgramUniform3fv)
		glProgramUniform3fv(m_program, location, 1, vector);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform3fv(location, 1, vector);
	}

	return true;
}

bool NzGLSLProgram::SendVector(int location, const NzVector4d& vector)
{
	if (glProgramUniform4dv)
		glProgramUniform4dv(m_program, location, 1, vector);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform4dv(location, 1, vector);
	}

	return true;
}

bool NzGLSLProgram::SendVector(int location, const NzVector4f& vector)
{
	if (glProgramUniform4fv)
		glProgramUniform4fv(m_program, location, 1, vector);
	else
	{
		NzOpenGL::BindProgram(m_program);
		glUniform4fv(location, 1, vector);
	}

	return true;
}

bool NzGLSLProgram::OnResourceCreated(const NzResource* resource, int index)
{
	NazaraUnused(resource);

	auto it = m_textures.find(index);

	#ifdef NAZARA_DEBUG
	if (it == m_textures.end())
	{
		NazaraInternalError("Invalid index (" + NzString::Number(index) + ')');
		return false;
	}
	#endif

	TextureSlot& slot = it->second;

	#ifdef NAZARA_DEBUG
	if (slot.texture != resource)
	{
		NazaraInternalError("Wrong texture at location #" + NzString::Number(index));
		return false;
	}
	#endif

	slot.enabled = true;
	slot.updated = false;

	return true;
}

bool NzGLSLProgram::OnResourceDestroy(const NzResource* resource, int index)
{
	NazaraUnused(resource);

	auto it = m_textures.find(index);

	#ifdef NAZARA_DEBUG
	if (it == m_textures.end())
	{
		NazaraInternalError("Invalid index (" + NzString::Number(index) + ')');
		return false;
	}
	#endif

	TextureSlot& slot = it->second;

	#ifdef NAZARA_DEBUG
	if (slot.texture != resource)
	{
		NazaraInternalError("Wrong texture at location #" + NzString::Number(index));
		return false;
	}
	#endif

	slot.enabled = false;

	return true;
}

void NzGLSLProgram::OnResourceReleased(const NzResource* resource, int index)
{
	if (m_textures.erase(index) == 0)
		NazaraInternalError("Texture " + NzString::Pointer(resource) + " not found");
}

void NzGLSLProgram::PreLinkage()
{
	m_idCache.clear();
	m_textures.clear();
}

bool NzGLSLProgram::PostLinkage()
{
	// On suppose qu'un contexte OpenGL est actif à l'appel de cette fonction
	GLint success;
	glGetProgramiv(m_program, GL_LINK_STATUS, &success);

	if (success == GL_TRUE)
	{
		static NzString successStr("Linkage successful");
		m_log = successStr;

		// Pour éviter de se tromper entre le nom et la constante
		#define CacheUniform(name) m_uniformLocations[nzShaderUniform_##name] = GetUniformLocation(#name)

		CacheUniform(EyePosition);
		CacheUniform(InvProjMatrix);
		CacheUniform(InvTargetSize);
		CacheUniform(InvViewMatrix);
		CacheUniform(InvViewProjMatrix);
		CacheUniform(InvWorldMatrix);
		CacheUniform(InvWorldViewMatrix);
		CacheUniform(InvWorldViewProjMatrix);
		CacheUniform(MaterialAlphaMap);
		CacheUniform(MaterialAlphaThreshold);
		CacheUniform(MaterialAmbient);
		CacheUniform(MaterialDiffuse);
		CacheUniform(MaterialDiffuseMap);
		CacheUniform(MaterialEmissiveMap);
		CacheUniform(MaterialHeightMap);
		CacheUniform(MaterialNormalMap);
		CacheUniform(MaterialShininess);
		CacheUniform(MaterialSpecular);
		CacheUniform(MaterialSpecularMap);
		CacheUniform(ProjMatrix);
		CacheUniform(SceneAmbient);
		CacheUniform(TargetSize);
		CacheUniform(VertexDepth);
		CacheUniform(ViewMatrix);
		CacheUniform(ViewProjMatrix);
		CacheUniform(WorldMatrix);
		CacheUniform(WorldViewMatrix);
		CacheUniform(WorldViewProjMatrix);

		#undef CacheUniform

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
