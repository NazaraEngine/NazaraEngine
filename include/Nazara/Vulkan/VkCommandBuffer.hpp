// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKAN_VKCOMMANDBUFFER_HPP
#define NAZARA_VULKAN_VKCOMMANDBUFFER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Vulkan/VkCommandPool.hpp>
#include <vulkan/vulkan.h>

namespace Nz 
{
	namespace Vk
	{
		class CommandBuffer
		{
			friend CommandPool;

			public:
				inline CommandBuffer();
				CommandBuffer(const CommandBuffer&) = delete;
				inline CommandBuffer(CommandBuffer&& commandBuffer);
				inline ~CommandBuffer();

				inline bool Begin(const VkCommandBufferBeginInfo& info);
				inline bool Begin(VkCommandBufferUsageFlags flags);
				inline bool Begin(VkCommandBufferUsageFlags flags, const VkCommandBufferInheritanceInfo& inheritanceInfo);
				inline bool Begin(VkCommandBufferUsageFlags flags, VkRenderPass renderPass, UInt32 subpass, VkFramebuffer framebuffer, bool occlusionQueryEnable, VkQueryControlFlags queryFlags, VkQueryPipelineStatisticFlags pipelineStatistics);
				inline bool Begin(VkCommandBufferUsageFlags flags, bool occlusionQueryEnable, VkQueryControlFlags queryFlags, VkQueryPipelineStatisticFlags pipelineStatistics);

				inline bool End();

				inline void Free();

				inline void PipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, const VkImageMemoryBarrier& imageMemoryBarrier);
				inline void PipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, const VkMemoryBarrier& memoryBarrier, const VkBufferMemoryBarrier& bufferMemoryBarrier, const VkImageMemoryBarrier& imageMemoryBarrier);
				inline void PipelineBarrier(VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkDependencyFlags dependencyFlags, UInt32 memoryBarrierCount, const VkMemoryBarrier* memoryBarriers, UInt32 bufferMemoryBarrierCount, const VkBufferMemoryBarrier* bufferMemoryBarriers, UInt32 imageMemoryBarrierCount, const VkImageMemoryBarrier* imageMemoryBarriers);

				inline VkResult GetLastErrorCode() const;

				CommandBuffer& operator=(const CommandBuffer&) = delete;
				CommandBuffer& operator=(CommandBuffer&& commandBuffer);

				inline operator VkCommandBuffer() const;

			private:
				inline CommandBuffer(CommandPool& pool, VkCommandBuffer handle);

				CommandPoolHandle m_pool;
				VkAllocationCallbacks m_allocator;
				VkCommandBuffer m_handle;
				VkResult m_lastErrorCode;

		};
	}
}

#include <Nazara/Vulkan/VkCommandBuffer.inl>

#endif // NAZARA_VULKAN_VKCOMMANDBUFFER_HPP
