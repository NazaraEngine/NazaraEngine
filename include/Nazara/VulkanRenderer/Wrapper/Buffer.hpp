// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_WRAPPER_BUFFER_HPP
#define NAZARA_VULKANRENDERER_WRAPPER_BUFFER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DeviceObject.hpp>

namespace Nz
{
	namespace Vk
	{
		class Buffer : public DeviceObject<Buffer, VkBuffer, VkBufferCreateInfo, VK_OBJECT_TYPE_BUFFER>
		{
			friend DeviceObject;

			public:
				Buffer() = default;
				Buffer(const Buffer&) = delete;
				Buffer(Buffer&&) noexcept = default;
				~Buffer() = default;

				bool BindBufferMemory(VkDeviceMemory memory, VkDeviceSize offset = 0);

				using DeviceObject::Create;
				inline bool Create(Device& device, VkBufferCreateFlags flags, VkDeviceSize size, VkBufferUsageFlags usage, const VkAllocationCallbacks* allocator = nullptr);

				VkMemoryRequirements GetMemoryRequirements() const;

				Buffer& operator=(const Buffer&) = delete;
				Buffer& operator=(Buffer&&) = delete;

			private:
				static inline VkResult CreateHelper(Device& device, const VkBufferCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkBuffer* handle);
				static inline void DestroyHelper(Device& device, VkBuffer handle, const VkAllocationCallbacks* allocator);
		};
	}
}

#include <Nazara/VulkanRenderer/Wrapper/Buffer.inl>

#endif // NAZARA_VULKANRENDERER_WRAPPER_BUFFER_HPP
