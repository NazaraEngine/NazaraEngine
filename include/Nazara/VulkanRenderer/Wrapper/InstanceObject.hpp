// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Vulkan renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_VULKANRENDERER_WRAPPER_INSTANCEOBJECT_HPP
#define NAZARA_VULKANRENDERER_WRAPPER_INSTANCEOBJECT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/VulkanRenderer/Wrapper/Instance.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <vulkan/vulkan_core.h>

namespace Nz::Vk
{
	template<typename C, typename VkType, typename CreateInfo, VkObjectType ObjectType>
	class InstanceObject
	{
		public:
			InstanceObject();
			InstanceObject(const InstanceObject&) = delete;
			InstanceObject(InstanceObject&& object) noexcept;
			~InstanceObject();

			bool Create(Instance& instance, const CreateInfo& createInfo, const VkAllocationCallbacks* allocator = nullptr);
			void Destroy();

			bool IsValid() const;

			Instance* GetInstance() const;
			VkResult GetLastErrorCode() const;

			InstanceObject& operator=(const InstanceObject&) = delete;
			InstanceObject& operator=(InstanceObject&& object) noexcept;

			operator VkType() const;

		protected:
			MovablePtr<Instance> m_instance;
			VkAllocationCallbacks m_allocator;
			VkType m_handle;
			mutable VkResult m_lastErrorCode;
	};
}

#include <Nazara/VulkanRenderer/Wrapper/InstanceObject.inl>

#endif // NAZARA_VULKANRENDERER_WRAPPER_INSTANCEOBJECT_HPP
