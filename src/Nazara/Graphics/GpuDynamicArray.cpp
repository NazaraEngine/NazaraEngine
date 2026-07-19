// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Graphics/GpuDynamicArray.hpp>
#include <Nazara/Renderer/CommandBufferBuilder.hpp>
#include <Nazara/Renderer/RenderDevice.hpp>
#include <Nazara/Renderer/UploadPool.hpp>
#include <NazaraUtils/Assert.hpp>

namespace Nz
{
	GpuDynamicArray::GpuDynamicArray(RenderDevice& renderDevice, UInt32 entrySize, UInt32 initialCapacity, UInt32 headerSize, BufferUsageFlags bufferUsageFlags) :
	m_bufferUsageFlags(bufferUsageFlags),
	m_capacity(initialCapacity),
	m_entrySize(entrySize),
	m_headerSize(headerSize),
	m_size(0),
	m_renderDevice(renderDevice)
	{
		NazaraAssert(m_capacity > 0);
		NazaraAssert(m_entrySize > 0);

		UInt64 bufferSize = ComputeBufferSize(m_capacity);
		m_memory.resize(bufferSize);
		m_gpuBuffer = m_renderDevice.InstantiateBuffer(bufferSize, m_bufferUsageFlags);
	}

	UInt8* GpuDynamicArray::AccessEntry(UInt32 entryIndex)
	{
		NazaraAssert(entryIndex < m_size);

		UInt64 offset = ComputeBufferSize(entryIndex);
		UInt8* data = &m_memory[offset];

		m_invalidatedRange.start = std::min<UInt64>(m_invalidatedRange.start, offset);
		m_invalidatedRange.end = std::max<UInt64>(m_invalidatedRange.end, offset + m_entrySize);
		OnTransferRequired(this);

		return data;
	}

	UInt8* GpuDynamicArray::AccessHeader()
	{
		NazaraAssert(m_headerSize > 0);

		m_invalidatedRange.start = std::min<UInt64>(m_invalidatedRange.start, 0);
		m_invalidatedRange.end = std::max<UInt64>(m_invalidatedRange.end, m_headerSize);
		OnTransferRequired(this);

		return m_memory.data();
	}

	void GpuDynamicArray::OnTransfer(RenderResources& renderResources, CommandBufferBuilder& builder)
	{
		NazaraAssert(m_invalidatedRange.start <= m_invalidatedRange.end);
		UInt64 size = m_invalidatedRange.end - m_invalidatedRange.start;
		if (size == 0)
			return;

		const RenderDeviceInfo& deviceInfo = m_renderDevice.GetDeviceInfo();

		// Ensure alignment is respected
		UInt64 offsetAlignment = 1;
		if (m_bufferUsageFlags & BufferUsage::StorageBuffer)
			offsetAlignment = AlignPow2(offsetAlignment, deviceInfo.limits.minStorageBufferOffsetAlignment);

		if (m_bufferUsageFlags & BufferUsage::UniformBuffer)
			offsetAlignment = AlignPow2(offsetAlignment, deviceInfo.limits.minUniformBufferOffsetAlignment);

		if (m_invalidatedRange.start > offsetAlignment)
			m_invalidatedRange.start = AlignPow2(m_invalidatedRange.start - offsetAlignment + 1, offsetAlignment);
		else if (m_invalidatedRange.start < offsetAlignment)
			m_invalidatedRange.start = 0;

		size = m_invalidatedRange.end - m_invalidatedRange.start;

		UploadPool::Allocation& allocation = renderResources.GetUploadPool().Allocate(size);
		std::memcpy(allocation.mappedPtr, &m_memory[m_invalidatedRange.start], size);

		builder.CopyBuffer(allocation, GpuBufferView(m_gpuBuffer.get(), m_invalidatedRange.start, m_invalidatedRange.end));

		ResetInvalidationRanges();
	}

	UInt32 GpuDynamicArray::Push()
	{
		if (m_size == m_capacity)
			GrowBuffer();

		return m_size++;
	}

	void GpuDynamicArray::Pop()
	{
		m_size--;

		UInt64 bufferRange = ComputeBufferSize(m_size);
		if (m_invalidatedRange.start >= bufferRange)
			ResetInvalidationRanges();
		else
			m_invalidatedRange.end = std::min(m_invalidatedRange.end, bufferRange);
	}

	void GpuDynamicArray::UpdateDebugName(std::string debugName)
	{
		m_debugName = std::move(debugName);
		m_gpuBuffer->UpdateDebugName(m_debugName);
	}

	void GpuDynamicArray::GrowBuffer()
	{
		UInt64 previousSize = ComputeBufferSize(m_capacity);
		m_capacity *= 2;
		UInt64 newSize = ComputeBufferSize(m_capacity);

		m_memory.resize(newSize);
		m_gpuBuffer = m_renderDevice.InstantiateBuffer(newSize, m_bufferUsageFlags);
		if (!m_debugName.empty())
			m_gpuBuffer->UpdateDebugName(m_debugName);

		// We need to re-upload the whole buffer when growing
		m_invalidatedRange = InvalidatedRange{0u, previousSize};

		OnBufferInvalidated(this);
		OnTransferRequired(this);
	}
}
