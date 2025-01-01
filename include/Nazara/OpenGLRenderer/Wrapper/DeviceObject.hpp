// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WRAPPER_DEVICEOBJECT_HPP
#define NAZARA_OPENGLRENDERER_WRAPPER_DEVICEOBJECT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Context.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <NazaraUtils/MovableValue.hpp>
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

			void SetDebugName(std::string_view name);

			DeviceObject& operator=(const DeviceObject&) = delete;
			DeviceObject& operator=(DeviceObject&& object) noexcept = default;

			static constexpr GLuint InvalidObject = 0;

		protected:
			MovableLiteral<GLuint, InvalidObject> m_objectId;
			MovablePtr<OpenGLDevice> m_device;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/DeviceObject.inl>

#endif // NAZARA_OPENGLRENDERER_WRAPPER_DEVICEOBJECT_HPP
