// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_VKDEVICEOBJECT_HPP
#define NAZARA_OPENGLRENDERER_VKDEVICEOBJECT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/MovablePtr.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Device.hpp>
#include <vulkan/vulkan.h>
#include <string>

namespace Nz 
{
	namespace Vk
	{
		template<typename C, typename VkType, typename CreateInfo, VkObjectType ObjectType>
		class DeviceObject
		{
			public:
				DeviceObject();
				DeviceObject(const DeviceObject&) = delete;
				DeviceObject(DeviceObject&& object) noexcept;
				~DeviceObject();

				bool Create(Device& device, const CreateInfo& createInfo, const VkAllocationCallbacks* allocator = nullptr);
				void Destroy();

				bool IsValid() const;

				Device* GetDevice() const;
				VkResult GetLastErrorCode() const;

				void SetDebugName(const char* name);
				void SetDebugName(const std::string& name);

				DeviceObject& operator=(const DeviceObject&) = delete;
				DeviceObject& operator=(DeviceObject&& object) noexcept;

				operator VkType() const;

			protected:
				MovablePtr<Device> m_device;
				VkAllocationCallbacks m_allocator;
				VkType m_handle;
				mutable VkResult m_lastErrorCode;
		};
	}
}

#include <Nazara/OpenGLRenderer/Wrapper/DeviceObject.inl>

#endif // NAZARA_OPENGLRENDERER_VKDEVICEOBJECT_HPP
