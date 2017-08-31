// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/TextureSampler.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Context.hpp>
#include <Nazara/Renderer/OpenGL.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Renderer/Texture.hpp>
#include <unordered_map>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	namespace
	{
		std::unordered_map<UInt32, GLuint> s_samplers;
		UInt8 s_maxAnisotropyLevel;
		bool s_useAnisotropicFilter;
	}

	TextureSampler::TextureSampler() :
	m_filterMode(SamplerFilter_Default),
	m_wrapMode(SamplerWrap_Default),
	m_anisotropicLevel(0),
	m_mipmaps(true),
	m_samplerId(0)
	{
	}

	UInt8 TextureSampler::GetAnisotropicLevel() const
	{
		return m_anisotropicLevel;
	}

	SamplerFilter TextureSampler::GetFilterMode() const
	{
		return m_filterMode;
	}

	SamplerWrap TextureSampler::GetWrapMode() const
	{
		return m_wrapMode;
	}

	void TextureSampler::SetAnisotropyLevel(UInt8 anisotropyLevel)
	{
		#ifdef NAZARA_DEBUG
		if (!Renderer::IsInitialized())
		{
			NazaraError("Renderer module must be initialized");
			return;
		}
		#endif

		if (m_anisotropicLevel != anisotropyLevel)
		{
			if (anisotropyLevel > s_maxAnisotropyLevel)
			{
				NazaraWarning("Anisotropy level is over maximum anisotropy level (" + String::Number(anisotropyLevel) + " > " + String::Number(s_maxAnisotropyLevel) + ')');
				anisotropyLevel = s_maxAnisotropyLevel;
			}

			m_anisotropicLevel = anisotropyLevel;
			m_samplerId = 0;
		}
	}

	void TextureSampler::SetFilterMode(SamplerFilter filterMode)
	{
		if (m_filterMode != filterMode)
		{
			m_filterMode = filterMode;
			m_samplerId = 0;
		}
	}

	void TextureSampler::SetWrapMode(SamplerWrap wrapMode)
	{
		if (m_wrapMode != wrapMode)
		{
			m_wrapMode = wrapMode;
			m_samplerId = 0;
		}
	}

	UInt8 TextureSampler::GetDefaultAnisotropicLevel()
	{
		return s_defaultAnisotropyLevel;
	}

	SamplerFilter TextureSampler::GetDefaultFilterMode()
	{
		return s_defaultFilterMode;
	}

	SamplerWrap TextureSampler::GetDefaultWrapMode()
	{
		return s_defaultWrapMode;
	}

	void TextureSampler::SetDefaultAnisotropyLevel(UInt8 anisotropyLevel)
	{
		#if NAZARA_RENDERER_SAFE
		if (anisotropyLevel == 0)
		{
			NazaraError("Default anisotropy level mode cannot be set to default value (0)");
			return;
		}
		#endif

		#ifdef NAZARA_DEBUG
		if (!Renderer::IsInitialized())
		{
			NazaraError("Renderer module must be initialized");
			return;
		}
		#endif

		if (anisotropyLevel > s_maxAnisotropyLevel)
		{
			NazaraWarning("Anisotropy level is over maximum anisotropy level (" + String::Number(anisotropyLevel) + " > " + String::Number(s_maxAnisotropyLevel));
			anisotropyLevel = s_maxAnisotropyLevel;
		}

		s_defaultAnisotropyLevel = anisotropyLevel;

		if (s_useAnisotropicFilter)
		{
			for (const std::pair<const UInt32, GLuint>& pair : s_samplers)
			{
				if (((pair.first >> 5) & 0xFF) == 0)
					glSamplerParameterf(pair.second, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<float>(anisotropyLevel));
			}
		}
	}

	void TextureSampler::SetDefaultFilterMode(SamplerFilter filterMode)
	{
		#if NAZARA_RENDERER_SAFE
		if (filterMode == SamplerFilter_Default)
		{
			NazaraError("Default filter mode cannot be set to default enum value (SamplerFilter_Default)");
			return;
		}
		#endif

		s_defaultFilterMode = filterMode;

		for (const std::pair<const UInt32, GLuint>& pair : s_samplers)
		{
			if (((pair.first >> 1) & 0x3) == SamplerFilter_Default)
			{
				bool mipmaps = pair.first & 0x1;
				switch (filterMode)
				{
					case SamplerFilter_Bilinear:
						if (mipmaps)
							glSamplerParameteri(pair.second, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
						else
							glSamplerParameteri(pair.second, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

						glSamplerParameteri(pair.second, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						break;

					case SamplerFilter_Nearest:
						if (mipmaps)
							glSamplerParameteri(pair.second, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
						else
							glSamplerParameteri(pair.second, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

						glSamplerParameteri(pair.second, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
						break;

					case SamplerFilter_Trilinear:
						if (mipmaps)
							glSamplerParameteri(pair.second, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
						else
							glSamplerParameteri(pair.second, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Filtrage bilinéaire

						glSamplerParameteri(pair.second, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						break;

					default:
						NazaraError("Texture filter not handled (0x" + String::Number(filterMode, 16) + ')');
						break;
				}
			}
		}
	}

	void TextureSampler::SetDefaultWrapMode(SamplerWrap wrapMode)
	{
		#if NAZARA_RENDERER_SAFE
		if (wrapMode == SamplerWrap_Default)
		{
			NazaraError("Default wrap mode cannot be set to default enum value (SamplerWrap_Default)");
			return;
		}
		#endif

		s_defaultWrapMode = wrapMode;

		GLenum wrapEnum = OpenGL::SamplerWrapMode[wrapMode];
		for (const std::pair<const UInt32, GLuint>& pair : s_samplers)
		{
			if (((pair.first >> 3) & 0x3) == SamplerWrap_Default)
			{
				glSamplerParameteri(pair.second, GL_TEXTURE_WRAP_R, wrapEnum);
				glSamplerParameteri(pair.second, GL_TEXTURE_WRAP_T, wrapEnum);
				glSamplerParameteri(pair.second, GL_TEXTURE_WRAP_S, wrapEnum);
			}
		}
	}

	void TextureSampler::Apply(const Texture* texture) const
	{
		ImageType type = texture->GetType();
		GLenum target = OpenGL::TextureTarget[type];

		OpenGL::BindTexture(type, texture->GetOpenGLID());

		if (s_useAnisotropicFilter)
		{
			UInt8 anisotropyLevel = (m_anisotropicLevel == 0) ? s_defaultAnisotropyLevel : m_anisotropicLevel;
			glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<float>(anisotropyLevel));
		}

		SamplerFilter filterMode = (m_filterMode == SamplerFilter_Default) ? s_defaultFilterMode : m_filterMode;
		switch (filterMode)
		{
			case SamplerFilter_Bilinear:
				if (m_mipmaps)
					glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
				else
					glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

				glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;

			case SamplerFilter_Nearest:
				if (m_mipmaps)
					glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
				else
					glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

				glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				break;

			case SamplerFilter_Trilinear:
				if (m_mipmaps)
					glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				else
					glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Filtrage bilinéaire

				glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				break;

			default:
				NazaraError("Texture filter not handled (0x" + String::Number(filterMode, 16) + ')');
				break;
		}

		GLenum wrapMode = OpenGL::SamplerWrapMode[(m_wrapMode == SamplerWrap_Default) ? s_defaultWrapMode : m_wrapMode];
		switch (type)
		{
			// Notez l'absence de "break" ici
			case ImageType_3D:
				glTexParameteri(target, GL_TEXTURE_WRAP_R, wrapMode);
			case ImageType_2D:
			case ImageType_2D_Array:
			case ImageType_Cubemap:
				glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapMode);
			case ImageType_1D:
			case ImageType_1D_Array:
				glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapMode);
				break;
		}
	}

	void TextureSampler::Bind(unsigned int unit) const
	{
		static_assert(SamplerFilter_Max < 0x4, "Maximum sampler filter mode takes more than 2 bits");
		static_assert(SamplerWrap_Max < 0x4, "Maximum sampler wrap mode takes more than 2 bits");

		if (!m_samplerId)
			UpdateSamplerId();

		OpenGL::BindSampler(unit, m_samplerId);
	}

	unsigned int TextureSampler::GetOpenGLID() const
	{
		if (!m_samplerId)
			UpdateSamplerId();

		return m_samplerId;
	}

	void TextureSampler::UpdateSamplerId() const
	{
		UInt32 key = (((m_mipmaps) ? 1 : 0) << 0) | // 1 bit
					   (m_filterMode          << 1) | // 2 bits
					   (m_wrapMode            << 3) | // 2 bits
					   (m_anisotropicLevel    << 5);  // 8 bits

		auto it = s_samplers.find(key);
		if (it == s_samplers.end())
		{
			GLuint sampler;
			glGenSamplers(1, &sampler);

			if (s_useAnisotropicFilter)
			{
				UInt8 anisotropyLevel = (m_anisotropicLevel == 0) ? s_defaultAnisotropyLevel : m_anisotropicLevel;
				glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<float>(anisotropyLevel));
			}

			SamplerFilter filterMode = (m_filterMode == SamplerFilter_Default) ? s_defaultFilterMode : m_filterMode;
			switch (filterMode)
			{
				case SamplerFilter_Bilinear:
					glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, (m_mipmaps) ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
					glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					break;

				case SamplerFilter_Nearest:
					glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, (m_mipmaps) ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
					glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					break;

				case SamplerFilter_Trilinear:
					// Équivalent au filtrage bilinéaire si les mipmaps sont absentes
					glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, (m_mipmaps) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
					glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					break;

				default:
					NazaraError("Texture filter not handled (0x" + String::Number(filterMode, 16) + ')');
					break;
			}

			GLenum wrapMode = OpenGL::SamplerWrapMode[(m_wrapMode == SamplerWrap_Default) ? s_defaultWrapMode : m_wrapMode];
			glSamplerParameteri(sampler, GL_TEXTURE_WRAP_R, wrapMode);
			glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, wrapMode);
			glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, wrapMode);

			s_samplers[key] = sampler;
			m_samplerId = sampler;
		}
		else
			m_samplerId = it->second;
	}

	bool TextureSampler::UseMipmaps(bool mipmaps)
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

	bool TextureSampler::Initialize()
	{
		s_maxAnisotropyLevel = Renderer::GetMaxAnisotropyLevel();
		s_useAnisotropicFilter = OpenGL::IsSupported(OpenGLExtension_AnisotropicFilter);

		return true;
	}

	void TextureSampler::Uninitialize()
	{
		if (!s_samplers.empty())
		{
			Context::EnsureContext();
			for (const std::pair<const UInt32, GLuint>& pair : s_samplers)
				OpenGL::DeleteSampler(pair.second);

			s_samplers.clear();
		}
	}

	UInt8 TextureSampler::s_defaultAnisotropyLevel = 1;
	SamplerFilter TextureSampler::s_defaultFilterMode = SamplerFilter_Trilinear;
	SamplerWrap TextureSampler::s_defaultWrapMode = SamplerWrap_Repeat;
}
