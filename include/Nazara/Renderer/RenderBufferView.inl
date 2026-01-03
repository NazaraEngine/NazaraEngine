// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <memory>

namespace Nz
{
	inline RenderBufferView::RenderBufferView() :
	m_offset(0),
	m_size(0),
	m_buffer(nullptr)
	{
	}

	inline RenderBufferView::RenderBufferView(RenderBuffer* buffer) :
	RenderBufferView(buffer, 0, buffer->GetSize())
	{
	}

	inline RenderBufferView::RenderBufferView(RenderBuffer* buffer, UInt64 offset, UInt64 size) :
	m_offset(offset),
	m_size(size),
	m_buffer(buffer)
	{
	}

	inline RenderBuffer* RenderBufferView::GetBuffer() const
	{
		return m_buffer;
	}

	inline UInt64 RenderBufferView::GetOffset() const
	{
		return m_offset;
	}

	inline UInt64 RenderBufferView::GetSize() const
	{
		return m_size;
	}

	inline RenderBufferView::operator bool() const
	{
		return m_buffer != nullptr;
	}

	inline bool RenderBufferView::operator==(const RenderBufferView& rhs) const
	{
		return m_buffer == rhs.m_buffer && m_offset == rhs.m_offset && m_size == rhs.m_size;
	}

	inline bool RenderBufferView::operator!=(const RenderBufferView& rhs) const
	{
		return !operator==(rhs);
	}
}
