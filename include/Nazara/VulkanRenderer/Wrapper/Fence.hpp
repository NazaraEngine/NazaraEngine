// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_WRAPPER_FENCE_HPP
#define NAZARA_VULKANRENDERER_WRAPPER_FENCE_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/VulkanRenderer/Wrapper/DeviceObject.hpp>

namespace Nz::Vk
{
	class Fence : public DeviceObject<Fence, VkFence, VkFenceCreateInfo, VK_OBJECT_TYPE_FENCE>
	{
		friend DeviceObject;

		public:
			Fence() = default;
			Fence(const Fence&) = delete;
			Fence(Fence&&) = default;
			~Fence() = default;

			using DeviceObject::Create;
			inline bool Create(Device& device, VkFenceCreateFlags flags = 0, const VkAllocationCallbacks* allocator = nullptr);

			inline bool Reset();

			inline bool Wait();
			inline bool Wait(UInt64 timeout, bool* didTimeout = nullptr);

			Fence& operator=(const Fence&) = delete;
			Fence& operator=(Fence&&) = delete;

		private:
			static inline VkResult CreateHelper(Device& device, const VkFenceCreateInfo* createInfo, const VkAllocationCallbacks* allocator, VkFence* handle);
			static inline void DestroyHelper(Device& device, VkFence handle, const VkAllocationCallbacks* allocator);
	};
}

#include <Nazara/VulkanRenderer/Wrapper/Fence.inl>

#endif // NAZARA_VULKANRENDERER_WRAPPER_FENCE_HPP
