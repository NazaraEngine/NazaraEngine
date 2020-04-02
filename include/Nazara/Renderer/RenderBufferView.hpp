// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERBUFFERVIEW_HPP
#define NAZARA_RENDERBUFFERVIEW_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/MovablePtr.hpp>
#include <Nazara/Utility/AbstractBuffer.hpp>

namespace Nz
{
	class RenderBufferView
	{
		public:
			inline RenderBufferView(AbstractBuffer* buffer);
			inline RenderBufferView(AbstractBuffer* buffer, UInt64 offset, UInt64 size);
			RenderBufferView(const RenderBufferView&) = delete;
			RenderBufferView(RenderBufferView&&) = default;
			~RenderBufferView() = default;

			inline AbstractBuffer* GetBuffer() const;
			inline UInt64 GetOffset() const;
			inline UInt64 GetSize() const;

			RenderBufferView& operator=(const RenderBufferView&) = delete;
			RenderBufferView& operator=(RenderBufferView&&) = default;

		private:
			UInt64 m_offset;
			UInt64 m_size;
			AbstractBuffer* m_buffer;
	};
}

#include <Nazara/Renderer/RenderBufferView.inl>

#endif // NAZARA_RENDERBUFFERVIEW_HPP
