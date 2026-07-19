// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/GpuBufferPool.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>

namespace Nz
{
	GpuBufferPool::GpuBufferPool(std::shared_ptr<RenderDevice> renderDevice, BufferUsageFlags bufferUsages, std::size_t bufferSize, std::size_t bufferPerBlock) :
	m_bufferPerBlock(bufferPerBlock),
	m_bufferSize(bufferSize),
	m_renderDevice(std::move(renderDevice)),
	m_bufferUsages(bufferUsages)
	{
		m_bufferAlignedSize = m_bufferSize;
		if (bufferUsages.Test(BufferUsage::StorageBuffer))
			m_bufferAlignedSize = AlignPow2(m_bufferAlignedSize, m_renderDevice->GetDeviceInfo().limits.minStorageBufferOffsetAlignment);

		if (bufferUsages.Test(BufferUsage::UniformBuffer))
			m_bufferAlignedSize = AlignPow2(m_bufferAlignedSize, m_renderDevice->GetDeviceInfo().limits.minUniformBufferOffsetAlignment);
	}

	std::pair<std::shared_ptr<GpuBuffer>, GpuBufferView> GpuBufferPool::Allocate(std::size_t& index)
	{
		// First try to fetch from an already allocated block
		index = m_availableEntries.FindFirst();
		if (index != m_availableEntries.npos)
		{
			std::size_t blockIndex = index / m_bufferPerBlock;
			m_availableEntries.Set(index, false);

			std::size_t localIndex = index - (blockIndex * m_bufferPerBlock); //< faster than index % m_bufferPerBlock

			return { m_bufferBlocks[blockIndex], GpuBufferView(m_bufferBlocks[blockIndex].get(), localIndex * m_bufferAlignedSize, m_bufferSize) };
		}

		// Allocate a new block
		std::size_t blockIndex = m_bufferBlocks.size();
		m_bufferBlocks.emplace_back(m_renderDevice->InstantiateBuffer(m_bufferAlignedSize * m_bufferPerBlock, m_bufferUsages));
		m_availableEntries.Resize(m_availableEntries.GetSize() + m_bufferPerBlock, true);

		index = blockIndex * m_bufferPerBlock;
		m_availableEntries.Set(index, false);

		std::size_t localIndex = index - (blockIndex * m_bufferPerBlock); //< faster than index % m_bufferPerBlock

		return { m_bufferBlocks[blockIndex], GpuBufferView(m_bufferBlocks[blockIndex].get(), localIndex * m_bufferAlignedSize, m_bufferSize) };
	}

	void GpuBufferPool::Free(std::size_t index)
	{
		NazaraAssertMsg(!m_availableEntries.Test(index), "index is not a currently active buffer");

		m_availableEntries.Set(index, true);
	}
}
