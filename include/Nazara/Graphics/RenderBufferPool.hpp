// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_RENDERBUFFERPOOL_HPP
#define NAZARA_GRAPHICS_RENDERBUFFERPOOL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Renderer/RenderBufferView.hpp>
#include <NazaraUtils/Bitset.hpp>
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

			std::pair<std::shared_ptr<RenderBuffer>, RenderBufferView> Allocate(std::size_t& index);

			void Free(std::size_t index);

			inline UInt64 GetBufferAlignedSize() const;
			inline UInt64 GetBufferPerBlock() const;
			inline UInt64 GetBufferSize() const;
			inline BufferType GetBufferType() const;

			RenderBufferPool& operator=(const RenderBufferPool&) = delete;
			RenderBufferPool& operator=(RenderBufferPool&&) = delete;

		private:
			UInt64 m_bufferAlignedSize;
			UInt64 m_bufferPerBlock;
			UInt64 m_bufferSize;
			std::shared_ptr<RenderDevice> m_renderDevice;
			std::vector<std::shared_ptr<RenderBuffer>> m_bufferBlocks;
			Bitset<UInt64> m_availableEntries;
			BufferType m_bufferType;
	};
}

#include <Nazara/Graphics/RenderBufferPool.inl>

#endif // NAZARA_GRAPHICS_RENDERBUFFERPOOL_HPP
