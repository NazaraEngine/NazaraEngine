// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_INDEXBUFFER_HPP
#define NAZARA_INDEXBUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Utility/Buffer.hpp>

namespace Nz
{
	class NAZARA_UTILITY_API IndexBuffer
	{
		public:
			IndexBuffer() = default;
			IndexBuffer(bool largeIndices, std::shared_ptr<Buffer> buffer);
			IndexBuffer(bool largeIndices, std::shared_ptr<Buffer> buffer, std::size_t offset, std::size_t size);
			IndexBuffer(bool largeIndices, std::size_t length, DataStorage storage, BufferUsageFlags usage);
			IndexBuffer(const IndexBuffer&) = default;
			IndexBuffer(IndexBuffer&&) noexcept = default;
			~IndexBuffer() = default;

			unsigned int ComputeCacheMissCount() const;

			bool Fill(const void* data, std::size_t startIndex, std::size_t length);
			bool FillRaw(const void* data, std::size_t offset, std::size_t size);

			inline const std::shared_ptr<Buffer>& GetBuffer() const;
			inline std::size_t GetEndOffset() const;
			inline std::size_t GetIndexCount() const;
			inline std::size_t GetStride() const;
			inline std::size_t GetStartOffset() const;

			inline bool HasLargeIndices() const;

			inline bool IsValid() const;

			inline void* Map(BufferAccess access, std::size_t startVertex = 0, std::size_t length = 0);
			inline void* Map(BufferAccess access, std::size_t startVertex = 0, std::size_t length = 0) const;
			void* MapRaw(BufferAccess access, std::size_t offset = 0, std::size_t size = 0);
			void* MapRaw(BufferAccess access, std::size_t offset = 0, std::size_t size = 0) const;

			void Optimize();

			void Reset();
			void Reset(bool largeIndices, std::shared_ptr<Buffer> buffer);
			void Reset(bool largeIndices, std::shared_ptr<Buffer> buffer, std::size_t offset, std::size_t size);
			void Reset(bool largeIndices, std::size_t length, DataStorage storage, BufferUsageFlags usage);
			void Reset(const IndexBuffer& indexBuffer);

			void Unmap() const;

			IndexBuffer& operator=(const IndexBuffer&) = default;
			IndexBuffer& operator=(IndexBuffer&&) noexcept = default;

		private:
			std::shared_ptr<Buffer> m_buffer;
			std::size_t m_endOffset;
			std::size_t m_indexCount;
			std::size_t m_startOffset;
			bool m_largeIndices;
	};
}

#include <Nazara/Utility/IndexBuffer.inl>

#endif // NAZARA_INDEXBUFFER_HPP
