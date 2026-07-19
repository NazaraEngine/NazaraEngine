// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_GRAPHICS_GPUBUFFERPOOL_HPP
#define NAZARA_GRAPHICS_GPUBUFFERPOOL_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Graphics/Export.hpp>
#include <Nazara/Renderer/GpuBufferView.hpp>
#include <NazaraUtils/Bitset.hpp>
#include <vector>

namespace Nz
{
	class GpuBuffer;
	class GpuDevice;

	class NAZARA_GRAPHICS_API GpuBufferPool
	{
		public:
			GpuBufferPool(std::shared_ptr<GpuDevice> renderDevice, BufferUsageFlags bufferUsages, std::size_t bufferSize, std::size_t bufferPerBlock = 2048);
			GpuBufferPool(const GpuBufferPool&) = delete;
			GpuBufferPool(GpuBufferPool&&) = delete;
			~GpuBufferPool() = default;

			std::pair<std::shared_ptr<GpuBuffer>, GpuBufferView> Allocate(std::size_t& index);

			void Free(std::size_t index);

			inline UInt64 GetBufferAlignedSize() const;
			inline UInt64 GetBufferPerBlock() const;
			inline UInt64 GetBufferSize() const;
			inline BufferUsageFlags GetBufferUsageFlags() const;

			GpuBufferPool& operator=(const GpuBufferPool&) = delete;
			GpuBufferPool& operator=(GpuBufferPool&&) = delete;

		private:
			UInt64 m_bufferAlignedSize;
			UInt64 m_bufferPerBlock;
			UInt64 m_bufferSize;
			std::shared_ptr<GpuDevice> m_renderDevice;
			std::vector<std::shared_ptr<GpuBuffer>> m_bufferBlocks;
			Bitset<UInt64> m_availableEntries;
			BufferUsageFlags m_bufferUsages;
	};
}

#include <Nazara/Graphics/GpuBufferPool.inl>

#endif // NAZARA_GRAPHICS_GPUBUFFERPOOL_HPP
