// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_BUFFER_HPP
#define NAZARA_VULKANRENDERER_BUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Utility/AbstractBuffer.hpp>
#include <Nazara/VulkanRenderer/Config.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Buffer.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DeviceMemory.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Fence.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class NAZARA_VULKANRENDERER_API VulkanBuffer : public AbstractBuffer
	{
		public:
			inline VulkanBuffer(Vk::Device& device, BufferType type);
			VulkanBuffer(const VulkanBuffer&) = delete;
			VulkanBuffer(VulkanBuffer&&) = delete; ///TODO
			virtual ~VulkanBuffer();

			bool Fill(const void* data, UInt64 offset, UInt64 size) override;

			bool Initialize(UInt64 size, BufferUsageFlags usage) override;

			inline VkBuffer GetBuffer() const;
			UInt64 GetSize() const override;
			DataStorage GetStorage() const override;

			void* Map(BufferAccess access, UInt64 offset, UInt64 size) override;
			bool Unmap() override;

			VulkanBuffer& operator=(const VulkanBuffer&) = delete;
			VulkanBuffer& operator=(VulkanBuffer&&) = delete; ///TODO

		private:
			BufferType m_type;
			BufferUsageFlags m_usage;
			UInt64 m_size;
			VkBuffer m_buffer;
			VkBuffer m_stagingBuffer;
			VmaAllocation m_allocation;
			VmaAllocation m_stagingAllocation;
			Vk::Device& m_device;
	};
}

#include <Nazara/VulkanRenderer/VulkanBuffer.inl>

#endif // NAZARA_VULKANRENDERER_BUFFER_HPP
