// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_RENDERER_RENDERBUFFERVIEW_HPP
#define NAZARA_RENDERER_RENDERBUFFERVIEW_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Renderer/GpuBuffer.hpp>
#include <NazaraUtils/MovablePtr.hpp>

namespace Nz
{
	class GpuBufferView
	{
		public:
			inline GpuBufferView();
			inline GpuBufferView(GpuBuffer* buffer);
			inline GpuBufferView(GpuBuffer* buffer, UInt64 offset, UInt64 size);
			GpuBufferView(const GpuBufferView&) = default;
			GpuBufferView(GpuBufferView&&) = default;
			~GpuBufferView() = default;

			inline GpuBuffer* GetBuffer() const;
			inline UInt64 GetOffset() const;
			inline UInt64 GetSize() const;

			inline explicit operator bool() const;

			inline bool operator==(const GpuBufferView& rhs) const;
			inline bool operator!=(const GpuBufferView& rhs) const;

			GpuBufferView& operator=(const GpuBufferView&) = default;
			GpuBufferView& operator=(GpuBufferView&&) = default;

		private:
			UInt64 m_offset;
			UInt64 m_size;
			GpuBuffer* m_buffer;
	};
}

#include <Nazara/Renderer/GpuBufferView.inl>

#endif // NAZARA_RENDERER_RENDERBUFFERVIEW_HPP
