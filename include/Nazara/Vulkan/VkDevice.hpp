// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKAN_VKDEVICE_HPP
#define NAZARA_VULKAN_VKDEVICE_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/HandledObject.hpp>
#include <Nazara/Vulkan/Config.hpp>
#include <Nazara/Vulkan/VkLoader.hpp>
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

		class NAZARA_VULKAN_API Device : public HandledObject<Device>
		{
			public:
				struct QueueFamilyInfo;
				struct QueueInfo;

				inline Device(Instance& instance);
				Device(const Device&) = delete;
				Device(Device&&) = delete;
				inline ~Device();

				bool Create(VkPhysicalDevice device, const VkDeviceCreateInfo& createInfo, const VkAllocationCallbacks* allocator = nullptr);
				inline void Destroy();

				inline const std::vector<QueueFamilyInfo>& GetEnabledQueues() const;

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
				#define NAZARA_VULKAN_DEVICE_FUNCTION(func) PFN_##func func

				// Vulkan core
				NAZARA_VULKAN_DEVICE_FUNCTION(vkAllocateCommandBuffers);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkAllocateMemory);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkBeginCommandBuffer);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkBindBufferMemory);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkBindImageMemory);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdBeginQuery);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdBeginRenderPass);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdBindDescriptorSets);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdBindIndexBuffer);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdBindPipeline);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdBindVertexBuffers);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdBlitImage);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdClearAttachments);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdClearColorImage);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdClearDepthStencilImage);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdCopyBuffer);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdCopyBufferToImage);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdCopyImage);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdCopyImageToBuffer);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdCopyQueryPoolResults);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdDispatch);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdDispatchIndirect);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdDraw);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdDrawIndexed);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdDrawIndexedIndirect);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdDrawIndirect);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdEndQuery);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdEndRenderPass);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdExecuteCommands);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdFillBuffer);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdNextSubpass);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdPipelineBarrier);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdPushConstants);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdResetEvent);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdResetQueryPool);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdResolveImage);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdSetBlendConstants);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdSetDepthBias);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdSetDepthBounds);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdSetEvent);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdSetLineWidth);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdSetScissor);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdSetStencilCompareMask);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdSetStencilReference);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdSetStencilWriteMask);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdSetViewport);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdUpdateBuffer);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdWaitEvents);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCmdWriteTimestamp);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCreateBuffer);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCreateBufferView);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCreateCommandPool);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCreateComputePipelines);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCreateDescriptorPool);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCreateDescriptorSetLayout);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCreateEvent);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCreateFramebuffer);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCreateGraphicsPipelines);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCreateImage);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCreateImageView);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCreatePipelineCache);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCreatePipelineLayout);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCreateRenderPass);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCreateSampler);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCreateSemaphore);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCreateShaderModule);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkDestroyBuffer);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkDestroyBufferView);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkDestroyCommandPool);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkDestroyDescriptorPool);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkDestroyDescriptorSetLayout);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkDestroyDevice);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkDestroyEvent);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkDestroyFramebuffer);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkDestroyImage);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkDestroyImageView);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkDestroyPipeline);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkDestroyPipelineCache);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkDestroyPipelineLayout);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkDestroyRenderPass);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkDestroySampler);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkDestroySemaphore);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkDestroyShaderModule);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkDeviceWaitIdle);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkEndCommandBuffer);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkFreeCommandBuffers);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkFreeDescriptorSets);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkFreeMemory);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkFlushMappedMemoryRanges);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkGetBufferMemoryRequirements);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkGetDeviceMemoryCommitment);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkGetDeviceQueue);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkGetEventStatus);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkGetFenceStatus);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkGetImageMemoryRequirements);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkGetImageSparseMemoryRequirements);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkGetImageSubresourceLayout);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkGetRenderAreaGranularity);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkInvalidateMappedMemoryRanges);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkMapMemory);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkMergePipelineCaches);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkQueueSubmit);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkQueueWaitIdle);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkResetCommandBuffer);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkResetCommandPool);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkResetDescriptorPool);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkResetFences);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkResetEvent);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkSetEvent);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkUnmapMemory);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkUpdateDescriptorSets);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkWaitForFences);

				// VK_KHR_display_swapchain
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCreateSharedSwapchainsKHR);

				// VK_KHR_surface
				NAZARA_VULKAN_DEVICE_FUNCTION(vkDestroySurfaceKHR);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkGetPhysicalDeviceSurfacePresentModesKHR);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR);

				// VK_KHR_swapchain
				NAZARA_VULKAN_DEVICE_FUNCTION(vkAcquireNextImageKHR);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkCreateSwapchainKHR);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkDestroySwapchainKHR);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkGetSwapchainImagesKHR);
				NAZARA_VULKAN_DEVICE_FUNCTION(vkQueuePresentKHR);

				#undef NAZARA_VULKAN_DEVICE_FUNCTION

				struct QueueInfo
				{
					QueueFamilyInfo* familyInfo;
					Queue queue;
					float priority;
				};

				using QueueList = std::vector<QueueInfo>;

				struct QueueFamilyInfoi 
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
		};
	}
}

#include <Nazara/Vulkan/VkDevice.inl>

#endif // NAZARA_VULKAN_VKDEVICE_HPP
