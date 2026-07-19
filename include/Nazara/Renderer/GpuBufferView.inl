// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <memory>

namespace Nz
{
	inline GpuBufferView::GpuBufferView() :
	m_offset(0),
	m_size(0),
	m_buffer(nullptr)
	{
	}

	inline GpuBufferView::GpuBufferView(GpuBuffer* buffer) :
	GpuBufferView(buffer, 0, buffer->GetSize())
	{
	}

	inline GpuBufferView::GpuBufferView(GpuBuffer* buffer, UInt64 offset, UInt64 size) :
	m_offset(offset),
	m_size(size),
	m_buffer(buffer)
	{
	}

	inline GpuBuffer* GpuBufferView::GetBuffer() const
	{
		return m_buffer;
	}

	inline UInt64 GpuBufferView::GetOffset() const
	{
		return m_offset;
	}

	inline UInt64 GpuBufferView::GetSize() const
	{
		return m_size;
	}

	inline GpuBufferView::operator bool() const
	{
		return m_buffer != nullptr;
	}

	inline bool GpuBufferView::operator==(const GpuBufferView& rhs) const
	{
		return m_buffer == rhs.m_buffer && m_offset == rhs.m_offset && m_size == rhs.m_size;
	}

	inline bool GpuBufferView::operator!=(const GpuBufferView& rhs) const
	{
		return !operator==(rhs);
	}
}
