// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TEXTURE_SAMPLER_HPP
#define NAZARA_TEXTURE_SAMPLER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Utility/Enums.hpp>

namespace Nz
{
	struct TextureSamplerInfo
	{
		float anisotropyLevel = 0.f;
		SamplerFilter magFilter = SamplerFilter_Linear;
		SamplerFilter minFilter = SamplerFilter_Linear;
		SamplerMipmapMode mipmapMode = SamplerMipmapMode_Linear;
		SamplerWrap wrapModeU = SamplerWrap_Clamp;
		SamplerWrap wrapModeV = SamplerWrap_Clamp;
		SamplerWrap wrapModeW = SamplerWrap_Clamp;

		inline bool operator==(const TextureSamplerInfo& samplerInfo) const;
		inline bool operator!=(const TextureSamplerInfo& samplerInfo) const;
	};

	class NAZARA_RENDERER_API TextureSampler
	{
		public:
			TextureSampler() = default;
			TextureSampler(const TextureSampler&) = delete;
			TextureSampler(TextureSampler&&) = delete;
			virtual ~TextureSampler();

			TextureSampler& operator=(const TextureSampler&) = delete;
			TextureSampler& operator=(TextureSampler&&) = delete;
	};
}

template<>
struct std::hash<Nz::TextureSamplerInfo>;

#include <Nazara/Renderer/TextureSampler.inl>

#endif // NAZARA_TEXTURE_HPP
