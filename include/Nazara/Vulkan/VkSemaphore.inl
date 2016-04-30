// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Vulkan/VkSemaphore.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Vulkan/VkDevice.hpp>
#include <Nazara/Vulkan/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		inline Semaphore::Semaphore(Device& device) :
		m_device(device),
		m_semaphore(VK_NULL_HANDLE)
		{
		}

		inline Semaphore::~Semaphore()
		{
			Destroy();
		}

		inline bool Semaphore::Create(const VkSemaphoreCreateInfo& createInfo, const VkAllocationCallbacks* allocator)
		{
			m_lastErrorCode = m_device.vkCreateSemaphore(m_device, &createInfo, allocator, &m_semaphore);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to create Vulkan semaphore");
				return false;
			}

			// Store the allocator to access them when needed
			if (allocator)
				m_allocator = *allocator;
			else
				m_allocator.pfnAllocation = nullptr;

			return true;
		}

		inline bool Semaphore::Create(VkSemaphoreCreateFlags flags, const VkAllocationCallbacks* allocator)
		{
			VkSemaphoreCreateInfo createInfo =
			{
				VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
				nullptr,
				flags
			};

			return Create(createInfo, allocator);
		}

		inline void Semaphore::Destroy()
		{
			if (m_semaphore != VK_NULL_HANDLE)
				m_device.vkDestroySemaphore(m_device, m_semaphore, (m_allocator.pfnAllocation) ? &m_allocator : nullptr);
		}

		inline VkResult Semaphore::GetLastErrorCode() const
		{
			return m_lastErrorCode;
		}

		inline Semaphore::operator VkSemaphore()
		{
			return m_semaphore;
		}
	}
}

#include <Nazara/Vulkan/DebugOff.hpp>
