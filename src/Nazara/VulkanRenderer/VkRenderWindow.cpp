// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VkRenderWindow.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <Nazara/VulkanRenderer/Vulkan.hpp>
#include <Nazara/VulkanRenderer/VulkanSurface.hpp>
#include <array>
#include <stdexcept>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	VkRenderWindow::VkRenderWindow() :
	m_physicalDevice(nullptr),
	m_depthStencilFormat(VK_FORMAT_MAX_ENUM)
	{
	}

	VkRenderWindow::~VkRenderWindow()
	{
		m_device->WaitForIdle();
		m_frameBuffers.clear();
		m_renderPass.Destroy();

		m_swapchain.Destroy();
	}

	bool VkRenderWindow::Acquire(UInt32* imageIndex) const
	{
		if (!m_swapchain.AcquireNextImage(std::numeric_limits<UInt64>::max(), m_imageReadySemaphore, VK_NULL_HANDLE, imageIndex))
		{
			NazaraError("Failed to acquire next image");
			return false;
		}

		return true;
	}

	void VkRenderWindow::BuildPreRenderCommands(UInt32 imageIndex, Vk::CommandBuffer& commandBuffer)
	{
		//commandBuffer.SetImageLayout(m_swapchain.GetBuffer(imageIndex).image, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

		// Temporary
		if (m_depthStencilFormat != VK_FORMAT_MAX_ENUM)
		{
			VkImageSubresourceRange imageRange = {
						VK_IMAGE_ASPECT_DEPTH_BIT, // VkImageAspectFlags                     aspectMask
						0,                         // uint32_t                               baseMipLevel
						1,                         // uint32_t                               levelCount
						0,                         // uint32_t                               baseArrayLayer
						1                          // uint32_t                               layerCount
			};

			commandBuffer.SetImageLayout(m_depthBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, imageRange);
		}
	}

	void VkRenderWindow::BuildPostRenderCommands(UInt32 imageIndex, Vk::CommandBuffer& commandBuffer)
	{
		//commandBuffer.SetImageLayout(m_swapchain.GetBuffer(imageIndex).image, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	}

	bool VkRenderWindow::Create(RenderSurface* surface, const Vector2ui& size, const RenderWindowParameters& parameters)
	{
		m_physicalDevice = Vulkan::GetPhysicalDevices()[0].device;

		Vk::Surface& vulkanSurface = static_cast<VulkanSurface*>(surface)->GetSurface();

		m_device = Vulkan::SelectDevice(m_physicalDevice, vulkanSurface, &m_presentableFamilyQueue);
		if (!m_device)
		{
			NazaraError("Failed to get compatible Vulkan device");
			return false;
		}

		m_presentQueue = m_device->GetQueue(m_presentableFamilyQueue, 0);

		std::vector<VkSurfaceFormatKHR> surfaceFormats;
		if (!vulkanSurface.GetFormats(m_physicalDevice, &surfaceFormats))
		{
			NazaraError("Failed to query supported surface formats");
			return false;
		}

		if (surfaceFormats.size() == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
			m_colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
		else
			m_colorFormat = surfaceFormats[0].format;

		m_colorSpace = surfaceFormats[0].colorSpace;

		if (!parameters.depthFormats.empty())
		{
			const Vk::PhysicalDevice& deviceInfo = Vulkan::GetPhysicalDeviceInfo(m_physicalDevice);

			for (PixelFormatType format : parameters.depthFormats)
			{
				switch (format)
				{
					case PixelFormatType_Depth16:
						m_depthStencilFormat = VK_FORMAT_D16_UNORM;
						break;

					case PixelFormatType_Depth24:
					case PixelFormatType_Depth24Stencil8:
						m_depthStencilFormat = VK_FORMAT_D24_UNORM_S8_UINT;
						break;

					case PixelFormatType_Depth32:
						m_depthStencilFormat = VK_FORMAT_D32_SFLOAT;
						break;

					case PixelFormatType_Stencil1:
					case PixelFormatType_Stencil4:
					case PixelFormatType_Stencil8:
						m_depthStencilFormat = VK_FORMAT_S8_UINT;
						break;

					case PixelFormatType_Stencil16:
						m_depthStencilFormat = VK_FORMAT_MAX_ENUM;
						break;

					default:
					{
						PixelFormatContent formatContent = PixelFormat::GetContent(format);
						if (formatContent != PixelFormatContent_DepthStencil && formatContent != PixelFormatContent_Stencil)
							NazaraWarning("Invalid format " + PixelFormat::GetName(format) + " for depth-stencil attachment");

						m_depthStencilFormat = VK_FORMAT_MAX_ENUM;
						break;
					}
				}

				if (m_depthStencilFormat != VK_FORMAT_MAX_ENUM)
				{
					VkFormatProperties formatProperties = m_device->GetInstance().GetPhysicalDeviceFormatProperties(m_physicalDevice, m_depthStencilFormat);
					if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
						break; //< Found it

					m_depthStencilFormat = VK_FORMAT_MAX_ENUM;
				}
			}
		}

		if (!SetupSwapchain(vulkanSurface, size))
		{
			NazaraError("Failed to create swapchain");
			return false;
		}

		if (m_depthStencilFormat != VK_FORMAT_MAX_ENUM && !SetupDepthBuffer(size))
		{
			NazaraError("Failed to create depth buffer");
			return false;
		}

		if (!SetupRenderPass(size))
		{
			NazaraError("Failed to create render pass");
			return false;
		}

		UInt32 imageCount = m_swapchain.GetBufferCount();

		// Framebuffers
		m_frameBuffers.resize(imageCount);
		for (UInt32 i = 0; i < imageCount; ++i)
		{
			std::array<VkImageView, 2> attachments = {m_swapchain.GetBuffer(i).view, m_depthBufferView};

			VkFramebufferCreateInfo frameBufferCreate = {
				VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,    // VkStructureType             sType;
				nullptr,                                      // const void*                 pNext;
				0,                                            // VkFramebufferCreateFlags    flags;
				m_renderPass,                                 // VkRenderPass                renderPass;
				(attachments[1] != VK_NULL_HANDLE) ? 2U : 1U, // uint32_t                    attachmentCount;
				attachments.data(),                           // const VkImageView*          pAttachments;
				size.x,                                       // uint32_t                    width;
				size.y,                                       // uint32_t                    height;
				1U                                            // uint32_t                    layers;
			};

			if (!m_frameBuffers[i].Create(m_device, frameBufferCreate))
			{
				NazaraError("Failed to create framebuffer for image #" + String::Number(i));
				return false;
			}
		}

		m_imageReadySemaphore.Create(m_device);

		m_clock.Restart();

		return true;
	}

	bool VkRenderWindow::SetupDepthBuffer(const Vector2ui& size)
	{
		VkImageCreateInfo imageCreateInfo = {
			VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,                                           // VkStructureType          sType;
			nullptr,                                                                       // const void*              pNext;
			0U,                                                                            // VkImageCreateFlags       flags;
			VK_IMAGE_TYPE_2D,                                                              // VkImageType              imageType;
			m_depthStencilFormat,                                                          // VkFormat                 format;
			{size.x, size.y, 1U},                                                 // VkExtent3D               extent;
			1U,                                                                            // uint32_t                 mipLevels;
			1U,                                                                            // uint32_t                 arrayLayers;
			VK_SAMPLE_COUNT_1_BIT,                                                         // VkSampleCountFlagBits    samples;
			VK_IMAGE_TILING_OPTIMAL,                                                       // VkImageTiling            tiling;
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, // VkImageUsageFlags        usage;
			VK_SHARING_MODE_EXCLUSIVE,                                                     // VkSharingMode            sharingMode;
			0U,                                                                            // uint32_t                 queueFamilyIndexCount;
			nullptr,                                                                       // const uint32_t*          pQueueFamilyIndices;
			VK_IMAGE_LAYOUT_UNDEFINED,                                                     // VkImageLayout            initialLayout;
		};

		if (!m_depthBuffer.Create(m_device, imageCreateInfo))
		{
			NazaraError("Failed to create depth buffer");
			return false;
		}

		VkMemoryRequirements memoryReq = m_depthBuffer.GetMemoryRequirements();
		if (!m_depthBufferMemory.Create(m_device, memoryReq.size, memoryReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
		{
			NazaraError("Failed to allocate depth buffer memory");
			return false;
		}

		if (!m_depthBuffer.BindImageMemory(m_depthBufferMemory))
		{
			NazaraError("Failed to bind depth buffer to buffer");
			return false;
		}

		VkImageViewCreateInfo imageViewCreateInfo = {
			VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO, // VkStructureType            sType;
			nullptr,                                  // const void*                pNext;
			0,                                        // VkImageViewCreateFlags     flags;
			m_depthBuffer,                            // VkImage                    image;
			VK_IMAGE_VIEW_TYPE_2D,                    // VkImageViewType            viewType;
			m_depthStencilFormat,                     // VkFormat                   format;
			{                                         // VkComponentMapping         components;
				VK_COMPONENT_SWIZZLE_R,               // VkComponentSwizzle         .r;
				VK_COMPONENT_SWIZZLE_G,               // VkComponentSwizzle         .g;
				VK_COMPONENT_SWIZZLE_B,               // VkComponentSwizzle         .b;
				VK_COMPONENT_SWIZZLE_A                // VkComponentSwizzle         .a;
			},
			{                                         // VkImageSubresourceRange    subresourceRange;
				VK_IMAGE_ASPECT_DEPTH_BIT,            // VkImageAspectFlags         .aspectMask;
				0,                                    // uint32_t                   .baseMipLevel;
				1,                                    // uint32_t                   .levelCount;
				0,                                    // uint32_t                   .baseArrayLayer;
				1                                     // uint32_t                   .layerCount;
			}
		};

		if (!m_depthBufferView.Create(m_device, imageViewCreateInfo))
		{
			NazaraError("Failed to create depth buffer view");
			return false;
		}

		return true;
	}

	bool VkRenderWindow::SetupRenderPass(const Vector2ui& size)
	{
		std::array<VkAttachmentDescription, 2> attachments = {
			{
				{
					0,                                        // VkAttachmentDescriptionFlags    flags;
					m_colorFormat,                            // VkFormat                        format;
					VK_SAMPLE_COUNT_1_BIT,                    // VkSampleCountFlagBits           samples;
					VK_ATTACHMENT_LOAD_OP_CLEAR,              // VkAttachmentLoadOp              loadOp;
					VK_ATTACHMENT_STORE_OP_STORE,             // VkAttachmentStoreOp             storeOp;
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,          // VkAttachmentLoadOp              stencilLoadOp;
					VK_ATTACHMENT_STORE_OP_DONT_CARE,         // VkAttachmentStoreOp             stencilStoreOp;
					VK_IMAGE_LAYOUT_UNDEFINED, // VkImageLayout                   initialLayout;
					VK_IMAGE_LAYOUT_PRESENT_SRC_KHR  // VkImageLayout                   finalLayout;
				},
				{
					0,                                                // VkAttachmentDescriptionFlags    flags;
					m_depthStencilFormat,                             // VkFormat                        format;
					VK_SAMPLE_COUNT_1_BIT,                            // VkSampleCountFlagBits           samples;
					VK_ATTACHMENT_LOAD_OP_CLEAR,                      // VkAttachmentLoadOp              loadOp;
					VK_ATTACHMENT_STORE_OP_DONT_CARE,                     // VkAttachmentStoreOp             storeOp;
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,                  // VkAttachmentLoadOp              stencilLoadOp;
					VK_ATTACHMENT_STORE_OP_DONT_CARE,                 // VkAttachmentStoreOp             stencilStoreOp;
					VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, // VkImageLayout                   initialLayout;
					VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL  // VkImageLayout                   finalLayout;
				},
			}
		};

		VkAttachmentReference colorReference = {
			0,                                       // uint32_t         attachment;
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL // VkImageLayout    layout;
		};

		VkAttachmentReference depthReference = {
			1,                                               // uint32_t         attachment;
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL // VkImageLayout    layout;
		};

		VkSubpassDescription subpass = {
			0,                                                                        // VkSubpassDescriptionFlags       flags;
			VK_PIPELINE_BIND_POINT_GRAPHICS,                                          // VkPipelineBindPoint             pipelineBindPoint;
			0U,                                                                       // uint32_t                        inputAttachmentCount;
			nullptr,                                                                  // const VkAttachmentReference*    pInputAttachments;
			1U,                                                                       // uint32_t                        colorAttachmentCount;
			&colorReference,                                                          // const VkAttachmentReference*    pColorAttachments;
			nullptr,                                                                  // const VkAttachmentReference*    pResolveAttachments;
			(m_depthStencilFormat != VK_FORMAT_MAX_ENUM) ? &depthReference : nullptr, // const VkAttachmentReference*    pDepthStencilAttachment;
			0U,                                                                       // uint32_t                        preserveAttachmentCount;
			nullptr                                                                   // const uint32_t*                 pPreserveAttachments;
		};

		std::array<VkSubpassDependency, 2> dependencies;
		// First dependency at the start of the renderpass
		// Does the transition from final to initial layout 
		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL; // Producer of the dependency 
		dependencies[0].dstSubpass = 0; // Consumer is our single subpass that will wait for the execution depdendency
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		// Second dependency at the end the renderpass
		// Does the transition from the initial to the final layout
		dependencies[1].srcSubpass = 0; // Producer of the dependency is our single subpass
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL; // Consumer are all commands outside of the renderpass
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		VkRenderPassCreateInfo createInfo = {
			VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,              // VkStructureType                   sType;
			nullptr,                                                // const void*                       pNext;
			0,                                                      // VkRenderPassCreateFlags           flags;
			(m_depthStencilFormat != VK_FORMAT_MAX_ENUM) ? 2U : 1U, // uint32_t                          attachmentCount;
			attachments.data(),                                     // const VkAttachmentDescription*    pAttachments;
			1U,                                                     // uint32_t                          subpassCount;
			&subpass,                                               // const VkSubpassDescription*       pSubpasses;
			UInt32(dependencies.size()),                            // uint32_t                          dependencyCount;
			dependencies.data()                                     // const VkSubpassDependency*        pDependencies;
		};

		return m_renderPass.Create(m_device, createInfo);
	}

	bool VkRenderWindow::SetupSwapchain(Vk::Surface& surface, const Vector2ui& size)
	{
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		if (!surface.GetCapabilities(m_physicalDevice, &surfaceCapabilities))
		{
			NazaraError("Failed to query surface capabilities");
			return false;
		}

		Nz::UInt32 imageCount = surfaceCapabilities.minImageCount + 1;
		if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
			imageCount = surfaceCapabilities.maxImageCount;

		VkExtent2D extent;
		if (surfaceCapabilities.currentExtent.width == -1)
		{
			extent.width = Nz::Clamp<Nz::UInt32>(size.x, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
			extent.height = Nz::Clamp<Nz::UInt32>(size.y, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
		}
		else
			extent = surfaceCapabilities.currentExtent;

		std::vector<VkPresentModeKHR> presentModes;
		if (!surface.GetPresentModes(m_physicalDevice, &presentModes))
		{
			NazaraError("Failed to query supported present modes");
			return false;
		}

		VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
		for (VkPresentModeKHR presentMode : presentModes)
		{
			if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
				break;
			}

			if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
				swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		}

		VkSwapchainCreateInfoKHR swapchainInfo = {
			VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			nullptr,
			0,
			surface,
			imageCount,
			m_colorFormat,
			m_colorSpace,
			extent,
			1,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			0, nullptr,
			surfaceCapabilities.currentTransform,
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			swapchainPresentMode,
			VK_TRUE,
			0
		};

		if (!m_swapchain.Create(m_device, swapchainInfo))
		{
			NazaraError("Failed to create swapchain");
			return false;
		}

		return true;
	}
}
