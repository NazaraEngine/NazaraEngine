// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RENDERER_RENDERBUFFERVIEW_HPP
#define NAZARA_RENDERER_RENDERBUFFERVIEW_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/RenderBuffer.hpp>
#include <NazaraUtils/MovablePtr.hpp>

namespace Nz
{
	class RenderBufferView
	{
		public:
			inline RenderBufferView();
			inline RenderBufferView(RenderBuffer* buffer);
			inline RenderBufferView(RenderBuffer* buffer, UInt64 offset, UInt64 size);
			RenderBufferView(const RenderBufferView&) = default;
			RenderBufferView(RenderBufferView&&) = default;
			~RenderBufferView() = default;

			inline RenderBuffer* GetBuffer() const;
			inline UInt64 GetOffset() const;
			inline UInt64 GetSize() const;

			inline explicit operator bool() const;

			inline bool operator==(const RenderBufferView& rhs) const;
			inline bool operator!=(const RenderBufferView& rhs) const;

			RenderBufferView& operator=(const RenderBufferView&) = default;
			RenderBufferView& operator=(RenderBufferView&&) = default;

		private:
			UInt64 m_offset;
			UInt64 m_size;
			RenderBuffer* m_buffer;
	};
}

#include <Nazara/Renderer/RenderBufferView.inl>

#endif // NAZARA_RENDERER_RENDERBUFFERVIEW_HPP
