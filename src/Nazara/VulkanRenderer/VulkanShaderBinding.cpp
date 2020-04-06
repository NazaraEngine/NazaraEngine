// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanShaderBinding.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/StackVector.hpp>
#include <Nazara/VulkanRenderer/VulkanBuffer.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderPipelineLayout.hpp>
#include <Nazara/VulkanRenderer/VulkanTexture.hpp>
#include <Nazara/VulkanRenderer/VulkanTextureSampler.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	void VulkanShaderBinding::Update(std::initializer_list<Binding> bindings)
	{
		StackVector<VkDescriptorBufferInfo> bufferBinding = NazaraStackVector(VkDescriptorBufferInfo, bindings.size());
		StackVector<VkDescriptorImageInfo> imageBinding = NazaraStackVector(VkDescriptorImageInfo, bindings.size());
		StackVector<VkWriteDescriptorSet> writeOps = NazaraStackVector(VkWriteDescriptorSet, bindings.size());

		for (const Binding& binding : bindings)
		{
			VkWriteDescriptorSet& writeOp = writeOps.emplace_back();
			writeOp.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeOp.dstSet = m_descriptorSet;
			writeOp.dstBinding = UInt32(binding.bindingIndex);

			std::visit([&](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;

				if constexpr (std::is_same_v<T, TextureBinding>)
				{
					VulkanTexture& vkTexture = *static_cast<VulkanTexture*>(arg.texture);
					VulkanTextureSampler& vkSampler = *static_cast<VulkanTextureSampler*>(arg.sampler);

					VkDescriptorImageInfo& imageInfo = imageBinding.emplace_back();
					imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					imageInfo.imageView = vkTexture.GetImageView();
					imageInfo.sampler = vkSampler.GetSampler();

					writeOp.descriptorCount = 1;
					writeOp.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

					writeOp.pImageInfo = &imageInfo;
				}
				else if constexpr (std::is_same_v<T, UniformBufferBinding>)
				{
					VulkanBuffer& vkBuffer = *static_cast<VulkanBuffer*>(arg.buffer);

					VkDescriptorBufferInfo& bufferInfo = bufferBinding.emplace_back();
					bufferInfo.buffer = vkBuffer.GetBuffer();
					bufferInfo.offset = arg.offset;
					bufferInfo.range = arg.range;

					writeOp.descriptorCount = 1;
					writeOp.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

					writeOp.pBufferInfo = &bufferInfo;
				}
				else
					static_assert(AlwaysFalse<T>::value, "non-exhaustive visitor");

			}, binding.content);
		}

		m_owner.GetDevice()->vkUpdateDescriptorSets(*m_owner.GetDevice(), UInt32(writeOps.size()), writeOps.data(), 0U, nullptr);
	}

	void VulkanShaderBinding::Release()
	{
		m_owner.Release(*this);
	}
}
