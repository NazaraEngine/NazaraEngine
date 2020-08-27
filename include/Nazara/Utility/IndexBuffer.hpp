// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_INDEXBUFFER_HPP
#define NAZARA_INDEXBUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Utility/Buffer.hpp>

namespace Nz
{
	class IndexBuffer;

	using IndexBufferConstRef = ObjectRef<const IndexBuffer>;
	using IndexBufferRef = ObjectRef<IndexBuffer>;

	class NAZARA_UTILITY_API IndexBuffer : public RefCounted
	{
		public:
			IndexBuffer() = default;
			IndexBuffer(bool largeIndices, BufferRef buffer);
			IndexBuffer(bool largeIndices, BufferRef buffer, std::size_t offset, std::size_t size);
			IndexBuffer(bool largeIndices, std::size_t length, DataStorage storage, BufferUsageFlags usage);
			IndexBuffer(const IndexBuffer& indexBuffer);
			IndexBuffer(IndexBuffer&&) = delete;
			~IndexBuffer();

			unsigned int ComputeCacheMissCount() const;

			bool Fill(const void* data, std::size_t startIndex, std::size_t length);
			bool FillRaw(const void* data, std::size_t offset, std::size_t size);

			inline const BufferRef& GetBuffer() const;
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
			void Reset(bool largeIndices, BufferRef buffer);
			void Reset(bool largeIndices, BufferRef buffer, std::size_t offset, std::size_t size);
			void Reset(bool largeIndices, std::size_t length, DataStorage storage, BufferUsageFlags usage);
			void Reset(const IndexBuffer& indexBuffer);

			void Unmap() const;

			IndexBuffer& operator=(const IndexBuffer& indexBuffer);
			IndexBuffer& operator=(IndexBuffer&&) = delete;

			template<typename... Args> static IndexBufferRef New(Args&&... args);

			// Signals:
			NazaraSignal(OnIndexBufferRelease, const IndexBuffer* /*indexBuffer*/);

		private:
			BufferRef m_buffer;
			std::size_t m_endOffset;
			std::size_t m_indexCount;
			std::size_t m_startOffset;
			bool m_largeIndices;
	};
}

#include <Nazara/Utility/IndexBuffer.inl>

#endif // NAZARA_INDEXBUFFER_HPP
