// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/VulkanRenderer/VulkanTextureFramebuffer.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderPass.hpp>
#include <Nazara/VulkanRenderer/VulkanTexture.hpp>
#include <NazaraUtils/StackArray.hpp>
#include <stdexcept>

namespace Nz
{
	VulkanTextureFramebuffer::VulkanTextureFramebuffer(Vk::Device& device, UInt32 width, UInt32 height, const std::shared_ptr<RenderPass>& renderPass, const std::vector<std::shared_ptr<Texture>>& attachments) :
	VulkanFramebuffer(FramebufferType::Texture)
	{
		assert(renderPass);
		const VulkanRenderPass& vkRenderPass = SafeCast<const VulkanRenderPass&>(*renderPass);

		StackArray<VkImageView> imageViews = NazaraStackArrayNoInit(VkImageView, attachments.size());
		for (std::size_t i = 0; i < attachments.size(); ++i)
		{
			assert(attachments[i]);

			const VulkanTexture& vkTexture = SafeCast<const VulkanTexture&>(*attachments[i]);
			imageViews[i] = vkTexture.GetImageView();
		}

		VkFramebufferCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.renderPass = vkRenderPass.GetRenderPass(),
			.attachmentCount = UInt32(imageViews.size()),
			.pAttachments = imageViews.data(),
			.width = UInt32(width),
			.height = UInt32(height),
			.layers = 1
		};

		if (!m_framebuffer.Create(device, createInfo))
			throw std::runtime_error("failed to instantiate Vulkan framebuffer: " + TranslateVulkanError(m_framebuffer.GetLastErrorCode()));
	}
}
