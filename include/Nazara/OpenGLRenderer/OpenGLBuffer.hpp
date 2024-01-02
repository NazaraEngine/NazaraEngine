// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLBUFFER_HPP
#define NAZARA_OPENGLRENDERER_OPENGLBUFFER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/OpenGLDevice.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Buffer.hpp>
#include <Nazara/Renderer/RenderBuffer.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLBuffer : public RenderBuffer
	{
		public:
			OpenGLBuffer(OpenGLDevice& device, BufferType type, UInt64 size, BufferUsageFlags usage, const void* initialData = nullptr);
			OpenGLBuffer(const OpenGLBuffer&) = delete;
			OpenGLBuffer(OpenGLBuffer&&) = delete;
			~OpenGLBuffer() = default;

			bool Fill(const void* data, UInt64 offset, UInt64 size) override;

			inline const GL::Buffer& GetBuffer() const;

			void* Map(UInt64 offset, UInt64 size) override;
			bool Unmap() override;

			void UpdateDebugName(std::string_view name) override;

			OpenGLBuffer& operator=(const OpenGLBuffer&) = delete;
			OpenGLBuffer& operator=(OpenGLBuffer&&) = delete;

		private:
			GL::Buffer m_buffer;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLBuffer.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLBUFFER_HPP
