// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_WRAPPER_DEVICEMEMORY_HPP
#define NAZARA_VULKANRENDERER_WRAPPER_DEVICEMEMORY_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DeviceObject.hpp>
#include <NazaraUtils/MovablePtr.hpp>

namespace Nz::Vk
{
	class DeviceMemory : public DeviceObject<DeviceMemory, VkDeviceMemory, VkMemoryAllocateInfo, VK_OBJECT_TYPE_DEVICE_MEMORY>
	{
		friend DeviceObject;

		public:
			DeviceMemory() = default;
			DeviceMemory(const DeviceMemory&) = delete;
			DeviceMemory(DeviceMemory&& memory) noexcept = default;
			~DeviceMemory() = default;

			using DeviceObject::Create;
			inline bool Create(Device& device, VkDeviceSize size, UInt32 memoryType, const VkAllocationCallbacks* allocator = nullptr);
			inline bool Create(Device& device, VkDeviceSize size, UInt32 typeBits, VkFlags properties, const VkAllocationCallbacks* allocator = nullptr);

			inline bool FlushMemory();
			inline bool FlushMemory(UInt64 offset, UInt64 size);

			inline void* GetMappedPointer();

			inline bool Map(VkMemoryMapFlags flags = 0);
			inline bool Map(VkDeviceSize offset, VkDeviceSize size, VkMemoryMapFlags flags = 0);

			inline void Unmap();

			DeviceMemory& operator=(const DeviceMemory&) = delete;
			DeviceMemory& operator=(DeviceMemory&&) = delete;

		private:
			static inline VkResult CreateHelper(Device& device, const VkMemoryAllocateInfo* allocInfo, const VkAllocationCallbacks* allocator, VkDeviceMemory* handle);
			static inline void DestroyHelper(Device& device, VkDeviceMemory handle, const VkAllocationCallbacks* allocator);

			MovablePtr<void> m_mappedPtr;
	};
}

#include <Nazara/VulkanRenderer/Wrapper/DeviceMemory.inl>

#endif // NAZARA_VULKANRENDERER_WRAPPER_DEVICEMEMORY_HPP
