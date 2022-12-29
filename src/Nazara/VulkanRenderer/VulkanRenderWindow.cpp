// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VulkanRenderWindow.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Renderer/RenderWindow.hpp>
#include <Nazara/Utility/PixelFormat.hpp>
#include <Nazara/VulkanRenderer/Vulkan.hpp>
#include <Nazara/VulkanRenderer/VulkanCommandPool.hpp>
#include <Nazara/VulkanRenderer/VulkanDevice.hpp>
#include <Nazara/VulkanRenderer/VulkanSurface.hpp>
#include <Nazara/Utils/StackArray.hpp>
#include <array>
#include <stdexcept>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	VulkanRenderWindow::VulkanRenderWindow(RenderWindow& owner) :
	m_currentFrame(0),
	m_owner(owner),
	m_shouldRecreateSwapchain(false)
	{
	}

	VulkanRenderWindow::~VulkanRenderWindow()
	{
		if (m_device)
			m_device->WaitForIdle();

		m_concurrentImageData.clear();
		m_renderPass.reset();
		m_framebuffers.clear();
		m_swapchain.Destroy();
	}

	RenderFrame VulkanRenderWindow::Acquire()
	{
		bool invalidateFramebuffer = false;

		Vector2ui size = m_owner.GetSize();
		// Special case: window is minimized
		if (size == Nz::Vector2ui::Zero() || m_owner.IsMinimized())
			return RenderFrame();

		if (m_shouldRecreateSwapchain || size != m_swapchainSize)
		{
			Vk::Surface& vulkanSurface = static_cast<VulkanSurface*>(m_owner.GetSurface())->GetSurface();

			OnRenderTargetSizeChange(this, size);

			if (!CreateSwapchain(vulkanSurface, size))
				throw std::runtime_error("failed to recreate swapchain");

			m_shouldRecreateSwapchain = false;
			invalidateFramebuffer = true;
		}

		VulkanRenderImage& currentFrame = *m_concurrentImageData[m_currentFrame];
		Vk::Fence& inFlightFence = currentFrame.GetInFlightFence();

		// Wait until previous rendering to this image has been done
		inFlightFence.Wait();

		UInt32 imageIndex;
		m_swapchain.AcquireNextImage(std::numeric_limits<UInt64>::max(), currentFrame.GetImageAvailableSemaphore(), VK_NULL_HANDLE, &imageIndex);

		switch (m_swapchain.GetLastErrorCode())
		{
			case VK_SUCCESS:
				break;

			case VK_SUBOPTIMAL_KHR:
				m_shouldRecreateSwapchain = true; //< Recreate swapchain next time
				break;

			case VK_ERROR_OUT_OF_DATE_KHR:
				m_shouldRecreateSwapchain = true;
				return Acquire();

			// Not expected (since timeout is infinite)
			case VK_TIMEOUT:
			case VK_NOT_READY:
			// Unhandled errors
			case VK_ERROR_DEVICE_LOST:
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			case VK_ERROR_OUT_OF_HOST_MEMORY:
			case VK_ERROR_SURFACE_LOST_KHR: //< TODO: Handle it by recreating the surface?
			case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
			default:
				throw std::runtime_error("failed to acquire next image: " + TranslateVulkanError(m_swapchain.GetLastErrorCode()));
		}

		if (m_inflightFences[imageIndex])
			m_inflightFences[imageIndex]->Wait();

		m_inflightFences[imageIndex] = &inFlightFence;
		m_inflightFences[imageIndex]->Reset();

		currentFrame.Reset(imageIndex);

		return RenderFrame(&currentFrame, invalidateFramebuffer, size, imageIndex);
	}

	bool VulkanRenderWindow::Create(RendererImpl* /*renderer*/, RenderSurface* surface, const RenderWindowParameters& parameters)
	{
		std::shared_ptr<VulkanDevice> device = std::static_pointer_cast<VulkanDevice>(m_owner.GetRenderDevice());

		const auto& physDeviceInfo = device->GetPhysicalDeviceInfo();

		Vk::Surface& vulkanSurface = static_cast<VulkanSurface*>(surface)->GetSurface();

		const std::vector<Vk::Device::QueueFamilyInfo>& queueFamilyInfo = device->GetEnabledQueues();
		UInt32 graphicsFamilyQueueIndex = UINT32_MAX;
		UInt32 presentableFamilyQueueIndex = UINT32_MAX;

		for (const Vk::Device::QueueFamilyInfo& queueInfo : queueFamilyInfo)
		{
			bool supported = false;
			if (vulkanSurface.GetSupportPresentation(physDeviceInfo.physDevice, queueInfo.familyIndex, &supported) && supported)
			{
				if (presentableFamilyQueueIndex == UINT32_MAX || queueInfo.flags & VK_QUEUE_GRAPHICS_BIT)
				{
					presentableFamilyQueueIndex = queueInfo.familyIndex;
					if (queueInfo.flags & VK_QUEUE_GRAPHICS_BIT)
					{
						graphicsFamilyQueueIndex = queueInfo.familyIndex;
						break;
					}
				}
			}
		}

		if (presentableFamilyQueueIndex == UINT32_MAX)
		{
			NazaraError("device doesn't support presenting to this surface");
			return false;
		}

		if (graphicsFamilyQueueIndex == UINT32_MAX)
		{
			for (const Vk::Device::QueueFamilyInfo& queueInfo : queueFamilyInfo)
			{
				if (queueInfo.flags & VK_QUEUE_GRAPHICS_BIT)
				{
					graphicsFamilyQueueIndex = queueInfo.familyIndex;
					break;
				}
			}
		}

		if (graphicsFamilyQueueIndex == UINT32_MAX)
		{
			NazaraError("device doesn't support graphics operations");
			return false;
		}

		UInt32 transferFamilyQueueIndex = UINT32_MAX;
		// Search for a transfer queue (first one being different to the graphics one)
		for (const Vk::Device::QueueFamilyInfo& queueInfo : queueFamilyInfo)
		{
			// Transfer bit is not mandatory if compute and graphics bits are set (as they implicitly support transfer)
			if (queueInfo.flags & (VK_QUEUE_COMPUTE_BIT | VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT))
			{
				transferFamilyQueueIndex = queueInfo.familyIndex;
				if (transferFamilyQueueIndex != graphicsFamilyQueueIndex)
					break;
			}
		}

		assert(transferFamilyQueueIndex != UINT32_MAX);

		m_device = std::move(device);

		m_graphicsQueue = m_device->GetQueue(graphicsFamilyQueueIndex, 0);
		m_presentQueue = m_device->GetQueue(presentableFamilyQueueIndex, 0);
		m_transferQueue = m_device->GetQueue(transferFamilyQueueIndex, 0);

		std::vector<VkSurfaceFormatKHR> surfaceFormats;
		if (!vulkanSurface.GetFormats(physDeviceInfo.physDevice, &surfaceFormats))
		{
			NazaraError("Failed to query supported surface formats");
			return false;
		}

		m_surfaceFormat = [&] () -> VkSurfaceFormatKHR
		{
			if (surfaceFormats.size() == 1 && surfaceFormats.front().format == VK_FORMAT_UNDEFINED)
			{
				// If the list contains one undefined format, it means any format can be used
				return { VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
			}
			else
			{
				// Search for RGBA8 and default to first format
				for (const VkSurfaceFormatKHR& surfaceFormat : surfaceFormats)
				{
					if (surfaceFormat.format == VK_FORMAT_R8G8B8A8_UNORM)
						return surfaceFormat;
				}

				return surfaceFormats.front();
			}
		}();

		m_depthStencilFormat = VK_FORMAT_MAX_ENUM;
		if (!parameters.depthFormats.empty())
		{
			for (PixelFormat format : parameters.depthFormats)
			{
				switch (format)
				{
					case PixelFormat::Depth16:
						m_depthStencilFormat = VK_FORMAT_D16_UNORM;
						break;

					case PixelFormat::Depth16Stencil8:
						m_depthStencilFormat = VK_FORMAT_D16_UNORM_S8_UINT;
						break;

					case PixelFormat::Depth24:
					case PixelFormat::Depth24Stencil8:
						m_depthStencilFormat = VK_FORMAT_D24_UNORM_S8_UINT;
						break;

					case PixelFormat::Depth32F:
						m_depthStencilFormat = VK_FORMAT_D32_SFLOAT;
						break;

					case PixelFormat::Depth32FStencil8:
						m_depthStencilFormat = VK_FORMAT_D32_SFLOAT_S8_UINT;
						break;

					case PixelFormat::Stencil1:
					case PixelFormat::Stencil4:
					case PixelFormat::Stencil8:
						m_depthStencilFormat = VK_FORMAT_S8_UINT;
						break;

					case PixelFormat::Stencil16:
						continue;

					default:
					{
						PixelFormatContent formatContent = PixelFormatInfo::GetContent(format);
						if (formatContent != PixelFormatContent::DepthStencil && formatContent != PixelFormatContent::Stencil)
							NazaraWarning("Invalid format " + PixelFormatInfo::GetName(format) + " for depth-stencil attachment");

						m_depthStencilFormat = VK_FORMAT_MAX_ENUM;
						break;
					}
				}

				if (m_depthStencilFormat != VK_FORMAT_MAX_ENUM)
				{
					VkFormatProperties formatProperties = m_device->GetInstance().GetPhysicalDeviceFormatProperties(physDeviceInfo.physDevice, m_depthStencilFormat);
					if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
						break; //< Found it

					m_depthStencilFormat = VK_FORMAT_MAX_ENUM;
				}
			}
		}

		if (!SetupRenderPass())
		{
			NazaraError("Failed to create render pass");
			return false;
		}

		if (!CreateSwapchain(vulkanSurface, m_owner.GetSize()))
		{
			NazaraError("failed to create swapchain");
			return false;
		}

		return true;
	}

	std::shared_ptr<CommandPool> VulkanRenderWindow::CreateCommandPool(QueueType queueType)
	{
		UInt32 queueFamilyIndex = [&] {
			switch (queueType)
			{
				case QueueType::Compute:
					return m_device->GetDefaultFamilyIndex(QueueType::Compute);

				case QueueType::Graphics:
					return m_graphicsQueue.GetQueueFamilyIndex();

				case QueueType::Transfer:
					return m_transferQueue.GetQueueFamilyIndex();
			}

			throw std::runtime_error("invalid queue type " + std::to_string(UnderlyingCast(queueType)));
		}();

		return std::make_shared<VulkanCommandPool>(*m_device, queueFamilyIndex);
	}

	const VulkanWindowFramebuffer& VulkanRenderWindow::GetFramebuffer(std::size_t i) const
	{
		assert(i < m_framebuffers.size());
		return m_framebuffers[i];
	}

	std::size_t VulkanRenderWindow::GetFramebufferCount() const
	{
		return m_framebuffers.size();
	}

	const VulkanRenderPass& VulkanRenderWindow::GetRenderPass() const
	{
		return *m_renderPass;
	}

	const Vector2ui& VulkanRenderWindow::GetSize() const
	{
		return m_swapchainSize;
	}

	void VulkanRenderWindow::Present(UInt32 imageIndex, VkSemaphore waitSemaphore)
	{
		NazaraAssert(imageIndex < m_inflightFences.size(), "Invalid image index");

		m_currentFrame = (m_currentFrame + 1) % m_inflightFences.size();

		m_presentQueue.Present(m_swapchain, imageIndex, waitSemaphore);

		switch (m_presentQueue.GetLastErrorCode())
		{
			case VK_SUCCESS:
				break;

			case VK_ERROR_OUT_OF_DATE_KHR:
			case VK_SUBOPTIMAL_KHR:
			{
				// Recreate swapchain next time
				m_shouldRecreateSwapchain = true;
				break;
			}

			// Unhandled errors
			case VK_ERROR_DEVICE_LOST:
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			case VK_ERROR_OUT_OF_HOST_MEMORY:
			case VK_ERROR_SURFACE_LOST_KHR: //< TODO: Handle it by recreating the surface?
			case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
			default:
				throw std::runtime_error("Failed to present image: " + TranslateVulkanError(m_swapchain.GetLastErrorCode()));
		}
	}

	bool VulkanRenderWindow::CreateSwapchain(Vk::Surface& surface, const Vector2ui& size)
	{
		assert(m_device);
		if (!SetupSwapchain(m_device->GetPhysicalDeviceInfo(), surface, size))
		{
			NazaraError("Failed to create swapchain");
			return false;
		}

		if (m_depthStencilFormat != VK_FORMAT_MAX_ENUM && !SetupDepthBuffer(size))
		{
			NazaraError("Failed to create depth buffer");
			return false;
		}

		if (!SetupFrameBuffers(size))
		{
			NazaraError("failed to create framebuffers");
			return false;
		}

		return true;
	}

	bool VulkanRenderWindow::SetupDepthBuffer(const Vector2ui& size)
	{
		VkImageCreateInfo imageCreateInfo = {
			VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,                                           // VkStructureType          sType;
			nullptr,                                                                       // const void*              pNext;
			0U,                                                                            // VkImageCreateFlags       flags;
			VK_IMAGE_TYPE_2D,                                                              // VkImageType              imageType;
			m_depthStencilFormat,                                                          // VkFormat                 format;
			{size.x, size.y, 1U},                                                          // VkExtent3D               extent;
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

		if (!m_depthBuffer.Create(*m_device, imageCreateInfo))
		{
			NazaraError("Failed to create depth buffer");
			return false;
		}

		VkMemoryRequirements memoryReq = m_depthBuffer.GetMemoryRequirements();
		if (!m_depthBufferMemory.Create(*m_device, memoryReq.size, memoryReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
		{
			NazaraError("Failed to allocate depth buffer memory");
			return false;
		}

		if (!m_depthBuffer.BindImageMemory(m_depthBufferMemory))
		{
			NazaraError("Failed to bind depth buffer to buffer");
			return false;
		}

		PixelFormat format = FromVulkan(m_depthStencilFormat).value();

		VkImageAspectFlags aspectMask;
		if (PixelFormatInfo::GetContent(format) == PixelFormatContent::DepthStencil)
			aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		else
			aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

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
				aspectMask,                           // VkImageAspectFlags         .aspectMask;
				0,                                    // uint32_t                   .baseMipLevel;
				1,                                    // uint32_t                   .levelCount;
				0,                                    // uint32_t                   .baseArrayLayer;
				1                                     // uint32_t                   .layerCount;
			}
		};

		if (!m_depthBufferView.Create(*m_device, imageViewCreateInfo))
		{
			NazaraError("Failed to create depth buffer view");
			return false;
		}

		return true;
	}

	bool VulkanRenderWindow::SetupFrameBuffers(const Vector2ui& size)
	{
		UInt32 imageCount = m_swapchain.GetImageCount();

		m_framebuffers.clear();
		m_framebuffers.reserve(imageCount);
		for (UInt32 i = 0; i < imageCount; ++i)
		{
			std::array<VkImageView, 2> attachments = { m_swapchain.GetImage(i).view, m_depthBufferView };

			VkFramebufferCreateInfo frameBufferCreate = {
				VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
				nullptr,
				0,
				m_renderPass->GetRenderPass(),
				(attachments[1] != VK_NULL_HANDLE) ? 2U : 1U,
				attachments.data(),
				size.x,
				size.y,
				1U
			};

			Vk::Framebuffer framebuffer;

			if (!framebuffer.Create(*m_device, frameBufferCreate))
			{
				NazaraError("Failed to create framebuffer for image #" + NumberToString(i) + ": " + TranslateVulkanError(framebuffer.GetLastErrorCode()));
				return false;
			}

			m_framebuffers.emplace_back(std::move(framebuffer));
		}

		return true;
	}

	bool VulkanRenderWindow::SetupRenderPass()
	{
		std::optional<PixelFormat> colorFormat = FromVulkan(m_surfaceFormat.format);
		if (!colorFormat)
		{
			NazaraError("unhandled vulkan pixel format (0x" + NumberToString(m_surfaceFormat.format, 16) + ")");
			return false;
		}

		std::optional<PixelFormat> depthStencilFormat;
		if (m_depthStencilFormat != VK_FORMAT_MAX_ENUM)
		{
			depthStencilFormat = FromVulkan(m_depthStencilFormat);
			if (!depthStencilFormat)
			{
				NazaraError("unhandled vulkan pixel format (0x" + NumberToString(m_depthStencilFormat, 16) + ")");
				return false;
			}
		}

		std::vector<RenderPass::Attachment> attachments;
		std::vector<RenderPass::SubpassDescription> subpassDescriptions;
		std::vector<RenderPass::SubpassDependency> subpassDependencies;

		BuildRenderPass(*colorFormat, depthStencilFormat.value_or(PixelFormat::Undefined), attachments, subpassDescriptions, subpassDependencies);
		m_renderPass.emplace(*m_device, std::move(attachments), std::move(subpassDescriptions), std::move(subpassDependencies));
		return true;
	}

	bool VulkanRenderWindow::SetupSwapchain(const Vk::PhysicalDevice& deviceInfo, Vk::Surface& surface, const Vector2ui& size)
	{
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		if (!surface.GetCapabilities(deviceInfo.physDevice, &surfaceCapabilities))
		{
			NazaraError("Failed to query surface capabilities");
			return false;
		}

		Nz::UInt32 imageCount = surfaceCapabilities.minImageCount + 1;
		if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
			imageCount = surfaceCapabilities.maxImageCount;

		VkExtent2D extent;
		if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF)
		{
			extent.width = Nz::Clamp<Nz::UInt32>(size.x, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
			extent.height = Nz::Clamp<Nz::UInt32>(size.y, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
		}
		else
			extent = surfaceCapabilities.currentExtent;

		std::vector<VkPresentModeKHR> presentModes;
		if (!surface.GetPresentModes(deviceInfo.physDevice, &presentModes))
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

		// Ensure all operations on the device have been finished before recreating the swapchain (this can be avoided but is more complicated)
		m_device->WaitForIdle();

		VkSwapchainCreateInfoKHR swapchainInfo = {
			VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			nullptr,
			0,
			surface,
			imageCount,
			m_surfaceFormat.format,
			m_surfaceFormat.colorSpace,
			extent,
			1,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			0, nullptr,
			surfaceCapabilities.currentTransform,
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			swapchainPresentMode,
			VK_TRUE,
			m_swapchain
		};

		Vk::Swapchain newSwapchain;
		if (!newSwapchain.Create(*m_device, swapchainInfo))
		{
			NazaraError("failed to create swapchain: " + TranslateVulkanError(newSwapchain.GetLastErrorCode()));
			return false;
		}

		m_swapchain = std::move(newSwapchain);
		m_swapchainSize = size;

		// Framebuffers
		imageCount = m_swapchain.GetImageCount();

		m_inflightFences.resize(imageCount);

		if (m_concurrentImageData.size() != imageCount)
		{
			m_concurrentImageData.clear();
			m_concurrentImageData.reserve(imageCount);

			for (std::size_t i = 0; i < imageCount; ++i)
				m_concurrentImageData.emplace_back(std::make_unique<VulkanRenderImage>(*this));
		}

		return true;
	}
}

#if defined(NAZARA_PLATFORM_WINDOWS)
#include <Nazara/Core/AntiWindows.hpp>
#endif
