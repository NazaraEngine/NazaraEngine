// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WRAPPER_DEVICEOBJECT_HPP
#define NAZARA_OPENGLRENDERER_WRAPPER_DEVICEOBJECT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/MovablePtr.hpp>
#include <Nazara/Core/MovableValue.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <string>

namespace Nz::GL
{
	template<typename C, GLenum ObjectType, typename... CreateArgs>
	class DeviceObject
	{
		public:
			DeviceObject() = default;
			DeviceObject(OpenGLDevice& device, CreateArgs... args);
			DeviceObject(const DeviceObject&) = delete;
			DeviceObject(DeviceObject&& object) noexcept = default;
			~DeviceObject();

			bool Create(OpenGLDevice& device, CreateArgs... args);
			void Destroy();

			const Context& EnsureDeviceContext() const;

			bool IsValid() const;

			OpenGLDevice* GetDevice() const;
			GLuint GetObjectId() const;

			void SetDebugName(const std::string_view& name);

			DeviceObject& operator=(const DeviceObject&) = delete;
			DeviceObject& operator=(DeviceObject&& object) noexcept = default;

			static constexpr GLuint InvalidObject = 0;

		protected:
			MovablePtr<OpenGLDevice> m_device;
			MovableValue<GLuint> m_objectId;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/DeviceObject.inl>

#endif // NAZARA_OPENGLRENDERER_WRAPPER_DEVICEOBJECT_HPP
