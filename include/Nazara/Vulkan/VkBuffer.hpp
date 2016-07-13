// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VULKAN_VKBUFFER_HPP
#define NAZARA_VULKAN_VKBUFFER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Vulkan/VkDeviceObject.hpp>

namespace Nz 
{
	namespace Vk
	{
		class Buffer : public DeviceObject<Buffer, VkBuffer, VkBufferCreateInfo>
		{
			friend DeviceObject;

			public:
				Buffer() = default;
				Buffer(const Buffer&) = delete;
				Buffer(Buffer&&) = default;
				~Buffer() = default;

				using DeviceObject::Create;
				inline bool Create(const DeviceHandle& device, VkDeviceSize size, UInt32 memoryType, const VkAllocationCallbacks* allocator = nullptr);

				VkMemoryRequirements GetMemoryRequirements() const;

				Buffer& operator=(const Buffer&) = delete;
				Buffer& operator=(Buffer&&) = delete;

			private:
				static inline VkResult CreateHelper(const DeviceHandle& device, const VkBufferCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkBuffer* handle);
				static inline void DestroyHelper(const DeviceHandle& device, VkBuffer handle, const VkAllocationCallbacks* allocator);
		};
	}
}

#include <Nazara/Vulkan/VkBuffer.inl>

#endif // NAZARA_VULKAN_VKBUFFER_HPP
