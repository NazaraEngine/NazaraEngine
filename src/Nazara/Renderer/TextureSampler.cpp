// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <unordered_map>
#include <Nazara/Renderer/Debug.hpp>

namespace
{
	std::unordered_map<nzUInt32, GLuint> s_samplers;
	nzUInt8 s_maxAnisotropyLevel;
	bool s_useAnisotropicFilter;
}

NzTextureSampler::NzTextureSampler() :
m_filterMode(nzSamplerFilter_Default),
m_wrapMode(nzSamplerWrap_Default),
m_anisotropicLevel(0),
m_mipmaps(true),
m_samplerId(0)
{
}

nzUInt8 NzTextureSampler::GetAnisotropicLevel() const
{
	return m_anisotropicLevel;
}

nzSamplerFilter NzTextureSampler::GetFilterMode() const
{
	return m_filterMode;
}

nzSamplerWrap NzTextureSampler::GetWrapMode() const
{
	return m_wrapMode;
}

void NzTextureSampler::SetAnisotropyLevel(nzUInt8 anisotropyLevel)
{
	#ifdef NAZARA_DEBUG
	if (!NzRenderer::IsInitialized())
	{
		NazaraError("Renderer module must be initialized");
		return;
	}
	#endif

	if (m_anisotropicLevel != anisotropyLevel)
	{
		if (anisotropyLevel > s_maxAnisotropyLevel)
		{
			NazaraWarning("Anisotropy level is over maximum anisotropy level (" + NzString::Number(anisotropyLevel) + " > " + NzString::Number(s_maxAnisotropyLevel));
			anisotropyLevel = s_maxAnisotropyLevel;
		}

		m_anisotropicLevel = anisotropyLevel;
		m_samplerId = 0;
	}
}

void NzTextureSampler::SetFilterMode(nzSamplerFilter filterMode)
{
	if (m_filterMode != filterMode)
	{
		m_filterMode = filterMode;
		m_samplerId = 0;
	}
}

void NzTextureSampler::SetWrapMode(nzSamplerWrap wrapMode)
{
	if (m_wrapMode != wrapMode)
	{
		m_wrapMode = wrapMode;
		m_samplerId = 0;
	}
}

nzUInt8 NzTextureSampler::GetDefaultAnisotropicLevel()
{
	return s_defaultAnisotropyLevel;
}

nzSamplerFilter NzTextureSampler::GetDefaultFilterMode()
{
	return s_defaultFilterMode;
}

nzSamplerWrap NzTextureSampler::GetDefaultWrapMode()
{
	return s_defaultWrapMode;
}

void NzTextureSampler::SetDefaultAnisotropyLevel(nzUInt8 anisotropyLevel)
{
	#if NAZARA_RENDERER_SAFE
	if (anisotropyLevel == 0)
	{
		NazaraError("Default anisotropy level mode cannot be set to default value (0)");
		return;
	}
	#endif

	#ifdef NAZARA_DEBUG
	if (!NzRenderer::IsInitialized())
	{
		NazaraError("Renderer module must be initialized");
		return;
	}
	#endif

	if (anisotropyLevel > s_maxAnisotropyLevel)
	{
		NazaraWarning("Anisotropy level is over maximum anisotropy level (" + NzString::Number(anisotropyLevel) + " > " + NzString::Number(s_maxAnisotropyLevel));
		anisotropyLevel = s_maxAnisotropyLevel;
	}

	s_defaultAnisotropyLevel = anisotropyLevel;

	if (s_useAnisotropicFilter)
	{
		for (const std::pair<nzUInt32, GLuint>& pair : s_samplers)
		{
			if (((pair.first >> 5) & 0xFF) == 0)
				glSamplerParameterf(pair.second, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<float>(anisotropyLevel));
		}
	}
}

