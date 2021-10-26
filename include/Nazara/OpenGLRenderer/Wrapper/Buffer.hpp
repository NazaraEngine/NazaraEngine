// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_GLBUFFER_HPP
#define NAZARA_OPENGLRENDERER_GLBUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/MovableValue.hpp>
#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/DeviceObject.hpp>

namespace Nz::GL
{
	class Buffer : public DeviceObject<Buffer, GL_BUFFER>
	{
		friend DeviceObject;

		public:
			using DeviceObject::DeviceObject;
			Buffer(const Buffer&) = delete;
			Buffer(Buffer&&) noexcept = default;
			~Buffer() = default;

			inline void* MapRange(GLintptr offset, GLsizeiptr length, GLbitfield access);

			inline void Reset(BufferTarget target, GLsizeiptr size, const void* initialData, GLenum usage);

			inline void SubData(GLintptr offset, GLsizeiptr size, const void* data);

			inline bool Unmap();

			Buffer& operator=(const Buffer&) = delete;
			Buffer& operator=(Buffer&&) noexcept = default;

		private:
			static inline GLuint CreateHelper(OpenGLDevice& device, const Context& context);
			static inline void DestroyHelper(OpenGLDevice& device, const Context& context, GLuint objectId);

			BufferTarget m_target;
	};
}

#include <Nazara/OpenGLRenderer/Wrapper/Buffer.inl>

#endif
