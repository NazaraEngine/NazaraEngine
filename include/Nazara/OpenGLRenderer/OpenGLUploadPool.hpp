// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLUPLOADPOOL_HPP
#define NAZARA_OPENGLRENDERER_OPENGLUPLOADPOOL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/MovablePtr.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/Buffer.hpp>
#include <Nazara/OpenGLRenderer/Wrapper/DeviceMemory.hpp>
#include <optional>
#include <vector>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLUploadPool : public UploadPool
	{
		public:
			struct OpenGLAllocation : Allocation
			{
				VkBuffer buffer;
			};

			inline OpenGLUploadPool(Vk::Device& device, UInt64 blockSize);
			OpenGLUploadPool(const OpenGLUploadPool&) = delete;
			OpenGLUploadPool(OpenGLUploadPool&&) noexcept = default;
			~OpenGLUploadPool() = default;

			OpenGLAllocation& Allocate(UInt64 size) override;
			OpenGLAllocation& Allocate(UInt64 size, UInt64 alignment) override;

			void Reset() override;

			OpenGLUploadPool& operator=(const OpenGLUploadPool&) = delete;
			OpenGLUploadPool& operator=(OpenGLUploadPool&&) = delete;

		private:
			struct Block
			{
				Vk::DeviceMemory blockMemory;
				Vk::Buffer buffer;
				UInt64 freeOffset;
			};

			UInt64 m_blockSize;
			Vk::Device& m_device;
			std::vector<Block> m_blocks;
			std::vector<OpenGLAllocation> m_allocations;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLUploadPool.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLUPLOADPOOL_HPP
