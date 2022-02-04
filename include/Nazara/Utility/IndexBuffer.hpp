// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_INDEXBUFFER_HPP
#define NAZARA_UTILITY_INDEXBUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Utility/Buffer.hpp>

namespace Nz
{
	class NAZARA_UTILITY_API IndexBuffer
	{
		public:
			IndexBuffer() = default;
			IndexBuffer(bool largeIndices, std::shared_ptr<Buffer> buffer);
			IndexBuffer(bool largeIndices, std::shared_ptr<Buffer> buffer, UInt64 offset, UInt64 size);
			IndexBuffer(bool largeIndices, UInt64 indexCount, BufferUsageFlags usage, const BufferFactory& bufferFactory, const void* initialData = nullptr);
			IndexBuffer(const IndexBuffer&) = default;
			IndexBuffer(IndexBuffer&&) noexcept = default;
			~IndexBuffer() = default;

			unsigned int ComputeCacheMissCount();

			bool Fill(const void* data, UInt64 startIndex, UInt64 length);
			bool FillRaw(const void* data, UInt64 offset, UInt64 size);

			inline const std::shared_ptr<Buffer>& GetBuffer() const;
			inline UInt64 GetEndOffset() const;
			inline UInt64 GetIndexCount() const;
			inline UInt64 GetStride() const;
			inline UInt64 GetStartOffset() const;

			inline bool HasLargeIndices() const;

			inline bool IsValid() const;

			inline void* Map(UInt64 startIndex, UInt64 length);
			inline void* Map(UInt64 startIndex, UInt64 length) const;
			void* MapRaw(UInt64 offset, UInt64 size);
			void* MapRaw(UInt64 offset, UInt64 size) const;

			void Optimize();

			void Unmap() const;

			IndexBuffer& operator=(const IndexBuffer&) = default;
			IndexBuffer& operator=(IndexBuffer&&) noexcept = default;

		private:
			std::shared_ptr<Buffer> m_buffer;
			UInt32 m_indexCount;
			UInt64 m_endOffset;
			UInt64 m_startOffset;
			bool m_largeIndices;
	};
}

#include <Nazara/Utility/IndexBuffer.inl>

#endif // NAZARA_UTILITY_INDEXBUFFER_HPP
