// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANTEXTURESAMPLER_HPP
#define NAZARA_VULKANRENDERER_VULKANTEXTURESAMPLER_HPP

#include <Nazara/Prerequisites.hpp>
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

			VulkanTextureSampler& operator=(const VulkanTextureSampler&) = delete;
			VulkanTextureSampler& operator=(VulkanTextureSampler&&) = delete;

		private:
			Vk::Sampler m_sampler;
	};
}

#include <Nazara/VulkanRenderer/VulkanTextureSampler.inl>

#endif // NAZARA_VULKANRENDERER_VULKANTEXTURESAMPLER_HPP