void NzTextureSampler::SetDefaultFilterMode(nzSamplerFilter filterMode)
{
	#if NAZARA_RENDERER_SAFE
	if (filterMode == nzSamplerFilter_Default)
	{
		NazaraError("Default filter mode cannot be set to default enum value (nzSamplerFilter_Default)");
		return;
	}
	#endif

	s_defaultFilterMode = filterMode;

	for (const std::pair<nzUInt32, GLuint>& pair : s_samplers)
	{
		if (((pair.first >> 1) & 0x3) == nzSamplerFilter_Default)
		{
			bool mipmaps = pair.first & 0x1;
			switch (filterMode)
			{
				case nzSamplerFilter_Bilinear:
					if (mipmaps)
						glSamplerParameteri(pair.second, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
					else
						glSamplerParameteri(pair.second, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

					glSamplerParameteri(pair.second, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					break;

				case nzSamplerFilter_Nearest:
					if (mipmaps)
						glSamplerParameteri(pair.second, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
					else
						glSamplerParameteri(pair.second, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

					glSamplerParameteri(pair.second, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					break;

				case nzSamplerFilter_Trilinear:
					if (mipmaps)
						glSamplerParameteri(pair.second, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
					else
						glSamplerParameteri(pair.second, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Filtrage bilinéaire

					glSamplerParameteri(pair.second, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					break;

				default:
					NazaraError("Texture filter not handled (0x" + NzString::Number(filterMode, 16) + ')');
					break;
			}
		}
	}
}

void NzTextureSampler::SetDefaultWrapMode(nzSamplerWrap wrapMode)
{
	#if NAZARA_RENDERER_SAFE
	if (wrapMode == nzSamplerWrap_Default)
	{
		NazaraError("Default wrap mode cannot be set to default enum value (nzSamplerWrap_Default)");
		return;
	}
	#endif

	s_defaultWrapMode = wrapMode;

	GLenum wrapEnum = NzOpenGL::SamplerWrapMode[wrapMode];
	for (const std::pair<nzUInt32, GLuint>& pair : s_samplers)
	{
		if (((pair.first >> 3) & 0x3) == nzSamplerWrap_Default)
		{
			glSamplerParameteri(pair.second, GL_TEXTURE_WRAP_R, wrapEnum);
			glSamplerParameteri(pair.second, GL_TEXTURE_WRAP_T, wrapEnum);
			glSamplerParameteri(pair.second, GL_TEXTURE_WRAP_S, wrapEnum);
		}
	}
}

void NzTextureSampler::Apply(const NzTexture* texture) const
{
	nzImageType type = texture->GetType();
	GLenum target = NzOpenGL::TextureTarget[type];

	NzOpenGL::BindTexture(type, texture->GetOpenGLID());

	if (s_useAnisotropicFilter)
	{
		nzUInt8 anisotropyLevel = (m_anisotropicLevel == 0) ? s_defaultAnisotropyLevel : m_anisotropicLevel;
		glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<float>(anisotropyLevel));
	}

	nzSamplerFilter filterMode = (m_filterMode == nzSamplerFilter_Default) ? s_defaultFilterMode : m_filterMode;
	switch (filterMode)
	{
		case nzSamplerFilter_Bilinear:
			if (m_mipmaps)
				glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			else
				glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;

		case nzSamplerFilter_Nearest:
			if (m_mipmaps)
				glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			else
				glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			break;

		case nzSamplerFilter_Trilinear:
			if (m_mipmaps)
				glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			else
				glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Filtrage bilinéaire

			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;

		default:
			NazaraError("Texture filter not handled (0x" + NzString::Number(filterMode, 16) + ')');
			break;
	}

	GLenum wrapMode = NzOpenGL::SamplerWrapMode[(m_wrapMode == nzSamplerWrap_Default) ? s_defaultWrapMode : m_wrapMode];
	switch (type)
	{
		// Notez l'absence de "break" ici
		case nzImageType_3D:
			glTexParameteri(target, GL_TEXTURE_WRAP_R, wrapMode);
		case nzImageType_2D:
		case nzImageType_2D_Array:
		case nzImageType_Cubemap:
			glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapMode);
		case nzImageType_1D:
		case nzImageType_1D_Array:
			glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapMode);
			break;
	}
}

void NzTextureSampler::Bind(unsigned int unit) const
{
	static_assert(nzSamplerFilter_Max < 0x4, "Maximum sampler filter mode takes more than 2 bits");
	static_assert(nzSamplerWrap_Max < 0x4, "Maximum sampler wrap mode takes more than 2 bits");

	if (!m_samplerId)
		UpdateSamplerId();

	glBindSampler(unit, m_samplerId);
}

unsigned int NzTextureSampler::GetOpenGLID() const
{
	if (!m_samplerId)
		UpdateSamplerId();

	return m_samplerId;
}

void NzTextureSampler::UpdateSamplerId() const
{
	nzUInt32 key = (m_mipmaps          << 0) | // 1 bit
				   (m_filterMode       << 1) | // 2 bits
				   (m_wrapMode         << 3) | // 2 bits
				   (m_anisotropicLevel << 5);  // 8 bits

	auto it = s_samplers.find(key);
	if (it == s_samplers.end())
	{
		GLuint sampler;
		glGenSamplers(1, &sampler);

		if (s_useAnisotropicFilter)
		{
			nzUInt8 anisotropyLevel = (m_anisotropicLevel == 0) ? s_defaultAnisotropyLevel : m_anisotropicLevel;
			glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<float>(anisotropyLevel));
		}

		nzSamplerFilter filterMode = (m_filterMode == nzSamplerFilter_Default) ? s_defaultFilterMode : m_filterMode;
		switch (filterMode)
		{
			case nzSamplerFilter_Bilinear:
				glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, (m_mipmaps) ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
				glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;

			case nzSamplerFilter_Nearest:
				glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, (m_mipmaps) ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
				glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				break;

			case nzSamplerFilter_Trilinear:
				// Équivalent au filtrage bilinéaire si les mipmaps sont absentes
				glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, (m_mipmaps) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
				glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;

			default:
				NazaraError("Texture filter not handled (0x" + NzString::Number(filterMode, 16) + ')');
				break;
		}

		GLenum wrapMode = NzOpenGL::SamplerWrapMode[(m_wrapMode == nzSamplerWrap_Default) ? s_defaultWrapMode : m_wrapMode];
		glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, wrapMode);
		glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, wrapMode);
		glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, wrapMode);

		s_samplers[key] = sampler;
		m_samplerId = sampler;
	}
	else
		m_samplerId = it->second;
}

bool NzTextureSampler::UseMipmaps(bool mipmaps)
{
	if (m_mipmaps != mipmaps)
	{
		m_mipmaps = mipmaps;
		m_samplerId = 0;

		return true; // Renvoie true si la valeur a été changée (Donc s'il faut réappliquer le sampler)
	}
	else
		return false;
}

bool NzTextureSampler::Initialize()
{
	s_maxAnisotropyLevel = NzRenderer::GetMaxAnisotropyLevel();
	s_useAnisotropicFilter = NzOpenGL::IsSupported(nzOpenGLExtension_AnisotropicFilter);

	return true;
}

void NzTextureSampler::Uninitialize()
{
	if (!s_samplers.empty())
	{
		NzContext::EnsureContext();
		for (const std::pair<nzUInt32, GLuint>& pair : s_samplers)
			glDeleteSamplers(1, &pair.second);

		s_samplers.clear();
	}
}

nzUInt8 NzTextureSampler::s_defaultAnisotropyLevel = 1;
nzSamplerFilter NzTextureSampler::s_defaultFilterMode = nzSamplerFilter_Trilinear;
nzSamplerWrap NzTextureSampler::s_defaultWrapMode = nzSamplerWrap_Repeat;
