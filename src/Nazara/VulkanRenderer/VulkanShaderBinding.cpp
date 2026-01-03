// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/VulkanRenderer/VulkanShaderBinding.hpp>
#include <Nazara/VulkanRenderer/VulkanBuffer.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderPipelineLayout.hpp>
#include <Nazara/VulkanRenderer/VulkanTexture.hpp>
#include <Nazara/VulkanRenderer/VulkanTextureSampler.hpp>
#include <NazaraUtils/StackVector.hpp>
#include <NazaraUtils/TypeTraits.hpp>

namespace Nz
{
	void VulkanShaderBinding::Update(const Binding* bindings, std::size_t bindingCount)
	{
		std::size_t bufferBindingCount = 0;
		std::size_t imageBindingCount = 0;
		for (std::size_t i = 0; i < bindingCount; ++i)
		{
			const Binding& binding = bindings[i];

			std::visit(Overloaded{
				[](std::monostate) {},
				[&](const SampledTextureBinding&)
				{
					imageBindingCount++;
				},
				[&](const SampledTextureBindings& arg)
				{
					imageBindingCount += arg.arraySize;
				},
				[&](const StorageBufferBinding&)
				{
					bufferBindingCount++;
				},
				[&](const TextureBinding&)
				{
					imageBindingCount++;
				},
				[&](const UniformBufferBinding&)
				{
					bufferBindingCount++;
				},
			}, binding.content);
		}

		NazaraAssertMsg(bufferBindingCount < 128, "too many concurrent buffer update");
		NazaraAssertMsg(imageBindingCount < 128, "too many concurrent image binding update");
		NazaraAssertMsg(bindingCount < 128, "too many binding update");

		StackVector<VkDescriptorBufferInfo> bufferBinding = NazaraStackVector(VkDescriptorBufferInfo, bufferBindingCount);
		StackVector<VkDescriptorImageInfo> imageBinding = NazaraStackVector(VkDescriptorImageInfo, imageBindingCount);
		StackVector<VkWriteDescriptorSet> writeOps = NazaraStackVector(VkWriteDescriptorSet, bindingCount);

		for (std::size_t i = 0; i < bindingCount; ++i)
		{
			const Binding& binding = bindings[i];

			VkWriteDescriptorSet& writeOp = writeOps.emplace_back();
			writeOp.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeOp.dstSet = m_descriptorSet;
			writeOp.dstBinding = binding.bindingIndex;

			std::visit(Overloaded{
				[](std::monostate) {},
				[&](const SampledTextureBinding& arg)
				{
					const VulkanTexture* vkTexture = SafeCast<const VulkanTexture*>(arg.texture);
					const VulkanTextureSampler* vkSampler = SafeCast<const VulkanTextureSampler*>(arg.sampler);

					VkDescriptorImageInfo& imageInfo = imageBinding.emplace_back();
					imageInfo.imageLayout = ToVulkan(arg.textureLayout);
					imageInfo.imageView = (vkTexture) ? vkTexture->GetImageView() : VK_NULL_HANDLE;
					imageInfo.sampler = (vkSampler) ? vkSampler->GetSampler() : VK_NULL_HANDLE;

					writeOp.descriptorCount = 1;
					writeOp.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					writeOp.pImageInfo = &imageInfo;
				},
				[&](const SampledTextureBindings& arg)
				{
					for (UInt32 i = 0; i < arg.arraySize; ++i)
					{
						const VulkanTexture* vkTexture = SafeCast<const VulkanTexture*>(arg.textureBindings[i].texture);
						const VulkanTextureSampler* vkSampler = SafeCast<const VulkanTextureSampler*>(arg.textureBindings[i].sampler);

						VkDescriptorImageInfo& imageInfo = imageBinding.emplace_back();
						imageInfo.imageLayout = ToVulkan(arg.textureBindings[i].textureLayout);
						imageInfo.imageView = (vkTexture) ? vkTexture->GetImageView() : VK_NULL_HANDLE;
						imageInfo.sampler = (vkSampler) ? vkSampler->GetSampler() : VK_NULL_HANDLE;
					}

					writeOp.descriptorCount = arg.arraySize;
					writeOp.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					writeOp.pImageInfo = &imageBinding[imageBinding.size() - arg.arraySize];
				},
				[&](const StorageBufferBinding& arg)
				{
					VulkanBuffer* vkBuffer = SafeCast<VulkanBuffer*>(arg.buffer);

					VkDescriptorBufferInfo& bufferInfo = bufferBinding.emplace_back();
					bufferInfo.buffer = (vkBuffer) ? vkBuffer->GetBuffer() : VK_NULL_HANDLE;
					bufferInfo.offset = arg.offset;
					bufferInfo.range = arg.range;

					writeOp.descriptorCount = 1;
					writeOp.descriptorType = (arg.dynamic) ? VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
					writeOp.pBufferInfo = &bufferInfo;
				},
				[&](const TextureBinding& arg)
				{
					const VulkanTexture* vkTexture = SafeCast<const VulkanTexture*>(arg.texture);

					VkDescriptorImageInfo& imageInfo = imageBinding.emplace_back();
					imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
					imageInfo.imageView = (vkTexture) ? vkTexture->GetImageView() : VK_NULL_HANDLE;
					imageInfo.sampler = VK_NULL_HANDLE;

					writeOp.descriptorCount = 1;
					writeOp.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
					writeOp.pImageInfo = &imageInfo;
				},
				[&](const UniformBufferBinding& arg)
				{
					VulkanBuffer* vkBuffer = SafeCast<VulkanBuffer*>(arg.buffer);

					VkDescriptorBufferInfo& bufferInfo = bufferBinding.emplace_back();
					bufferInfo.buffer = (vkBuffer) ? vkBuffer->GetBuffer() : VK_NULL_HANDLE;
					bufferInfo.offset = arg.offset;
					bufferInfo.range = arg.range;

					writeOp.descriptorCount = 1;
					writeOp.descriptorType = (arg.dynamic) ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					writeOp.pBufferInfo = &bufferInfo;
				},
			}, binding.content);
		}

		m_owner.GetDevice()->vkUpdateDescriptorSets(*m_owner.GetDevice(), UInt32(writeOps.size()), writeOps.data(), 0U, nullptr);
	}

	void VulkanShaderBinding::UpdateDebugName(std::string_view name)
	{
		return m_owner.m_device->SetDebugName(VK_OBJECT_TYPE_DESCRIPTOR_SET, VulkanHandleToInteger(static_cast<VkDescriptorSet>(m_descriptorSet)), name);
	}

	void VulkanShaderBinding::Release()
	{
		m_owner.Release(*this);
	}
}
