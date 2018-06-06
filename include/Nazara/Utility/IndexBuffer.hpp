// Copyright (C) 2017 Jérôme Leclercq
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
			IndexBuffer(bool largeIndices, BufferRef buffer, UInt32 offset, UInt32 size);
			IndexBuffer(bool largeIndices, UInt32 length, DataStorage storage, BufferUsageFlags usage);
			IndexBuffer(const IndexBuffer& indexBuffer);
			IndexBuffer(IndexBuffer&&) = delete;
			~IndexBuffer();

			unsigned int ComputeCacheMissCount() const;

			bool Fill(const void* data, UInt32 startIndex, UInt32 length);
			bool FillRaw(const void* data, UInt32 offset, UInt32 size);

			inline const BufferRef& GetBuffer() const;
			inline UInt32 GetEndOffset() const;
			inline UInt32 GetIndexCount() const;
			inline UInt32 GetStride() const;
			inline UInt32 GetStartOffset() const;

			inline bool HasLargeIndices() const;

			inline bool IsValid() const;

			inline void* Map(BufferAccess access, UInt32 startVertex = 0, UInt32 length = 0);
			inline void* Map(BufferAccess access, UInt32 startVertex = 0, UInt32 length = 0) const;
			void* MapRaw(BufferAccess access, UInt32 offset = 0, UInt32 size = 0);
			void* MapRaw(BufferAccess access, UInt32 offset = 0, UInt32 size = 0) const;

			void Optimize();

			void Reset();
			void Reset(bool largeIndices, BufferRef buffer);
			void Reset(bool largeIndices, BufferRef buffer, UInt32 offset, UInt32 size);
			void Reset(bool largeIndices, UInt32 length, DataStorage storage, BufferUsageFlags usage);
			void Reset(const IndexBuffer& indexBuffer);

			void Unmap() const;

			IndexBuffer& operator=(const IndexBuffer& indexBuffer);
			IndexBuffer& operator=(IndexBuffer&&) = delete;

			template<typename... Args> static IndexBufferRef New(Args&&... args);

			// Signals:
			NazaraSignal(OnIndexBufferRelease, const IndexBuffer* /*indexBuffer*/);

		private:
			BufferRef m_buffer;
			UInt32 m_endOffset;
			UInt32 m_indexCount;
			UInt32 m_startOffset;
			bool m_largeIndices;
	};
}

#include <Nazara/Utility/IndexBuffer.inl>

#endif // NAZARA_INDEXBUFFER_HPP
