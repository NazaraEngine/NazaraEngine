// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANTEXTURESAMPLER_HPP
#define NAZARA_VULKANRENDERER_VULKANTEXTURESAMPLER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/TextureSampler.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Sampler.hpp>

namespace Nz
{
	class VulkanDevice;

	class NAZARA_VULKANRENDERER_API VulkanTextureSampler : public TextureSampler
	{
		public:
			VulkanTextureSampler(VulkanDevice& device, TextureSamplerInfo samplerInfo);
			VulkanTextureSampler(const VulkanTextureSampler&) = delete;
			VulkanTextureSampler(VulkanTextureSampler&&) = delete;
			~VulkanTextureSampler() = default;

			inline VkSampler GetSampler() const;

			void UpdateDebugName(std::string_view name) override;

			VulkanTextureSampler& operator=(const VulkanTextureSampler&) = delete;
			VulkanTextureSampler& operator=(VulkanTextureSampler&&) = delete;

		private:
			Vk::Sampler m_sampler;
	};
}

#include <Nazara/VulkanRenderer/VulkanTextureSampler.inl>

#endif // NAZARA_VULKANRENDERER_VULKANTEXTURESAMPLER_HPP
