// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanSingleFramebuffer.hpp>
#include <Nazara/Core/StackArray.hpp>
#include <Nazara/VulkanRenderer/VulkanRenderPass.hpp>
#include <Nazara/VulkanRenderer/VulkanTexture.hpp>
#include <stdexcept>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	VulkanSingleFramebuffer::VulkanSingleFramebuffer(Vk::Device& device, unsigned int width, unsigned int height, const std::shared_ptr<RenderPass>& renderPass, const std::vector<std::shared_ptr<Texture>>& attachments) :
	VulkanFramebuffer(Type::Single)
	{
		assert(renderPass);
		const VulkanRenderPass& vkRenderPass = static_cast<const VulkanRenderPass&>(*renderPass);

		StackArray<VkImageView> imageViews = NazaraStackArrayNoInit(VkImageView, attachments.size());
		for (std::size_t i = 0; i < attachments.size(); ++i)
		{
			assert(attachments[i]);

			const VulkanTexture& vkTexture = static_cast<const VulkanTexture&>(*attachments[i]);
			imageViews[i] = vkTexture.GetImageView();
		}

		VkFramebufferCreateInfo createInfo = {
			VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			nullptr,
			0,
			vkRenderPass.GetRenderPass(),
			UInt32(imageViews.size()),
			imageViews.data(),
			UInt32(width),
			UInt32(height),
			1
		};

		if (!m_framebuffer.Create(device, createInfo))
			throw std::runtime_error("failed to instantiate Vulkan framebuffer: " + TranslateVulkanError(m_framebuffer.GetLastErrorCode()));
	}
}
