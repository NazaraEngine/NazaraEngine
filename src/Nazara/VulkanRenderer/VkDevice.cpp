// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/VkDevice.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		bool Device::Create(VkPhysicalDevice device, const VkDeviceCreateInfo& createInfo, const VkAllocationCallbacks* allocator)
		{
			std::vector<VkQueueFamilyProperties> queuesProperties;
			if (!m_instance.GetPhysicalDeviceQueueFamilyProperties(device, &queuesProperties))
			{
				NazaraError("Failed to query queue family properties");
				return false;
			}

			m_lastErrorCode = m_instance.vkCreateDevice(device, &createInfo, allocator, &m_device);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to create Vulkan device");
				return false;
			}

			m_physicalDevice = device;

			// Store the allocator to access them when needed
			if (allocator)
				m_allocator = *allocator;
			else
				m_allocator.pfnAllocation = nullptr;

			// Parse extensions and layers
			for (UInt32 i = 0; i < createInfo.enabledExtensionCount; ++i)
				m_loadedExtensions.insert(createInfo.ppEnabledExtensionNames[i]);

			for (UInt32 i = 0; i < createInfo.enabledLayerCount; ++i)
				m_loadedLayers.insert(createInfo.ppEnabledLayerNames[i]);

			// Load all device-related functions
			#define NAZARA_VULKANRENDERER_LOAD_DEVICE(func) func = reinterpret_cast<PFN_##func>(GetProcAddr(#func))

			try
			{
				ErrorFlags flags(ErrorFlag_ThrowException, true);

				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkAllocateCommandBuffers);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkAllocateDescriptorSets);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkAllocateMemory);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkBeginCommandBuffer);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkBindBufferMemory);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkBindImageMemory);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdBeginQuery);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdBeginRenderPass);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdBindDescriptorSets);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdBindIndexBuffer);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdBindPipeline);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdBindVertexBuffers);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdBlitImage);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdClearAttachments);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdClearColorImage);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdClearDepthStencilImage);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdCopyBuffer);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdCopyBufferToImage);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdCopyImage);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdCopyImageToBuffer);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdCopyQueryPoolResults);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdDispatch);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdDispatchIndirect);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdDraw);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdDrawIndexed);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdDrawIndexedIndirect);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdDrawIndirect);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdEndQuery);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdEndRenderPass);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdExecuteCommands);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdFillBuffer);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdNextSubpass);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdPipelineBarrier);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdPushConstants);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdResetEvent);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdResetQueryPool);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdResolveImage);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdSetBlendConstants);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdSetDepthBias);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdSetDepthBounds);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdSetEvent);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdSetLineWidth);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdSetScissor);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdSetStencilCompareMask);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdSetStencilReference);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdSetStencilWriteMask);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdSetViewport);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdUpdateBuffer);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdWaitEvents);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCmdWriteTimestamp);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCreateBuffer);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCreateBufferView);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCreateCommandPool);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCreateComputePipelines);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCreateDescriptorPool);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCreateDescriptorSetLayout);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCreateEvent);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCreateFramebuffer);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCreateGraphicsPipelines);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCreateImage);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCreateImageView);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCreatePipelineCache);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCreatePipelineLayout);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCreateRenderPass);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCreateSampler);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCreateSemaphore);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCreateShaderModule);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkDestroyBuffer);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkDestroyBufferView);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkDestroyCommandPool);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkDestroyDescriptorPool);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkDestroyDescriptorSetLayout);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkDestroyDevice);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkDestroyEvent);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkDestroyFramebuffer);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkDestroyImage);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkDestroyImageView);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkDestroyPipeline);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkDestroyPipelineCache);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkDestroyPipelineLayout);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkDestroyRenderPass);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkDestroySampler);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkDestroySemaphore);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkDestroyShaderModule);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkDeviceWaitIdle);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkEndCommandBuffer);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkFreeCommandBuffers);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkFreeDescriptorSets);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkFreeMemory);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkFlushMappedMemoryRanges);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkGetBufferMemoryRequirements);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkGetDeviceMemoryCommitment);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkGetDeviceQueue);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkGetEventStatus);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkGetFenceStatus);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkGetImageMemoryRequirements);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkGetImageSparseMemoryRequirements);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkGetImageSubresourceLayout);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkGetRenderAreaGranularity);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkInvalidateMappedMemoryRanges);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkMapMemory);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkMergePipelineCaches);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkQueueSubmit);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkQueueWaitIdle);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkResetCommandBuffer);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkResetCommandPool);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkResetDescriptorPool);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkResetFences);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkResetEvent);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkSetEvent);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkUnmapMemory);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkUpdateDescriptorSets);
				NAZARA_VULKANRENDERER_LOAD_DEVICE(vkWaitForFences);

				// VK_KHR_display_swapchain
				if (IsExtensionLoaded("VK_KHR_display_swapchain"))
					NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCreateSharedSwapchainsKHR);

				// VK_KHR_swapchain
				if (IsExtensionLoaded("VK_KHR_swapchain"))
				{
					NAZARA_VULKANRENDERER_LOAD_DEVICE(vkAcquireNextImageKHR);
					NAZARA_VULKANRENDERER_LOAD_DEVICE(vkCreateSwapchainKHR);
					NAZARA_VULKANRENDERER_LOAD_DEVICE(vkDestroySwapchainKHR);
					NAZARA_VULKANRENDERER_LOAD_DEVICE(vkGetSwapchainImagesKHR);
					NAZARA_VULKANRENDERER_LOAD_DEVICE(vkQueuePresentKHR);
				}
			}
			catch (const std::exception& e)
			{
				NazaraError(String("Failed to query device function: ") + e.what());
				return false;
			}

			#undef NAZARA_VULKANRENDERER_LOAD_DEVICE

			// And retains informations about queues
			UInt32 maxFamilyIndex = 0;
			m_enabledQueuesInfos.resize(createInfo.queueCreateInfoCount);
			for (UInt32 i = 0; i < createInfo.queueCreateInfoCount; ++i)
			{
				const VkDeviceQueueCreateInfo& queueCreateInfo = createInfo.pQueueCreateInfos[i];
				QueueFamilyInfo& info = m_enabledQueuesInfos[i];

				info.familyIndex = queueCreateInfo.queueFamilyIndex;
				if (info.familyIndex > maxFamilyIndex)
					maxFamilyIndex = info.familyIndex;

				const VkQueueFamilyProperties& queueProperties = queuesProperties[info.familyIndex];
				info.flags = queueProperties.queueFlags;
				info.minImageTransferGranularity = queueProperties.minImageTransferGranularity;
				info.timestampValidBits = queueProperties.timestampValidBits;

				info.queues.resize(queueCreateInfo.queueCount);
				for (UInt32 queueIndex = 0; queueIndex < queueCreateInfo.queueCount; ++queueIndex)
				{
					QueueInfo& queueInfo = info.queues[queueIndex];
					queueInfo.familyInfo = &info;
					queueInfo.priority = queueCreateInfo.pQueuePriorities[queueIndex];
					vkGetDeviceQueue(m_device, info.familyIndex, queueIndex, &queueInfo.queue);
				}
			}

			m_queuesByFamily.resize(maxFamilyIndex + 1);
			for (const QueueFamilyInfo& familyInfo : m_enabledQueuesInfos)
				m_queuesByFamily[familyInfo.familyIndex] = &familyInfo.queues;

			return true;
		}
	}
}
