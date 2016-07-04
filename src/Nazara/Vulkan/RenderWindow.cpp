// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Vulkan/RenderWindow.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Vulkan/Vulkan.hpp>
#include <array>
#include <stdexcept>
#include <Nazara/Vulkan/Debug.hpp>

namespace Nz
{
	RenderWindow::RenderWindow() :
	RenderTarget(), Window(), 
	m_surface(Nz::Vulkan::GetInstance()),
	m_forcedPhysicalDevice(nullptr)
	{
	}

	RenderWindow::RenderWindow(VideoMode mode, const String& title, UInt32 style) :
	RenderWindow()
	{
		ErrorFlags flags(ErrorFlag_ThrowException, true);
		Create(mode, title, style);
	}

	RenderWindow::RenderWindow(WindowHandle handle) :
	RenderWindow()
	{
		ErrorFlags flags(ErrorFlag_ThrowException, true);
		Create(handle);
	}

	RenderWindow::~RenderWindow()
	{
		// Nécessaire si Window::Destroy est appelé par son destructeur
		OnWindowDestroy();
	}

	bool RenderWindow::Acquire(const Vk::Framebuffer** framebuffer) const
	{
		UInt32 imageIndex;
		if (!m_swapchain.AcquireNextImage(std::numeric_limits<UInt64>::max(), m_imageReadySemaphore, VK_NULL_HANDLE, &imageIndex))
		{
			NazaraError("Failed to acquire next image");
			return false;
		}

		VkSemaphore waitSemaphore = m_imageReadySemaphore;
		VkCommandBuffer barrierBuffer = m_images[imageIndex].presentToDrawCmd;

		VkSubmitInfo submitInfo = {
			VK_STRUCTURE_TYPE_SUBMIT_INFO, // VkStructureType                sType
			nullptr,                       // const void*                    pNext
			1,                             // uint32_t                       waitSemaphoreCount
			&waitSemaphore,                // const VkSemaphore*             pWaitSemaphores
			nullptr,                       // const VkPipelineStageFlags*    pWaitDstStageMask
			1,                             // uint32_t                       commandBufferCount
			&barrierBuffer,                // const VkCommandBuffer*         pCommandBuffers
			0,                             // uint32_t                       signalSemaphoreCount
			nullptr                        // const VkSemaphore*             pSignalSemaphores
		};

		if (!m_presentQueue.Submit(1, &submitInfo))
		{
			NazaraError("Failed to submit memory barrier");
			return false;
		}

		m_lastImageAcquired = imageIndex;

		if (framebuffer)
			*framebuffer = &m_images[imageIndex].frameBuffer;

		return true;
	}

	bool RenderWindow::Create(VideoMode mode, const String& title, UInt32 style)
	{
		return Window::Create(mode, title, style);
	}

	bool RenderWindow::Create(WindowHandle handle)
	{
		return Window::Create(handle);
	}

	const Vk::DeviceHandle& RenderWindow::GetDevice() const
	{
		return m_device;
	}

	UInt32 RenderWindow::GetPresentableFamilyQueue() const
	{
		return m_presentableFamilyQueue;
	}

	const Vk::Surface& RenderWindow::GetSurface() const
	{
		return m_surface;
	}

	const Vk::Swapchain& RenderWindow::GetSwapchain() const
	{
		return m_swapchain;
	}

	void RenderWindow::Present()
	{
		m_presentQueue.Present(m_swapchain, m_lastImageAcquired);
	}

	bool RenderWindow::IsValid() const
	{
		return m_impl != nullptr;
	}

	void RenderWindow::SetPhysicalDevice(VkPhysicalDevice device)
	{
		m_forcedPhysicalDevice = device;
	}

	bool RenderWindow::OnWindowCreated()
	{
		OnRenderTargetSizeChange(this);

		#if defined(NAZARA_PLATFORM_WINDOWS)
		HWND handle = reinterpret_cast<HWND>(GetHandle());
		HINSTANCE instance = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(handle, GWLP_HINSTANCE));
		bool success = m_surface.Create(instance, handle);
		#else
			#error This OS is not supported by Vulkan
		#endif

		if (!success)
		{
			NazaraError("Failed to create Vulkan surface");
			return false;
		}

		m_device = Vulkan::SelectDevice(m_forcedPhysicalDevice, m_surface, &m_presentableFamilyQueue);
		if (!m_device)
		{
			NazaraError("Failed to get compatible Vulkan device");
			return false;
		}

		m_presentQueue = m_device->GetQueue(m_presentableFamilyQueue, 0);

