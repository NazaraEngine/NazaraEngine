// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline UInt64 GpuDynamicArray::ComputeBufferSize(UInt32 entryCount) const
	{
		return m_headerSize + UInt64(entryCount) * m_entrySize;
	}

	inline const std::shared_ptr<RenderBuffer>& GpuDynamicArray::GetBuffer() const
	{
		return m_gpuBuffer;
	}

	inline UInt32 GpuDynamicArray::GetCapacity() const
	{
		return m_capacity;
	}

	inline const UInt8* GpuDynamicArray::GetEntryData(UInt32 entryIndex) const
	{
		return m_memory.data() + ComputeBufferSize(entryIndex);
	}

	inline UInt32 GpuDynamicArray::GetEntrySize() const
	{
		return m_entrySize;
	}

	inline const UInt8* GpuDynamicArray::GetHeaderData() const
	{
		return m_memory.data();
	}

	inline UInt32 GpuDynamicArray::GetHeaderSize() const
	{
		return m_headerSize;
	}

	inline UInt32 GpuDynamicArray::GetSize() const
	{
		return m_size;
	}

	inline void GpuDynamicArray::ResetInvalidationRanges()
	{
		m_invalidatedRange = InvalidatedRange{ MaxValue(), 0 };
	}
}

