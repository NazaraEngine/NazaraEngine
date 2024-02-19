// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Error.hpp>
#include <Nazara/VulkanRenderer/Utils.hpp>

namespace Nz
{
	namespace Vk
	{
		template<typename C, typename VkType, typename CreateInfo, VkObjectType ObjectType>
		InstanceObject<C, VkType, CreateInfo, ObjectType>::InstanceObject() :
		m_handle(VK_NULL_HANDLE)
		{
		}

		template<typename C, typename VkType, typename CreateInfo, VkObjectType ObjectType>
		InstanceObject<C, VkType, CreateInfo, ObjectType>::InstanceObject(InstanceObject&& object) noexcept :
		m_instance(std::move(object.m_instance)),
		m_allocator(object.m_allocator),
		m_handle(object.m_handle),
		m_lastErrorCode(object.m_lastErrorCode)
		{
			object.m_handle = VK_NULL_HANDLE;
		}

		template<typename C, typename VkType, typename CreateInfo, VkObjectType ObjectType>
		InstanceObject<C, VkType, CreateInfo, ObjectType>::~InstanceObject()
		{
			Destroy();
		}

		template<typename C, typename VkType, typename CreateInfo, VkObjectType ObjectType>
		bool InstanceObject<C, VkType, CreateInfo, ObjectType>::Create(Instance& instance, const CreateInfo& createInfo, const VkAllocationCallbacks* allocator)
		{
			m_instance = &instance;
			m_lastErrorCode = C::CreateHelper(*m_instance, &createInfo, allocator, &m_handle);
			if (m_lastErrorCode != VkResult::VK_SUCCESS)
			{
				NazaraErrorFmt("failed to create Vulkan instance object: {0}", TranslateVulkanError(m_lastErrorCode));
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
		void InstanceObject<C, VkType, CreateInfo, ObjectType>::Destroy()
		{
			if (IsValid())
			{
				C::DestroyHelper(*m_instance, m_handle, (m_allocator.pfnAllocation) ? &m_allocator : nullptr);
				m_handle = VK_NULL_HANDLE;
			}
		}

		template<typename C, typename VkType, typename CreateInfo, VkObjectType ObjectType>
		bool InstanceObject<C, VkType, CreateInfo, ObjectType>::IsValid() const
		{
			return m_handle != VK_NULL_HANDLE;
		}

		template<typename C, typename VkType, typename CreateInfo, VkObjectType ObjectType>
		Instance* InstanceObject<C, VkType, CreateInfo, ObjectType>::GetInstance() const
		{
			return m_instance;
		}

		template<typename C, typename VkType, typename CreateInfo, VkObjectType ObjectType>
		VkResult InstanceObject<C, VkType, CreateInfo, ObjectType>::GetLastErrorCode() const
		{
			return m_lastErrorCode;
		}

		template<typename C, typename VkType, typename CreateInfo, VkObjectType ObjectType>
		auto InstanceObject<C, VkType, CreateInfo, ObjectType>::operator=(InstanceObject&& object) noexcept -> InstanceObject&
		{
			std::swap(m_allocator, object.m_allocator);
			std::swap(m_instance, object.m_instance);
			std::swap(m_handle, object.m_handle);
			std::swap(m_lastErrorCode, object.m_lastErrorCode);

			return *this;
		}

		template<typename C, typename VkType, typename CreateInfo, VkObjectType ObjectType>
		InstanceObject<C, VkType, CreateInfo, ObjectType>::operator VkType() const
		{
			return m_handle;
		}
	}
}

