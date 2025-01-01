// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_TEXTURESAMPLER_HPP
#define NAZARA_RENDERER_TEXTURESAMPLER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Renderer/Export.hpp>
#include <functional>
#include <string_view>

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
		bool depthCompare = false;
		RendererComparison depthComparison = RendererComparison::LessOrEqual;

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

			virtual void UpdateDebugName(std::string_view name) = 0;

			TextureSampler& operator=(const TextureSampler&) = delete;
			TextureSampler& operator=(TextureSampler&&) = delete;

		protected:
			static void ValidateSamplerInfo(const RenderDevice& device, TextureSamplerInfo& samplerInfo);
	};
}

template<>
struct std::hash<Nz::TextureSamplerInfo>;

#include <Nazara/Renderer/TextureSampler.inl>

#endif // NAZARA_RENDERER_TEXTURESAMPLER_HPP
