// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_TEXTURESAMPLERCACHE_HPP
#define NAZARA_GRAPHICS_TEXTURESAMPLERCACHE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>
#include <unordered_map>

namespace Nz
{
	class RenderDevice;

	class NAZARA_GRAPHICS_API TextureSamplerCache
	{
		public:
			inline TextureSamplerCache(std::shared_ptr<RenderDevice> device);
			TextureSamplerCache(const TextureSamplerCache&) = delete;
			TextureSamplerCache(TextureSamplerCache&&) = delete;
			~TextureSamplerCache() = default;

			const std::shared_ptr<TextureSampler>& Get(const TextureSamplerInfo& info);

			TextureSamplerCache& operator=(const TextureSamplerCache&) = delete;
			TextureSamplerCache& operator=(TextureSamplerCache&&) = delete;

		private:
			std::shared_ptr<RenderDevice> m_device;
			std::unordered_map<TextureSamplerInfo, std::shared_ptr<TextureSampler>> m_samplers;
	};
}

#include <Nazara/Graphics/TextureSamplerCache.inl>

#endif // NAZARA_GRAPHICS_TEXTURESAMPLERCACHE_HPP
