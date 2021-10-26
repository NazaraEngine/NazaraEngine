// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_BUFFER_HPP
#define NAZARA_OPENGLRENDERER_BUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Utility/AbstractBuffer.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Buffer.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLBuffer : public AbstractBuffer
	{
		public:
			OpenGLBuffer(OpenGLDevice& device, BufferType type);
			OpenGLBuffer(const OpenGLBuffer&) = delete;
			OpenGLBuffer(OpenGLBuffer&&) = delete;
			~OpenGLBuffer() = default;

			bool Fill(const void* data, UInt64 offset, UInt64 size) override;

			bool Initialize(UInt64 size, BufferUsageFlags usage) override;

			inline const GL::Buffer& GetBuffer() const;
			UInt64 GetSize() const override;
			DataStorage GetStorage() const override;
			inline BufferType GetType() const;

			void* Map(BufferAccess access, UInt64 offset, UInt64 size) override;
			bool Unmap() override;

			OpenGLBuffer& operator=(const OpenGLBuffer&) = delete;
			OpenGLBuffer& operator=(OpenGLBuffer&&) = delete;

		private:
			GL::Buffer m_buffer;
			BufferType m_type;
			BufferUsageFlags m_usage;
			UInt64 m_size;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLBuffer.inl>

#endif // NAZARA_OPENGLRENDERER_BUFFER_HPP
