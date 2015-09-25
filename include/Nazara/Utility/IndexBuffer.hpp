// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_INDEXBUFFER_HPP
#define NAZARA_INDEXBUFFER_HPP

#include <Nazara/Prerequesites.hpp>
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
			IndexBuffer(bool largeIndices, Buffer* buffer);
			IndexBuffer(bool largeIndices, Buffer* buffer, unsigned int startOffset, unsigned int endOffset);
			IndexBuffer(bool largeIndices, unsigned int length, UInt32 storage = DataStorage_Software, BufferUsage usage = BufferUsage_Static);
			IndexBuffer(const IndexBuffer& indexBuffer);
			~IndexBuffer();

			unsigned int ComputeCacheMissCount() const;

			bool Fill(const void* data, unsigned int startIndex, unsigned int length, bool forceDiscard = false);
			bool FillRaw(const void* data, unsigned int offset, unsigned int size, bool forceDiscard = false);

			Buffer* GetBuffer() const;
			unsigned int GetEndOffset() const;
			unsigned int GetIndexCount() const;
			unsigned int GetStride() const;
			unsigned int GetStartOffset() const;

			bool HasLargeIndices() const;

			bool IsHardware() const;
			bool IsValid() const;

			void* Map(BufferAccess access, unsigned int startVertex = 0, unsigned int length = 0);
			void* Map(BufferAccess access, unsigned int startVertex = 0, unsigned int length = 0) const;
			void* MapRaw(BufferAccess access, unsigned int offset = 0, unsigned int size = 0);
			void* MapRaw(BufferAccess access, unsigned int offset = 0, unsigned int size = 0) const;

			void Optimize();

			void Reset();
			void Reset(bool largeIndices, Buffer* buffer);
			void Reset(bool largeIndices, Buffer* buffer, unsigned int startOffset, unsigned int endOffset);
			void Reset(bool largeIndices, unsigned int length, UInt32 storage = DataStorage_Software, BufferUsage usage = BufferUsage_Static);
			void Reset(const IndexBuffer& indexBuffer);

			bool SetStorage(UInt32 storage);

			void Unmap() const;

			IndexBuffer& operator=(const IndexBuffer& indexBuffer);

			template<typename... Args> static IndexBufferRef New(Args&&... args);

			// Signals:
			NazaraSignal(OnIndexBufferRelease, const IndexBuffer* /*indexBuffer*/);

		private:
			BufferRef m_buffer;
			bool m_largeIndices;
			unsigned int m_endOffset;
			unsigned int m_indexCount;
			unsigned int m_startOffset;
	};
}

#include <Nazara/Utility/IndexBuffer.inl>

#endif // NAZARA_INDEXBUFFER_HPP
