// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Vulkan/VkDevice.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Vulkan/Debug.hpp>

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
			#define NAZARA_VULKAN_LOAD_DEVICE(func) func = reinterpret_cast<PFN_##func>(GetProcAddr(#func))

			try
			{
				ErrorFlags flags(ErrorFlag_ThrowException, true);

				NAZARA_VULKAN_LOAD_DEVICE(vkAllocateCommandBuffers);
				NAZARA_VULKAN_LOAD_DEVICE(vkAllocateDescriptorSets);
				NAZARA_VULKAN_LOAD_DEVICE(vkAllocateMemory);
				NAZARA_VULKAN_LOAD_DEVICE(vkBeginCommandBuffer);
				NAZARA_VULKAN_LOAD_DEVICE(vkBindBufferMemory);
				NAZARA_VULKAN_LOAD_DEVICE(vkBindImageMemory);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdBeginQuery);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdBeginRenderPass);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdBindDescriptorSets);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdBindIndexBuffer);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdBindPipeline);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdBindVertexBuffers);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdBlitImage);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdClearAttachments);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdClearColorImage);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdClearDepthStencilImage);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdCopyBuffer);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdCopyBufferToImage);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdCopyImage);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdCopyImageToBuffer);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdCopyQueryPoolResults);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdDispatch);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdDispatchIndirect);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdDraw);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdDrawIndexed);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdDrawIndexedIndirect);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdDrawIndirect);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdEndQuery);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdEndRenderPass);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdExecuteCommands);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdFillBuffer);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdNextSubpass);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdPipelineBarrier);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdPushConstants);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdResetEvent);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdResetQueryPool);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdResolveImage);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdSetBlendConstants);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdSetDepthBias);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdSetDepthBounds);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdSetEvent);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdSetLineWidth);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdSetScissor);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdSetStencilCompareMask);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdSetStencilReference);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdSetStencilWriteMask);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdSetViewport);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdUpdateBuffer);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdWaitEvents);
				NAZARA_VULKAN_LOAD_DEVICE(vkCmdWriteTimestamp);
				NAZARA_VULKAN_LOAD_DEVICE(vkCreateBuffer);
				NAZARA_VULKAN_LOAD_DEVICE(vkCreateBufferView);
				NAZARA_VULKAN_LOAD_DEVICE(vkCreateCommandPool);
				NAZARA_VULKAN_LOAD_DEVICE(vkCreateComputePipelines);
				NAZARA_VULKAN_LOAD_DEVICE(vkCreateDescriptorPool);
				NAZARA_VULKAN_LOAD_DEVICE(vkCreateDescriptorSetLayout);
				NAZARA_VULKAN_LOAD_DEVICE(vkCreateEvent);
				NAZARA_VULKAN_LOAD_DEVICE(vkCreateFramebuffer);
				NAZARA_VULKAN_LOAD_DEVICE(vkCreateGraphicsPipelines);
				NAZARA_VULKAN_LOAD_DEVICE(vkCreateImage);
				NAZARA_VULKAN_LOAD_DEVICE(vkCreateImageView);
				NAZARA_VULKAN_LOAD_DEVICE(vkCreatePipelineCache);
				NAZARA_VULKAN_LOAD_DEVICE(vkCreatePipelineLayout);
				NAZARA_VULKAN_LOAD_DEVICE(vkCreateRenderPass);
				NAZARA_VULKAN_LOAD_DEVICE(vkCreateSampler);
				NAZARA_VULKAN_LOAD_DEVICE(vkCreateSemaphore);
				NAZARA_VULKAN_LOAD_DEVICE(vkCreateShaderModule);
				NAZARA_VULKAN_LOAD_DEVICE(vkDestroyBuffer);
				NAZARA_VULKAN_LOAD_DEVICE(vkDestroyBufferView);
				NAZARA_VULKAN_LOAD_DEVICE(vkDestroyCommandPool);
				NAZARA_VULKAN_LOAD_DEVICE(vkDestroyDescriptorPool);
				NAZARA_VULKAN_LOAD_DEVICE(vkDestroyDescriptorSetLayout);
				NAZARA_VULKAN_LOAD_DEVICE(vkDestroyDevice);
				NAZARA_VULKAN_LOAD_DEVICE(vkDestroyEvent);
				NAZARA_VULKAN_LOAD_DEVICE(vkDestroyFramebuffer);
				NAZARA_VULKAN_LOAD_DEVICE(vkDestroyImage);
				NAZARA_VULKAN_LOAD_DEVICE(vkDestroyImageView);
				NAZARA_VULKAN_LOAD_DEVICE(vkDestroyPipeline);
				NAZARA_VULKAN_LOAD_DEVICE(vkDestroyPipelineCache);
				NAZARA_VULKAN_LOAD_DEVICE(vkDestroyPipelineLayout);
				NAZARA_VULKAN_LOAD_DEVICE(vkDestroyRenderPass);
				NAZARA_VULKAN_LOAD_DEVICE(vkDestroySampler);
				NAZARA_VULKAN_LOAD_DEVICE(vkDestroySemaphore);
				NAZARA_VULKAN_LOAD_DEVICE(vkDestroyShaderModule);
				NAZARA_VULKAN_LOAD_DEVICE(vkDeviceWaitIdle);
				NAZARA_VULKAN_LOAD_DEVICE(vkEndCommandBuffer);
				NAZARA_VULKAN_LOAD_DEVICE(vkFreeCommandBuffers);
				NAZARA_VULKAN_LOAD_DEVICE(vkFreeDescriptorSets);
				NAZARA_VULKAN_LOAD_DEVICE(vkFreeMemory);
				NAZARA_VULKAN_LOAD_DEVICE(vkFlushMappedMemoryRanges);
				NAZARA_VULKAN_LOAD_DEVICE(vkGetBufferMemoryRequirements);
				NAZARA_VULKAN_LOAD_DEVICE(vkGetDeviceMemoryCommitment);
				NAZARA_VULKAN_LOAD_DEVICE(vkGetDeviceQueue);
				NAZARA_VULKAN_LOAD_DEVICE(vkGetEventStatus);
				NAZARA_VULKAN_LOAD_DEVICE(vkGetFenceStatus);
				NAZARA_VULKAN_LOAD_DEVICE(vkGetImageMemoryRequirements);
				NAZARA_VULKAN_LOAD_DEVICE(vkGetImageSparseMemoryRequirements);
				NAZARA_VULKAN_LOAD_DEVICE(vkGetImageSubresourceLayout);
				NAZARA_VULKAN_LOAD_DEVICE(vkGetRenderAreaGranularity);
				NAZARA_VULKAN_LOAD_DEVICE(vkInvalidateMappedMemoryRanges);
				NAZARA_VULKAN_LOAD_DEVICE(vkMapMemory);
				NAZARA_VULKAN_LOAD_DEVICE(vkMergePipelineCaches);
				NAZARA_VULKAN_LOAD_DEVICE(vkQueueSubmit);
				NAZARA_VULKAN_LOAD_DEVICE(vkQueueWaitIdle);
				NAZARA_VULKAN_LOAD_DEVICE(vkResetCommandBuffer);
				NAZARA_VULKAN_LOAD_DEVICE(vkResetCommandPool);
				NAZARA_VULKAN_LOAD_DEVICE(vkResetDescriptorPool);
				NAZARA_VULKAN_LOAD_DEVICE(vkResetFences);
				NAZARA_VULKAN_LOAD_DEVICE(vkResetEvent);
				NAZARA_VULKAN_LOAD_DEVICE(vkSetEvent);
				NAZARA_VULKAN_LOAD_DEVICE(vkUnmapMemory);
				NAZARA_VULKAN_LOAD_DEVICE(vkUpdateDescriptorSets);
				NAZARA_VULKAN_LOAD_DEVICE(vkWaitForFences);

				// VK_KHR_display_swapchain
				if (IsExtensionLoaded("VK_KHR_display_swapchain"))
					NAZARA_VULKAN_LOAD_DEVICE(vkCreateSharedSwapchainsKHR);

				// VK_KHR_swapchain
				if (IsExtensionLoaded("VK_KHR_swapchain"))
				{
					NAZARA_VULKAN_LOAD_DEVICE(vkAcquireNextImageKHR);
					NAZARA_VULKAN_LOAD_DEVICE(vkCreateSwapchainKHR);
					NAZARA_VULKAN_LOAD_DEVICE(vkDestroySwapchainKHR);
					NAZARA_VULKAN_LOAD_DEVICE(vkGetSwapchainImagesKHR);
					NAZARA_VULKAN_LOAD_DEVICE(vkQueuePresentKHR);
				}
			}
			catch (const std::exception& e)
			{
				NazaraError(String("Failed to query device function: ") + e.what());
				return false;
			}

			#undef NAZARA_VULKAN_LOAD_DEVICE

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
