// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - OpenGL Renderer"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_BUFFER_HPP
#define NAZARA_OPENGLRENDERER_BUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Utility/AbstractBuffer.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Buffer.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/DeviceMemory.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Fence.hpp>
#include <memory>
#include <vector>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLBuffer : public AbstractBuffer
	{
		public:
			inline OpenGLBuffer(Vk::Device& device, BufferType type);
			OpenGLBuffer(const OpenGLBuffer&) = delete;
			OpenGLBuffer(OpenGLBuffer&&) = delete; ///TODO
			virtual ~OpenGLBuffer();

			bool Fill(const void* data, UInt64 offset, UInt64 size) override;

			bool Initialize(UInt64 size, BufferUsageFlags usage) override;

			inline VkBuffer GetBuffer();
			UInt64 GetSize() const override;
			DataStorage GetStorage() const override;

			void* Map(BufferAccess access, UInt64 offset, UInt64 size) override;
			bool Unmap() override;

			OpenGLBuffer& operator=(const OpenGLBuffer&) = delete;
			OpenGLBuffer& operator=(OpenGLBuffer&&) = delete; ///TODO

		private:
			BufferType m_type;
			BufferUsageFlags m_usage;
			UInt64 m_size;
			VkBuffer m_buffer;
			VkBuffer m_stagingBuffer;
			VmaAllocation m_allocation;
			VmaAllocation m_stagingAllocation;
			Vk::Device& m_device;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLBuffer.inl>

#endif // NAZARA_OPENGLRENDERER_BUFFER_HPP
