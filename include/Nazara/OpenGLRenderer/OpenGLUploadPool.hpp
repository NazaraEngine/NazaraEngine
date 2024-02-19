// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - OpenGL renderer"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_OPENGLRENDERER_OPENGLUPLOADPOOL_HPP
#define NAZARA_OPENGLRENDERER_OPENGLUPLOADPOOL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/OpenGLRenderer/Export.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <NazaraUtils/MovablePtr.hpp>
#include <array>
#include <memory>
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
			static constexpr std::size_t AllocationPerBlock = 2048;

			using AllocationBlock = std::array<Allocation, AllocationPerBlock>;

			struct Block
			{
				std::vector<UInt8> memory;
				UInt64 freeOffset = 0;
				UInt64 size;
			};

			std::size_t m_nextAllocationIndex;
			std::vector<std::unique_ptr<AllocationBlock>> m_allocationBlocks;
			std::vector<Block> m_blocks;
			UInt64 m_blockSize;
	};
}

#include <Nazara/OpenGLRenderer/OpenGLUploadPool.inl>

#endif // NAZARA_OPENGLRENDERER_OPENGLUPLOADPOOL_HPP
