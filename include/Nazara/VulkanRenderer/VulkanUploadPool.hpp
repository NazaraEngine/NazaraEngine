// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANUPLOADPOOL_HPP
#define NAZARA_VULKANRENDERER_VULKANUPLOADPOOL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Buffer.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DeviceMemory.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <optional>
#include <vector>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanUploadPool : public UploadPool
	{
		public:
			struct VulkanAllocation : Allocation
			{
				VkBuffer buffer;
				UInt64 offset;
			};

			inline VulkanUploadPool(Vk::Device& device, UInt64 blockSize);
			VulkanUploadPool(const VulkanUploadPool&) = delete;
			VulkanUploadPool(VulkanUploadPool&&) noexcept = default;
			~VulkanUploadPool() = default;

			VulkanAllocation& Allocate(UInt64 size) override;
			VulkanAllocation& Allocate(UInt64 size, UInt64 alignment) override;

			void Reset() override;

			VulkanUploadPool& operator=(const VulkanUploadPool&) = delete;
			VulkanUploadPool& operator=(VulkanUploadPool&&) = delete;

		private:
			static constexpr std::size_t AllocationPerBlock = 2048;

			using AllocationBlock = std::array<VulkanAllocation, AllocationPerBlock>;

			struct Block
			{
				Vk::DeviceMemory blockMemory;
				Vk::Buffer buffer;
				UInt64 freeOffset = 0;
				UInt64 size;
			};

			UInt64 m_blockSize;
			Vk::Device& m_device;
			std::size_t m_nextAllocationIndex;
			std::vector<std::unique_ptr<AllocationBlock>> m_allocationBlocks;
			std::vector<Block> m_blocks;
	};
}

#include <Nazara/VulkanRenderer/VulkanUploadPool.inl>

#endif // NAZARA_VULKANRENDERER_VULKANUPLOADPOOL_HPP
