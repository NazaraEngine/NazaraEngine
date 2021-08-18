// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Vulkan Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/VulkanRenderer/Wrapper/DeviceObject.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/VulkanRenderer/Utils.hpp>
#include <type_traits>
#include <Nazara/VulkanRenderer/Debug.hpp>

namespace Nz::Vk
{
	template<typename C, typename VkType, typename CreateInfo, VkObjectType ObjectType>
	DeviceObject<C, VkType, CreateInfo, ObjectType>::DeviceObject() :
	m_handle(VK_NULL_HANDLE)
	{
	}

	template<typename C, typename VkType, typename CreateInfo, VkObjectType ObjectType>
	DeviceObject<C, VkType, CreateInfo, ObjectType>::DeviceObject(DeviceObject&& object) noexcept :
	m_device(std::move(object.m_device)),
	m_allocator(object.m_allocator),
	m_handle(object.m_handle),
	m_lastErrorCode(object.m_lastErrorCode)
	{
		object.m_handle = VK_NULL_HANDLE;
	}

	template<typename C, typename VkType, typename CreateInfo, VkObjectType ObjectType>
	DeviceObject<C, VkType, CreateInfo, ObjectType>::~DeviceObject()
	{
		Destroy();
	}

	template<typename C, typename VkType, typename CreateInfo, VkObjectType ObjectType>
	bool DeviceObject<C, VkType, CreateInfo, ObjectType>::Create(Device& device, const CreateInfo& createInfo, const VkAllocationCallbacks* allocator)
	{
		Destroy();

		m_device = &device;
		m_lastErrorCode = C::CreateHelper(*m_device, &createInfo, allocator, &m_handle);
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

	template<typename C, typename VkType, typename CreateInfo, VkObjectType ObjectType>
	void DeviceObject<C, VkType, CreateInfo, ObjectType>::Destroy()
	{
		if (IsValid())
		{
			C::DestroyHelper(*m_device, m_handle, (m_allocator.pfnAllocation) ? &m_allocator : nullptr);
			m_handle = VK_NULL_HANDLE;
		}
	}

	template<typename C, typename VkType, typename CreateInfo, VkObjectType ObjectType>
	bool DeviceObject<C, VkType, CreateInfo, ObjectType>::IsValid() const
	{
		return m_handle != VK_NULL_HANDLE;
	}

	template<typename C, typename VkType, typename CreateInfo, VkObjectType ObjectType>
	Device* DeviceObject<C, VkType, CreateInfo, ObjectType>::GetDevice() const
	{
		return m_device;
	}

	template<typename C, typename VkType, typename CreateInfo, VkObjectType ObjectType>
	VkResult DeviceObject<C, VkType, CreateInfo, ObjectType>::GetLastErrorCode() const
	{
		return m_lastErrorCode;
	}

	template<typename C, typename VkType, typename CreateInfo, VkObjectType ObjectType>
	void DeviceObject<C, VkType, CreateInfo, ObjectType>::SetDebugName(const char* name)
	{
		if (m_device->vkSetDebugUtilsObjectNameEXT)
		{
			VkDebugUtilsObjectNameInfoEXT debugName = {
				VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
				nullptr,
				ObjectType,
				0,
				name
			};

			if constexpr (std::is_pointer_v<VkType>)
				debugName.objectHandle = static_cast<UInt64>(reinterpret_cast<std::uintptr_t>(m_handle));
			else
				debugName.objectHandle = static_cast<UInt64>(m_handle);

			m_device->vkSetDebugUtilsObjectNameEXT(*m_device, &debugName);
		}
	}

	template<typename C, typename VkType, typename CreateInfo, VkObjectType ObjectType>
	void DeviceObject<C, VkType, CreateInfo, ObjectType>::SetDebugName(const std::string& name)
	{
		return SetDebugName(name.data());
	}

	template<typename C, typename VkType, typename CreateInfo, VkObjectType ObjectType>
	auto DeviceObject<C, VkType, CreateInfo, ObjectType>::operator=(DeviceObject&& object) noexcept -> DeviceObject&
	{
		std::swap(m_allocator, object.m_allocator);
		std::swap(m_device, object.m_device);
		std::swap(m_handle, object.m_handle);
		std::swap(m_lastErrorCode, object.m_lastErrorCode);

		return *this;
	}

	template<typename C, typename VkType, typename CreateInfo, VkObjectType ObjectType>
	DeviceObject<C, VkType, CreateInfo, ObjectType>::operator VkType() const
	{
		return m_handle;
	}
}

#include <Nazara/VulkanRenderer/DebugOff.hpp>
