// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANDESCRIPTORSETLAYOUTCACHE_HPP
#define NAZARA_VULKANRENDERER_VULKANDESCRIPTORSETLAYOUTCACHE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DescriptorSetLayout.hpp>
#include <unordered_map>

namespace Nz
{
	namespace Vk
	{
		class Device;
	}

	struct VulkanDescriptorSetLayoutInfo
	{
		VkDescriptorSetLayoutCreateFlags createFlags = 0;
		std::vector<VkDescriptorSetLayoutBinding> bindings;
	};

	struct VulkanDescriptorSetLayoutBindingHasher
	{
		inline std::size_t operator()(const VulkanDescriptorSetLayoutInfo& layoutInfo) const;
	};

	struct VulkanDescriptorSetLayoutBindingEqual
	{
		inline bool operator()(const VulkanDescriptorSetLayoutInfo& lhs, const VulkanDescriptorSetLayoutInfo& rhs) const;
	};

	class VulkanDescriptorSetLayoutCache
	{
		public:
			inline VulkanDescriptorSetLayoutCache(Vk::Device& device);
			~VulkanDescriptorSetLayoutCache() = default;

			inline void Clear();

			inline const Vk::DescriptorSetLayout& Get(const VulkanDescriptorSetLayoutInfo& layoutInfo) const;

		private:
			using Cache = std::unordered_map<VulkanDescriptorSetLayoutInfo, Vk::DescriptorSetLayout, VulkanDescriptorSetLayoutBindingHasher, VulkanDescriptorSetLayoutBindingEqual>;

			mutable Cache m_cache;
			Vk::Device& m_device;
	};
}

#include <Nazara/VulkanRenderer/VulkanDescriptorSetLayoutCache.inl>

#endif // NAZARA_VULKANRENDERER_VULKANDESCRIPTORSETLAYOUTCACHE_HPP
