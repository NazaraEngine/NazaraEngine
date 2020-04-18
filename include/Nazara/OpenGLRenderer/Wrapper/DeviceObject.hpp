// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_GLDEVICEOBJECT_HPP
#define NAZARA_OPENGLRENDERER_GLDEVICEOBJECT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/MovablePtr.hpp>
#include <string>

namespace Nz::GL
{
	template<typename C>
	class DeviceObject
	{
		public:
			DeviceObject();
			DeviceObject(const DeviceObject&) = delete;
			DeviceObject(DeviceObject&& object) noexcept;
			~DeviceObject();

			bool Create(OpenGLDevice& device);
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
			MovablePtr<OpenGLDevice> m_device;
			GLuint m_handle;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/DeviceObject.inl>

#endif // NAZARA_OPENGLRENDERER_VKDEVICEOBJECT_HPP
