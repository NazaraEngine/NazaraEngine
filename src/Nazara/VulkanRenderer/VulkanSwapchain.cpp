// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/VulkanRenderer/VulkanSwapchain.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/PixelFormat.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Platform/WindowHandle.hpp>
#include <Nazara/Renderer/WindowSwapchain.hpp>
#include <Nazara/VulkanRenderer/Vulkan.hpp>
#include <Nazara/VulkanRenderer/VulkanCommandPool.hpp>
#include <Nazara/VulkanRenderer/VulkanDevice.hpp>
#include <NazaraUtils/StackArray.hpp>
#include <array>
#include <stdexcept>

#ifdef VK_USE_PLATFORM_WIN32_KHR
#include <Windows.h>
#include <vulkan/vulkan_win32.h>
#endif

#ifdef VK_USE_PLATFORM_METAL_EXT
#include <objc/runtime.h>
#include <vulkan/vulkan_metal.h>
#endif


namespace Nz
{
#ifdef VK_USE_PLATFORM_METAL_EXT
	id CreateAndAttachMetalLayer(void* window);
#endif

	VulkanSwapchain::VulkanSwapchain(VulkanDevice& device, WindowHandle windowHandle, const Vector2ui& windowSize, const SwapchainParameters& parameters) :
	m_currentFrame(0),
	m_surface(device.GetInstance()),
	m_swapchainSize(windowSize),
	m_device(device),
	m_shouldRecreateSwapchain(false)
	{
		if (!SetupSurface(windowHandle))
			throw std::runtime_error("failed to create surface");

		const auto& physDeviceInfo = m_device.GetPhysicalDeviceInfo();

		const std::vector<Vk::Device::QueueFamilyInfo>& queueFamilyInfo = m_device.GetEnabledQueues();
		UInt32 graphicsFamilyQueueIndex = UINT32_MAX;
		UInt32 presentableFamilyQueueIndex = UINT32_MAX;

		for (const Vk::Device::QueueFamilyInfo& queueInfo : queueFamilyInfo)
		{
			bool supported = false;
			if (m_surface.GetSupportPresentation(physDeviceInfo.physDevice, queueInfo.familyIndex, &supported) && supported)
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
			throw std::runtime_error("device doesn't support presenting to this surface");

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
			throw std::runtime_error("device doesn't support graphics operation");

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

		m_graphicsQueue = m_device.GetQueue(graphicsFamilyQueueIndex, 0);
		m_presentQueue = m_device.GetQueue(presentableFamilyQueueIndex, 0);
		m_transferQueue = m_device.GetQueue(transferFamilyQueueIndex, 0);

		std::vector<VkSurfaceFormatKHR> surfaceFormats;
		if (!m_surface.GetFormats(physDeviceInfo.physDevice, &surfaceFormats))
			throw std::runtime_error("failed to query supported surface formats");

		m_surfaceFormat = [&]() -> VkSurfaceFormatKHR
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

		m_depthStencilFormat = VK_FORMAT_UNDEFINED;
		if (!parameters.depthFormats.empty())
		{
			for (PixelFormat format : parameters.depthFormats)
			{
				PixelFormatContent formatContent = PixelFormatInfo::GetContent(format);
				if (formatContent != PixelFormatContent::DepthStencil && formatContent != PixelFormatContent::Stencil)
					NazaraWarning("invalid format {0} for depth-stencil attachment", PixelFormatInfo::GetName(format));

				m_depthStencilFormat = ToVulkan(format);
				if (m_depthStencilFormat == VK_FORMAT_UNDEFINED)
					continue;

				VkFormatProperties formatProperties = m_device.GetInstance().GetPhysicalDeviceFormatProperties(physDeviceInfo.physDevice, m_depthStencilFormat);
				if (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
					break; //< Found it

				m_depthStencilFormat = VK_FORMAT_UNDEFINED;
			}

			if (m_depthStencilFormat == VK_FORMAT_UNDEFINED)
				throw std::runtime_error("failed to find a support depth-stencil format");
		}

		std::vector<VkPresentModeKHR> presentModes;
		if (!m_surface.GetPresentModes(physDeviceInfo.physDevice, &presentModes))
			throw std::runtime_error("failed to query supported present modes");

		m_supportedPresentModes.Clear();
		for (VkPresentModeKHR vkPresentMode : presentModes)
		{
			if (auto presentModeOpt = FromVulkan(vkPresentMode))
				m_supportedPresentModes |= *presentModeOpt;
		}

		m_presentMode = PresentMode::VerticalSync; //< guaranteed to be supported
		for (PresentMode presentMode : parameters.presentMode)
		{
			if (m_supportedPresentModes & presentMode)
			{
				m_presentMode = presentMode;
				break;
			}
		}

		if (!SetupRenderPass())
			throw std::runtime_error("failed to create renderpass");

		if (!CreateSwapchain())
			throw std::runtime_error("failed to create swapchain");
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		m_device.WaitForIdle();

		m_concurrentImageData.clear();
		m_renderPass.reset();
		m_framebuffers.clear();
		m_swapchain.Destroy();
	}

	RenderFrame VulkanSwapchain::AcquireFrame()
	{
		bool invalidateFramebuffer = false;

		if (m_shouldRecreateSwapchain)
		{
			if (!CreateSwapchain())
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
				return AcquireFrame();

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

		return RenderFrame(&currentFrame, invalidateFramebuffer, m_swapchainSize);
	}

	std::shared_ptr<CommandPool> VulkanSwapchain::CreateCommandPool(QueueType queueType)
	{
		UInt32 queueFamilyIndex = [&] {
			switch (queueType)
			{
				case QueueType::Compute:
					return m_device.GetDefaultFamilyIndex(QueueType::Compute);

				case QueueType::Graphics:
					return m_graphicsQueue.GetQueueFamilyIndex();

				case QueueType::Transfer:
					return m_transferQueue.GetQueueFamilyIndex();
			}

			throw std::runtime_error("invalid queue type " + std::to_string(UnderlyingCast(queueType)));
		}();

		return std::make_shared<VulkanCommandPool>(m_device, queueFamilyIndex);
	}

	bool VulkanSwapchain::CreateSwapchain()
	{
		if (!SetupSwapchain(m_device.GetPhysicalDeviceInfo()))
		{
			NazaraError("failed to create swapchain");
			return false;
		}

		if (m_depthStencilFormat != VK_FORMAT_MAX_ENUM && !SetupDepthBuffer())
		{
			NazaraError("failed to create depth buffer");
			return false;
		}

		if (!SetupFrameBuffers())
		{
			NazaraError("failed to create framebuffers");
			return false;
		}

		return true;
	}

	const VulkanWindowFramebuffer& VulkanSwapchain::GetFramebuffer(std::size_t imageIndex) const
	{
		assert(imageIndex < m_framebuffers.size());
		return m_framebuffers[imageIndex];
	}

	std::size_t VulkanSwapchain::GetFramebufferCount() const
	{
		return m_framebuffers.size();
	}

	const VulkanRenderPass& VulkanSwapchain::GetRenderPass() const
	{
		return *m_renderPass;
	}

	const Vector2ui& VulkanSwapchain::GetSize() const
	{
		return m_swapchainSize;
	}

	PresentMode VulkanSwapchain::GetPresentMode() const
	{
		return m_presentMode;
	}

	PresentModeFlags VulkanSwapchain::GetSupportedPresentModes() const
	{
		return m_supportedPresentModes;
	}

	RenderResources& VulkanSwapchain::GetTransientResources()
	{
		return *m_concurrentImageData[m_currentFrame];
	}

	void VulkanSwapchain::NotifyResize(const Vector2ui& newSize)
	{
		OnSwapchainResize(this, newSize);

		m_swapchainSize = newSize;
		m_shouldRecreateSwapchain = true;
	}

	void VulkanSwapchain::Present(UInt32 imageIndex, VkSemaphore waitSemaphore)
	{
		NazaraAssertMsg(imageIndex < m_inflightFences.size(), "Invalid image index");

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
				throw std::runtime_error("failed to present image: " + TranslateVulkanError(m_swapchain.GetLastErrorCode()));
		}
	}

	void VulkanSwapchain::SetPresentMode(PresentMode presentMode)
	{
		NazaraAssertMsg(m_supportedPresentModes & presentMode, "unsupported present mode");

		if (m_presentMode != presentMode)
		{
			m_presentMode = presentMode;
			m_shouldRecreateSwapchain = true;
		}
	}

	bool VulkanSwapchain::SetupDepthBuffer()
	{
		VkImageCreateInfo imageCreateInfo = {
			VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,                                           // VkStructureType          sType;
			nullptr,                                                                       // const void*              pNext;
			0U,                                                                            // VkImageCreateFlags       flags;
			VK_IMAGE_TYPE_2D,                                                              // VkImageType              imageType;
			m_depthStencilFormat,                                                          // VkFormat                 format;
			{ m_swapchainSize.x, m_swapchainSize.y, 1U },                                  // VkExtent3D               extent;
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
			NazaraError("failed to create depth buffer");
			return false;
		}

		VkMemoryRequirements memoryReq = m_depthBuffer.GetMemoryRequirements();
		if (!m_depthBufferMemory.Create(m_device, memoryReq.size, memoryReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
		{
			NazaraError("failed to allocate depth buffer memory");
			return false;
		}

		if (!m_depthBuffer.BindImageMemory(m_depthBufferMemory))
		{
			NazaraError("failed to bind depth buffer to buffer");
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

		if (!m_depthBufferView.Create(m_device, imageViewCreateInfo))
		{
			NazaraError("failed to create depth buffer view");
			return false;
		}

		return true;
	}

	bool VulkanSwapchain::SetupFrameBuffers()
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
				m_swapchainSize.x,
				m_swapchainSize.y,
				1U
			};

			Vk::Framebuffer framebuffer;

			if (!framebuffer.Create(*m_swapchain.GetDevice(), frameBufferCreate))
			{
				NazaraError("failed to create framebuffer for image #{0}: {1}", i, TranslateVulkanError(framebuffer.GetLastErrorCode()));
				return false;
			}

			m_framebuffers.emplace_back(std::move(framebuffer));
		}

		return true;
	}

	bool VulkanSwapchain::SetupRenderPass()
	{
		std::optional<PixelFormat> colorFormat = FromVulkan(m_surfaceFormat.format);
		if (!colorFormat)
		{
			NazaraError("unhandled vulkan pixel format ({0:#x})", UnderlyingCast(m_surfaceFormat.format));
			return false;
		}

		std::optional<PixelFormat> depthStencilFormat;
		if (m_depthStencilFormat != VK_FORMAT_MAX_ENUM)
		{
			depthStencilFormat = FromVulkan(m_depthStencilFormat);
			if (!depthStencilFormat)
			{
				NazaraError("unhandled vulkan pixel format ({0:#x})", UnderlyingCast(m_depthStencilFormat));
				return false;
			}
		}

		std::vector<RenderPass::Attachment> attachments;
		std::vector<RenderPass::SubpassDescription> subpassDescriptions;
		std::vector<RenderPass::SubpassDependency> subpassDependencies;

		BuildRenderPass(*colorFormat, depthStencilFormat.value_or(PixelFormat::Undefined), attachments, subpassDescriptions, subpassDependencies);
		m_renderPass.emplace(m_device, std::move(attachments), std::move(subpassDescriptions), std::move(subpassDependencies));
		return true;
	}

	bool VulkanSwapchain::SetupSurface(WindowHandle windowHandle)
	{
		bool success = false;
#if defined(NAZARA_PLATFORM_WINDOWS)
		{
			NazaraAssertMsg(windowHandle.type == WindowBackend::Windows, "expected Windows window");

			HWND winHandle = static_cast<HWND>(windowHandle.windows.window);
			HINSTANCE instance = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(winHandle, GWLP_HINSTANCE));

			success = m_surface.Create(instance, winHandle);
		}
#elif defined(NAZARA_PLATFORM_LINUX) || defined(NAZARA_PLATFORM_FREEBSD)
		{
			switch (windowHandle.type)
			{
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
				case WindowBackend::Wayland:
				{
					wl_display* display = static_cast<wl_display*>(windowHandle.wayland.display);
					wl_surface* surface = static_cast<wl_surface*>(windowHandle.wayland.surface);

					success = m_surface.Create(display, surface);
					break;
				}
#endif

#ifdef VK_USE_PLATFORM_XLIB_KHR
				case WindowBackend::X11:
				{
					Display* display = static_cast<Display*>(windowHandle.x11.display);
					::Window window = static_cast<::Window>(windowHandle.x11.window);

					success = m_surface.Create(display, window);
					break;
				}
#endif

				default:
				{
					NazaraError("unhandled window type");
					return false;
				}
			}
		}
#elif defined(NAZARA_PLATFORM_MACOS)
		{
			NazaraAssertMsg(windowHandle.type == WindowBackend::Cocoa, "expected cocoa window");
			id layer = CreateAndAttachMetalLayer(windowHandle.cocoa.window);
			success = m_surface.Create(layer);
		}
#else
#error This OS is not supported by Vulkan
#endif

		if (!success)
		{
			NazaraError("failed to create Vulkan surface: {0}", TranslateVulkanError(m_surface.GetLastErrorCode()));
			return false;
		}

		return true;
	}

	bool VulkanSwapchain::SetupSwapchain(const Vk::PhysicalDevice& deviceInfo)
	{
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		if (!m_surface.GetCapabilities(deviceInfo.physDevice, &surfaceCapabilities))
		{
			NazaraError("failed to query surface capabilities");
			return false;
		}

		UInt32 imageCount = surfaceCapabilities.minImageCount + 1;
		if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
			imageCount = surfaceCapabilities.maxImageCount;

		VkExtent2D extent;
		if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF)
		{
			extent.width = std::clamp<UInt32>(m_swapchainSize.x, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
			extent.height = std::clamp<UInt32>(m_swapchainSize.y, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
		}
		else
			extent = surfaceCapabilities.currentExtent;

		VkCompositeAlphaFlagBitsKHR compositeAlpha;
		if (surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
			compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		else
			compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;

		// Ensure all operations on the device have been finished before recreating the swapchain (this can be avoided but is more complicated)
		m_device.WaitForIdle();

		VkSwapchainCreateInfoKHR swapchainInfo = {
			VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			nullptr,
			0,
			m_surface,
			imageCount,
			m_surfaceFormat.format,
			m_surfaceFormat.colorSpace,
			extent,
			1,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			VK_SHARING_MODE_EXCLUSIVE,
			0, nullptr,
			surfaceCapabilities.currentTransform,
			compositeAlpha,
			ToVulkan(m_presentMode),
			VK_TRUE,
			m_swapchain
		};

		Vk::Swapchain newSwapchain;
		if (!newSwapchain.Create(m_device, swapchainInfo))
		{
			NazaraError("failed to create swapchain: {0}", TranslateVulkanError(newSwapchain.GetLastErrorCode()));
			return false;
		}

		m_swapchain = std::move(newSwapchain);
		m_swapchainSize = { SafeCast<unsigned int>(extent.width), SafeCast<unsigned int>(extent.height) };

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
