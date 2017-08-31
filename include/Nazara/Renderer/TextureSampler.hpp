// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TEXTURESAMPLER_HPP
#define NAZARA_TEXTURESAMPLER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Utility/Enums.hpp>

namespace Nz
{
	class Texture;

	class NAZARA_RENDERER_API TextureSampler
	{
		friend class Renderer;

		public:
			TextureSampler();
			TextureSampler(const TextureSampler& sampler) = default;

			UInt8 GetAnisotropicLevel() const;
			SamplerFilter GetFilterMode() const;
			SamplerWrap GetWrapMode() const;

			void SetAnisotropyLevel(UInt8 anisotropyLevel);
			void SetFilterMode(SamplerFilter filterMode);
			void SetWrapMode(SamplerWrap wrapMode);

			TextureSampler& operator=(const TextureSampler& sampler) = default;

			static UInt8 GetDefaultAnisotropicLevel();
			static SamplerFilter GetDefaultFilterMode();
			static SamplerWrap GetDefaultWrapMode();

			static void SetDefaultAnisotropyLevel(UInt8 anisotropyLevel);
			static void SetDefaultFilterMode(SamplerFilter filterMode);
			static void SetDefaultWrapMode(SamplerWrap wrapMode);

		private:
			void Apply(const Texture* texture) const;
			void Bind(unsigned int unit) const;
			unsigned int GetOpenGLID() const;
			void UpdateSamplerId() const;
			bool UseMipmaps(bool mipmaps);

			static bool Initialize();
			static void Uninitialize();

			SamplerFilter m_filterMode;
			SamplerWrap m_wrapMode;
			UInt8 m_anisotropicLevel;
			bool m_mipmaps;
			mutable unsigned int m_samplerId;

			static SamplerFilter s_defaultFilterMode;
			static SamplerWrap s_defaultWrapMode;
			static UInt8 s_defaultAnisotropyLevel;
	};
}

#endif // NAZARA_TEXTURESAMPLER_HPP
