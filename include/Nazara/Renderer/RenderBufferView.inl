// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderBufferView.hpp>
#include <memory>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
	inline RenderBufferView::RenderBufferView(AbstractBuffer* buffer) :
	RenderBufferView(buffer, 0, buffer->GetSize())
	{
	}

	inline RenderBufferView::RenderBufferView(AbstractBuffer* buffer, UInt64 offset, UInt64 size) :
	m_offset(offset),
	m_size(size),
	m_buffer(buffer)
	{
	}

	inline AbstractBuffer* RenderBufferView::GetBuffer() const
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
}

#include <Nazara/Renderer/DebugOff.hpp>
