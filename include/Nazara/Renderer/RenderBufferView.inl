// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/RenderBufferView.hpp>
#include <memory>
#include <Nazara/Renderer/Debug.hpp>

namespace Nz
{
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
}

#include <Nazara/Renderer/DebugOff.hpp>