		std::vector<VkSurfaceFormatKHR> surfaceFormats;
		if (!m_surface.GetFormats(m_forcedPhysicalDevice, &surfaceFormats))
		{
			NazaraError("Failed to query supported surface formats");
			return false;
		}

		VkFormat colorFormat;
		if (surfaceFormats.size() == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
			colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
		else
			colorFormat = surfaceFormats[0].format;

		VkColorSpaceKHR colorSpace = surfaceFormats[0].colorSpace;

		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		if (!m_surface.GetCapabilities(m_forcedPhysicalDevice, &surfaceCapabilities))
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
			extent.width = Nz::Clamp<Nz::UInt32>(GetWidth(), surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
			extent.height = Nz::Clamp<Nz::UInt32>(GetHeight(), surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
		}
		else
			extent = surfaceCapabilities.currentExtent;

		std::vector<VkPresentModeKHR> presentModes;
		if (!m_surface.GetPresentModes(m_forcedPhysicalDevice, &presentModes))
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
			m_surface,
			imageCount,
			colorFormat,
			colorSpace,
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

		if (!SetupRenderPass(colorFormat, VK_FORMAT_MAX_ENUM))
		{
			NazaraError("Failed to create render pass");
			return false;
		}

		if (!m_cmdPool.Create(m_device, m_presentableFamilyQueue))
		{
			NazaraError("Failed to create present command pool");
			return false;
		}

		auto cmdBuffers = m_cmdPool.AllocateCommandBuffers(imageCount * 2, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

		VkImageSubresourceRange imageRange = {
			VK_IMAGE_ASPECT_COLOR_BIT, // VkImageAspectFlags                     aspectMask
			0,                         // uint32_t                               baseMipLevel
			1,                         // uint32_t                               levelCount
			0,                         // uint32_t                               baseArrayLayer
			1                          // uint32_t                               layerCount
		};

		m_images.resize(imageCount);
		for (UInt32 i = 0; i < imageCount; ++i)
		{
			ImageData& imageData = m_images[i];
			imageData.presentToDrawCmd = std::move(cmdBuffers[i*2 + 0]);
			imageData.drawToPresentCmd = std::move(cmdBuffers[i*2 + 1]);

			VkImage image = m_swapchain.GetBuffer(i).image;

			// Barriers
			VkImageMemoryBarrier presentToDrawBarrier = {
				VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER, // VkStructureType         sType
				nullptr,                                // const void*             pNext
				VK_ACCESS_MEMORY_READ_BIT,              // VkAccessFlags           srcAccessMask
				VK_ACCESS_TRANSFER_WRITE_BIT,           // VkAccessFlags           dstAccessMask
				VK_IMAGE_LAYOUT_UNDEFINED,              // VkImageLayout           oldLayout
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,   // VkImageLayout           newLayout
				VK_QUEUE_FAMILY_IGNORED,                // uint32_t                srcQueueFamilyIndex
				VK_QUEUE_FAMILY_IGNORED,                // uint32_t                dstQueueFamilyIndex
				image,                                  // VkImage                 image
				imageRange                              // VkImageSubresourceRange subresourceRange
			};

			imageData.presentToDrawCmd.Begin(0);

			m_device->vkCmdPipelineBarrier(imageData.presentToDrawCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &presentToDrawBarrier);

			if (!imageData.presentToDrawCmd.End())
			{
				NazaraError("Failed to record present to draw barrier command buffer for image #" + String::Number(i));
				return false;
			}

			VkImageMemoryBarrier drawToPresentBarrier = {
				VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER, // VkStructureType         sType
				nullptr,                                // const void*             pNext
				VK_ACCESS_TRANSFER_WRITE_BIT,           // VkAccessFlags           srcAccessMask
				VK_ACCESS_MEMORY_READ_BIT,              // VkAccessFlags           dstAccessMask
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,   // VkImageLayout           oldLayout
				VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,        // VkImageLayout           newLayout
				VK_QUEUE_FAMILY_IGNORED,                // uint32_t                srcQueueFamilyIndex
				VK_QUEUE_FAMILY_IGNORED,                // uint32_t                dstQueueFamilyIndex
				image,                                  // VkImage                 image
				imageRange                              // VkImageSubresourceRange subresourceRange
			};

			imageData.drawToPresentCmd.Begin(0);

			m_device->vkCmdPipelineBarrier(imageData.drawToPresentCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &drawToPresentBarrier);

			if (!imageData.drawToPresentCmd.End())
			{
				NazaraError("Failed to record draw to present barrier command buffer for image #" + String::Number(i));
				return false;
			}

			// Framebuffer
			std::array<VkImageView, 2> attachments = {m_swapchain.GetBuffer(i).view, VK_NULL_HANDLE};

			VkFramebufferCreateInfo frameBufferCreate = {
				VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,    // VkStructureType             sType;
				nullptr,                                      // const void*                 pNext;
				0,                                            // VkFramebufferCreateFlags    flags;
				m_renderPass,                                 // VkRenderPass                renderPass;
				(attachments[1] != VK_NULL_HANDLE) ? 2U : 1U, // uint32_t                    attachmentCount;
				attachments.data(),                           // const VkImageView*          pAttachments;
				extent.width,                                 // uint32_t                    width;
				extent.height,                                // uint32_t                    height;
				1U                                            // uint32_t                    layers;
			};

			if (!imageData.frameBuffer.Create(m_device, frameBufferCreate))
			{
				NazaraError("Failed to create framebuffer for image #" + String::Number(i));
				return false;
			}
		}

		m_imageReadySemaphore.Create(m_device);

		m_clock.Restart();

		return true;
	}

	void RenderWindow::OnWindowDestroy()
	{
		m_images.clear();
		m_renderPass.Destroy();
		m_cmdPool.Destroy();

		m_swapchain.Destroy();
		m_surface.Destroy();
	}

	void RenderWindow::OnWindowResized()
	{
		OnRenderTargetSizeChange(this);
	}

	bool RenderWindow::SetupRenderPass(VkFormat colorFormat, VkFormat depthFormat)
	{
		std::array<VkAttachmentDescription, 2> attachments = {
			{
				{
					0,                                        // VkAttachmentDescriptionFlags    flags;
					colorFormat,                              // VkFormat                        format;
					VK_SAMPLE_COUNT_1_BIT,                    // VkSampleCountFlagBits           samples;
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,          // VkAttachmentLoadOp              loadOp;
					VK_ATTACHMENT_STORE_OP_STORE,             // VkAttachmentStoreOp             storeOp;
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,          // VkAttachmentLoadOp              stencilLoadOp;
					VK_ATTACHMENT_STORE_OP_DONT_CARE,         // VkAttachmentStoreOp             stencilStoreOp;
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, // VkImageLayout                   initialLayout;
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL  // VkImageLayout                   finalLayout;
				},
				{
					0,                                        // VkAttachmentDescriptionFlags    flags;
					depthFormat,                              // VkFormat                        format;
					VK_SAMPLE_COUNT_1_BIT,                    // VkSampleCountFlagBits           samples;
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,          // VkAttachmentLoadOp              loadOp;
					VK_ATTACHMENT_STORE_OP_STORE,             // VkAttachmentStoreOp             storeOp;
					VK_ATTACHMENT_LOAD_OP_DONT_CARE,          // VkAttachmentLoadOp              stencilLoadOp;
					VK_ATTACHMENT_STORE_OP_DONT_CARE,         // VkAttachmentStoreOp             stencilStoreOp;
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, // VkImageLayout                   initialLayout;
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL  // VkImageLayout                   finalLayout;
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
			0,                                                               // VkSubpassDescriptionFlags       flags;
			VK_PIPELINE_BIND_POINT_GRAPHICS,                                 // VkPipelineBindPoint             pipelineBindPoint;
			0U,                                                              // uint32_t                        inputAttachmentCount;
			nullptr,                                                         // const VkAttachmentReference*    pInputAttachments;
			1U,                                                              // uint32_t                        colorAttachmentCount;
			&colorReference,                                                 // const VkAttachmentReference*    pColorAttachments;
			nullptr,                                                         // const VkAttachmentReference*    pResolveAttachments;
			(depthFormat != VK_FORMAT_MAX_ENUM) ? &depthReference : nullptr, // const VkAttachmentReference*    pDepthStencilAttachment;
			0U,                                                              // uint32_t                        preserveAttachmentCount;
			nullptr                                                          // const uint32_t*                 pPreserveAttachments;
		};

		VkRenderPassCreateInfo createInfo = {
			VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,     // VkStructureType                   sType;
			nullptr,                                       // const void*                       pNext;
			0,                                             // VkRenderPassCreateFlags           flags;
			(depthFormat != VK_FORMAT_MAX_ENUM) ? 2U : 1U, // uint32_t                          attachmentCount;
			attachments.data(),                            // const VkAttachmentDescription*    pAttachments;
			1U,                                            // uint32_t                          subpassCount;
			&subpass,                                      // const VkSubpassDescription*       pSubpasses;
			0U,                                            // uint32_t                          dependencyCount;
			nullptr                                        // const VkSubpassDependency*        pDependencies;
		};

		return m_renderPass.Create(m_device, createInfo);
	}
}
