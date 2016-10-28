// Copyright (C) 2016 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Wrapper/DeviceObject.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/VulkanRenderer/Utils.hpp>
#include <Nazara/VulkanRenderer/Wrapper/CommandPool.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Device.hpp>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz
{
	namespace Vk
	{
		template<typename C, typename VkType, typename CreateInfo>
		inline DeviceObject<C, VkType, CreateInfo>::DeviceObject() :
		m_handle(VK_NULL_HANDLE)
		{
		}

		template<typename C, typename VkType, typename CreateInfo>
		inline DeviceObject<C, VkType, CreateInfo>::DeviceObject(DeviceObject&& object) :
		m_device(std::move(object.m_device)),
		m_allocator(object.m_allocator),
		m_handle(object.m_handle),
		m_lastErrorCode(object.m_lastErrorCode)
		{
			object.m_handle = VK_NULL_HANDLE;
		}

		template<typename C, typename VkType, typename CreateInfo>
		inline DeviceObject<C, VkType, CreateInfo>::~DeviceObject()
		{
			Destroy();
		}

		template<typename C, typename VkType, typename CreateInfo>
		inline bool DeviceObject<C, VkType, CreateInfo>::Create(const DeviceHandle& device, const CreateInfo& createInfo, const VkAllocationCallbacks* allocator)
		{
			m_device = device;
			m_lastErrorCode = C::CreateHelper(m_device, &createInfo, allocator, &m_handle);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraError("Failed to create Vulkan object: " + TranslateVulkanError(m_lastErrorCode));
				return false;
			}

			// Store the allocator to access them when needed
			if (allocator)
				m_allocator = *allocator;
			else
				m_allocator.pfnAllocation = nullptr;

			return true;
		}

		template<typename C, typename VkType, typename CreateInfo>
		inline void DeviceObject<C, VkType, CreateInfo>::Destroy()
		{
			if (IsValid())
			{
				C::DestroyHelper(m_device, m_handle, (m_allocator.pfnAllocation) ? &m_allocator : nullptr);
				m_handle = VK_NULL_HANDLE;
			}
		}

		template<typename C, typename VkType, typename CreateInfo>
		inline bool DeviceObject<C, VkType, CreateInfo>::IsValid() const
		{
			return m_handle != VK_NULL_HANDLE;
		}

		template<typename C, typename VkType, typename CreateInfo>
		inline const DeviceHandle& DeviceObject<C, VkType, CreateInfo>::GetDevice() const
		{
			return m_device;
		}

		template<typename C, typename VkType, typename CreateInfo>
		inline VkResult DeviceObject<C, VkType, CreateInfo>::GetLastErrorCode() const
		{
			return m_lastErrorCode;
		}

		template<typename C, typename VkType, typename CreateInfo>
		inline DeviceObject<C, VkType, CreateInfo>::operator VkType() const
		{
			return m_handle;
		}
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
