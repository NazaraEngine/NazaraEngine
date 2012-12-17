// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TEXTURESAMPLER_HPP
#define NAZARA_TEXTURESAMPLER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/Enums.hpp>

class NzTexture;

class NAZARA_API NzTextureSampler
{
	friend class NzRenderer;

	public:
		NzTextureSampler();
		NzTextureSampler(const NzTextureSampler& sampler) = default;

		nzUInt8 GetAnisotropicLevel() const;
		nzSamplerFilter GetFilterMode() const;
		nzSamplerWrap GetWrapMode() const;

		void SetAnisotropyLevel(nzUInt8 anisotropyLevel);
		void SetFilterMode(nzSamplerFilter filterMode);
		void SetWrapMode(nzSamplerWrap wrapMode);

		NzTextureSampler& operator=(const NzTextureSampler& sampler) = default;

		static nzUInt8 GetDefaultAnisotropicLevel();
		static nzSamplerFilter GetDefaultFilterMode();
		static nzSamplerWrap GetDefaultWrapMode();

		static void SetDefaultAnisotropyLevel(nzUInt8 anisotropyLevel);
		static void SetDefaultFilterMode(nzSamplerFilter filterMode);
		static void SetDefaultWrapMode(nzSamplerWrap wrapMode);

	private:
		void Apply(const NzTexture* texture);
		void Bind(unsigned int unit);
		bool UseMipmaps(bool mipmaps);

		static bool Initialize();
		static void Uninitialize();

		nzSamplerFilter m_filterMode;
		nzSamplerWrap m_wrapMode;
		nzUInt8 m_anisotropicLevel;
		bool m_mipmaps;
		mutable unsigned int m_samplerId;

		static nzSamplerFilter s_defaultFilterMode;
		static nzSamplerWrap s_defaultWrapMode;
		static nzUInt8 s_defaultAnisotropyLevel;
};

#endif // NAZARA_TEXTURESAMPLER_HPP
