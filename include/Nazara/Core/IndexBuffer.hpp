// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_INDEXBUFFER_HPP
#define NAZARA_CORE_INDEXBUFFER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Buffer.hpp>

namespace Nz
{
	class NAZARA_CORE_API IndexBuffer
	{
		public:
			IndexBuffer() = default;
			IndexBuffer(IndexType indexType, std::shared_ptr<Buffer> buffer);
			IndexBuffer(IndexType indexType, std::shared_ptr<Buffer> buffer, UInt64 offset, UInt64 size);
			IndexBuffer(IndexType indexType, UInt32 indexCount, BufferUsageFlags usage, const BufferFactory& bufferFactory, const void* initialData = nullptr);
			IndexBuffer(const IndexBuffer&) = default;
			IndexBuffer(IndexBuffer&&) noexcept = default;
			~IndexBuffer() = default;

			UInt64 ComputeCacheMissCount();

			bool Fill(const void* data, UInt64 startIndex, UInt64 length);
			bool FillRaw(const void* data, UInt64 offset, UInt64 size);

			inline const std::shared_ptr<Buffer>& GetBuffer() const;
			inline UInt64 GetEndOffset() const;
			inline UInt32 GetIndexCount() const;
			inline IndexType GetIndexType() const;
			inline UInt64 GetStride() const;
			inline UInt64 GetStartOffset() const;

			inline bool IsValid() const;

			inline void* Map(UInt64 startIndex, UInt64 length);
			inline void* Map(UInt64 startIndex, UInt64 length) const;
			void* MapRaw(UInt64 offset, UInt64 size);
			void* MapRaw(UInt64 offset, UInt64 size) const;

			void Optimize();

			void ReverseWinding();

			void Unmap() const;

			IndexBuffer& operator=(const IndexBuffer&) = default;
			IndexBuffer& operator=(IndexBuffer&&) noexcept = default;

		private:
			std::shared_ptr<Buffer> m_buffer;
			IndexType m_indexType;
			UInt32 m_indexCount;
			UInt64 m_endOffset;
			UInt64 m_startOffset;
	};
}

#include <Nazara/Core/IndexBuffer.inl>

#endif // NAZARA_CORE_INDEXBUFFER_HPP
