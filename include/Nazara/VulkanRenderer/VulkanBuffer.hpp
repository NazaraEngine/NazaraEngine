// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_VULKANBUFFER_HPP
#define NAZARA_VULKANRENDERER_VULKANBUFFER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/RenderBuffer.hpp>
#include <Nazara/VulkanRenderer/Export.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Buffer.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DeviceMemory.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Fence.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanBuffer : public RenderBuffer
	{
		public:
			VulkanBuffer(VulkanDevice& device, BufferType type, UInt64 size, BufferUsageFlags usage, const void* initialData = nullptr);
			VulkanBuffer(const VulkanBuffer&) = delete;
			VulkanBuffer(VulkanBuffer&&) = delete; ///TODO
			virtual ~VulkanBuffer();

			bool Fill(const void* data, UInt64 offset, UInt64 size) override;

			inline VkBuffer GetBuffer() const;

			void* Map(UInt64 offset, UInt64 size) override;
			bool Unmap() override;

			void UpdateDebugName(std::string_view name) override;

			VulkanBuffer& operator=(const VulkanBuffer&) = delete;
			VulkanBuffer& operator=(VulkanBuffer&&) = delete; ///TODO

		private:
			VkBuffer m_buffer;
			VkBuffer m_stagingBuffer;
			VmaAllocation m_allocation;
			VmaAllocation m_stagingAllocation;
			UInt64 m_stagingBufferSize;
			Vk::Device& m_device;
	};
}

#include <Nazara/VulkanRenderer/VulkanBuffer.inl>

#endif // NAZARA_VULKANRENDERER_VULKANBUFFER_HPP
