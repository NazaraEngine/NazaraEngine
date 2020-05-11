// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLUPLOADPOOL_HPP
#define NAZARA_OPENGLRENDERER_OPENGLUPLOADPOOL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/MovablePtr.hpp>
#include <Nazara/OpenGLRenderer/Config.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <optional>
#include <vector>

namespace Nz
{
	class NAZARA_OPENGLRENDERER_API OpenGLUploadPool : public UploadPool
	{
		public:
			inline OpenGLUploadPool(UInt64 blockSize);
			OpenGLUploadPool(const OpenGLUploadPool&) = delete;
			OpenGLUploadPool(OpenGLUploadPool&&) noexcept = default;
			~OpenGLUploadPool() = default;

			Allocation& Allocate(UInt64 size) override;
			Allocation& Allocate(UInt64 size, UInt64 alignment) override;

			void Reset() override;

			OpenGLUploadPool& operator=(const OpenGLUploadPool&) = delete;
			OpenGLUploadPool& operator=(OpenGLUploadPool&&) = delete;

		private:
			struct Block
			{
				std::vector<UInt8> memory;
				UInt64 freeOffset = 0;
			};

			std::vector<Block> m_blocks;
			std::vector<Allocation> m_allocations;
			UInt64 m_blockSize;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLUploadPool.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLUPLOADPOOL_HPP
