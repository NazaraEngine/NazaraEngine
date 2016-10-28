// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VKDEVICE_HPP
#define NAZARA_VULKANRENDERER_VKDEVICE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/HandledObject.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Loader.hpp>
#include <vulkan/vulkan.h>
#include <unordered_set>

namespace Nz 
{
	namespace Vk
	{
		class Device;
		class Queue;
		class Instance;

		using DeviceHandle = ObjectHandle<Device>;

		class NAZARA_VULKANRENDERER_API Device : public HandledObject<Device>
		{
			public:
				struct QueueFamilyInfo;
				struct QueueInfo;
				using QueueList = std::vector<QueueInfo>;

				inline Device(Instance& instance);
				Device(const Device&) = delete;
				Device(Device&&) = delete;
				inline ~Device();

				bool Create(VkPhysicalDevice device, const VkDeviceCreateInfo& createInfo, const VkAllocationCallbacks* allocator = nullptr);
				inline void Destroy();

				inline const std::vector<QueueFamilyInfo>& GetEnabledQueues() const;
				inline const QueueList& GetEnabledQueues(UInt32 familyQueue) const;

				inline Queue GetQueue(UInt32 queueFamilyIndex, UInt32 queueIndex);
				inline Instance& GetInstance();
				inline const Instance& GetInstance() const;
				inline VkResult GetLastErrorCode() const;
				inline VkPhysicalDevice GetPhysicalDevice() const;

				inline bool IsExtensionLoaded(const String& extensionName);
				inline bool IsLayerLoaded(const String& layerName);

				inline bool WaitForIdle();

				Device& operator=(const Device&) = delete;
				Device& operator=(Device&&) = delete;

				inline operator VkDevice();

				// Vulkan functions
				#define NAZARA_VULKANRENDERER_DEVICE_FUNCTION(func) PFN_##func func

				// Vulkan core
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkAllocateCommandBuffers);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkAllocateDescriptorSets);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkAllocateMemory);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkBeginCommandBuffer);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkBindBufferMemory);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkBindImageMemory);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdBeginQuery);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdBeginRenderPass);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdBindDescriptorSets);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdBindIndexBuffer);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdBindPipeline);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdBindVertexBuffers);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdBlitImage);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdClearAttachments);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdClearColorImage);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdClearDepthStencilImage);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdCopyBuffer);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdCopyBufferToImage);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdCopyImage);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdCopyImageToBuffer);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdCopyQueryPoolResults);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdDispatch);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdDispatchIndirect);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdDraw);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdDrawIndexed);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdDrawIndexedIndirect);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdDrawIndirect);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdEndQuery);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdEndRenderPass);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdExecuteCommands);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdFillBuffer);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdNextSubpass);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdPipelineBarrier);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdPushConstants);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdResetEvent);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdResetQueryPool);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdResolveImage);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdSetBlendConstants);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdSetDepthBias);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdSetDepthBounds);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdSetEvent);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdSetLineWidth);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdSetScissor);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdSetStencilCompareMask);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdSetStencilReference);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdSetStencilWriteMask);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdSetViewport);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdUpdateBuffer);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdWaitEvents);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCmdWriteTimestamp);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateBuffer);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateBufferView);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateCommandPool);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateComputePipelines);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateDescriptorPool);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateDescriptorSetLayout);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateEvent);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateFramebuffer);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateGraphicsPipelines);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateImage);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateImageView);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreatePipelineCache);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreatePipelineLayout);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateRenderPass);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateSampler);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateSemaphore);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateShaderModule);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyBuffer);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyBufferView);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyCommandPool);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyDescriptorPool);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyDescriptorSetLayout);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyDevice);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyEvent);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyFramebuffer);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyImage);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyImageView);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyPipeline);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyPipelineCache);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyPipelineLayout);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyRenderPass);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroySampler);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroySemaphore);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroyShaderModule);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDeviceWaitIdle);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkEndCommandBuffer);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkFreeCommandBuffers);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkFreeDescriptorSets);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkFreeMemory);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkFlushMappedMemoryRanges);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkGetBufferMemoryRequirements);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkGetDeviceMemoryCommitment);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkGetDeviceQueue);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkGetEventStatus);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkGetFenceStatus);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkGetImageMemoryRequirements);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkGetImageSparseMemoryRequirements);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkGetImageSubresourceLayout);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkGetRenderAreaGranularity);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkInvalidateMappedMemoryRanges);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkMapMemory);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkMergePipelineCaches);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkQueueSubmit);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkQueueWaitIdle);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkResetCommandBuffer);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkResetCommandPool);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkResetDescriptorPool);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkResetFences);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkResetEvent);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkSetEvent);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkUnmapMemory);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkUpdateDescriptorSets);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkWaitForFences);

				// VK_KHR_display_swapchain
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateSharedSwapchainsKHR);

				// VK_KHR_surface
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroySurfaceKHR);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkGetPhysicalDeviceSurfacePresentModesKHR);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR);

				// VK_KHR_swapchain
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkAcquireNextImageKHR);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkCreateSwapchainKHR);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkDestroySwapchainKHR);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkGetSwapchainImagesKHR);
				NAZARA_VULKANRENDERER_DEVICE_FUNCTION(vkQueuePresentKHR);

				#undef NAZARA_VULKANRENDERER_DEVICE_FUNCTION

				struct QueueInfo
				{
					QueueFamilyInfo* familyInfo;
					VkQueue queue;
					float priority;
				};

				struct QueueFamilyInfo
				{
					QueueList queues;
					VkExtent3D minImageTransferGranularity;
					VkQueueFlags flags;
					UInt32 familyIndex;
					UInt32 timestampValidBits;
				};

			private:
				inline PFN_vkVoidFunction GetProcAddr(const char* name);

				Instance& m_instance;
				VkAllocationCallbacks m_allocator;
				VkDevice m_device;
				VkPhysicalDevice m_physicalDevice;
				VkResult m_lastErrorCode;
				std::unordered_set<String> m_loadedExtensions;
				std::unordered_set<String> m_loadedLayers;
				std::vector<QueueFamilyInfo> m_enabledQueuesInfos;
				std::vector<const QueueList*> m_queuesByFamily;
		};
	}
}

#include <Nazara/VulkanRenderer/Wrapper/Device.inl>

#endif // NAZARA_VULKANRENDERER_VKDEVICE_HPP
