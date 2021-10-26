// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_TEXTURE_SAMPLER_HPP
#define NAZARA_TEXTURE_SAMPLER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <functional>

namespace Nz
{
	struct TextureSamplerInfo
	{
		float anisotropyLevel = 0.f;
		SamplerFilter magFilter = SamplerFilter::Linear;
		SamplerFilter minFilter = SamplerFilter::Linear;
		SamplerMipmapMode mipmapMode = SamplerMipmapMode::Linear;
		SamplerWrap wrapModeU = SamplerWrap::Clamp;
		SamplerWrap wrapModeV = SamplerWrap::Clamp;
		SamplerWrap wrapModeW = SamplerWrap::Clamp;

		inline bool operator==(const TextureSamplerInfo& samplerInfo) const;
		inline bool operator!=(const TextureSamplerInfo& samplerInfo) const;
	};

	class RenderDevice;

	class NAZARA_RENDERER_API TextureSampler
	{
		public:
			TextureSampler() = default;
			TextureSampler(const TextureSampler&) = delete;
			TextureSampler(TextureSampler&&) = delete;
			virtual ~TextureSampler();

			TextureSampler& operator=(const TextureSampler&) = delete;
			TextureSampler& operator=(TextureSampler&&) = delete;

		protected:
			static void ValidateSamplerInfo(const RenderDevice& device, TextureSamplerInfo& samplerInfo);
	};
}

template<>
struct std::hash<Nz::TextureSamplerInfo>;

#include <Nazara/Renderer/TextureSampler.inl>

#endif // NAZARA_TEXTURE_HPP
