// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_WRAPPER_BUFFER_HPP
#define NAZARA_OPENGLRENDERER_WRAPPER_BUFFER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/DeviceObject.hpp>
#include <NazaraUtils/MovableValue.hpp>

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

#endif // NAZARA_OPENGLRENDERER_WRAPPER_BUFFER_HPP
