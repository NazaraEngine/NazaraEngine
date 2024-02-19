// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/VulkanRenderer/Utils.hpp>
#include <NazaraUtils/Hash.hpp>
#include <stdexcept>

namespace Nz
{
	inline std::size_t VulkanDescriptorSetLayoutBindingHasher::operator()(const VulkanDescriptorSetLayoutInfo& layoutInfo) const
	{
		std::size_t hash = 0;
		HashCombine(hash, layoutInfo.createFlags);
		for (const auto& binding : layoutInfo.bindings)
		{
			HashCombine(hash, binding.binding);
			HashCombine(hash, binding.descriptorCount);
			HashCombine(hash, binding.descriptorType);
			HashCombine(hash, binding.pImmutableSamplers);
			HashCombine(hash, binding.stageFlags);
		}

		return hash;
	}

	inline bool VulkanDescriptorSetLayoutBindingEqual::operator()(const VulkanDescriptorSetLayoutInfo& lhs, const VulkanDescriptorSetLayoutInfo& rhs) const
	{
		if (lhs.createFlags != rhs.createFlags)
			return false;

		if (lhs.bindings.size() != rhs.bindings.size())
			return false;

		for (std::size_t i = 0; i < lhs.bindings.size(); ++i)
		{
			const auto& lhsBinding = lhs.bindings[i];
			const auto& bindingRhs = rhs.bindings[i];

			if (lhsBinding.binding != bindingRhs.binding)
				return false;

			if (lhsBinding.descriptorCount != bindingRhs.descriptorCount)
				return false;

			if (lhsBinding.descriptorType != bindingRhs.descriptorType)
				return false;

			if (lhsBinding.pImmutableSamplers != bindingRhs.pImmutableSamplers)
				return false;

			if (lhsBinding.stageFlags != bindingRhs.stageFlags)
				return false;
		}

		return true;
	}

	inline VulkanDescriptorSetLayoutCache::VulkanDescriptorSetLayoutCache(Vk::Device& device) :
	m_device(device)
	{
	}

	inline void VulkanDescriptorSetLayoutCache::Clear()
	{
		m_cache.clear();
	}

	inline const Vk::DescriptorSetLayout& VulkanDescriptorSetLayoutCache::Get(const VulkanDescriptorSetLayoutInfo& layoutInfo) const
	{
		auto it = m_cache.find(layoutInfo);
		if (it != m_cache.end())
			return it->second;

		Vk::DescriptorSetLayout setLayout;
		if (!setLayout.Create(m_device, UInt32(layoutInfo.bindings.size()), layoutInfo.bindings.data(), layoutInfo.createFlags))
			throw std::runtime_error("failed to create descriptor set layout: " + TranslateVulkanError(setLayout.GetLastErrorCode()));

		return m_cache.emplace(layoutInfo, std::move(setLayout)).first->second;
	}
}

