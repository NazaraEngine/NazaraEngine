// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_BUFFERPOOL_HPP
#define NAZARA_GRAPHICS_BUFFERPOOL_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/Config.hpp>
#include <Nazara/Renderer/RenderBufferView.hpp>
#include <Nazara/Utils/Bitset.hpp>
#include <vector>

namespace Nz
{
	class RenderBuffer;
	class RenderDevice;

	class NAZARA_GRAPHICS_API RenderBufferPool
	{
		public:
			RenderBufferPool(std::shared_ptr<RenderDevice> renderDevice, BufferType bufferType, std::size_t bufferSize, std::size_t bufferPerBlock = 2048);
			RenderBufferPool(const RenderBufferPool&) = delete;
			RenderBufferPool(RenderBufferPool&&) = delete;
			~RenderBufferPool() = default;

			RenderBufferView Allocate(std::size_t& index);

			void Free(std::size_t index);

			inline std::size_t GetBufferAlignedSize() const;
			inline std::size_t GetBufferPerBlock() const;
			inline std::size_t GetBufferSize() const;
			inline BufferType GetBufferType() const;

			RenderBufferPool& operator=(const RenderBufferPool&) = delete;
			RenderBufferPool& operator=(RenderBufferPool&&) = delete;

		private:
			std::size_t m_bufferAlignedSize;
			std::size_t m_bufferPerBlock;
			std::size_t m_bufferSize;
			std::shared_ptr<RenderDevice> m_renderDevice;
			std::vector<std::shared_ptr<RenderBuffer>> m_bufferBlocks;
			Bitset<UInt64> m_availableEntries;
			BufferType m_bufferType;
	};
}

#include <Nazara/Graphics/RenderBufferPool.inl>

#endif // NAZARA_GRAPHICS_BUFFERPOOL_HPP
