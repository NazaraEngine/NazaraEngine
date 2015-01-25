// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_INDEXBUFFER_HPP
#define NAZARA_INDEXBUFFER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectListenerWrapper.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Utility/Buffer.hpp>

class NzIndexBuffer;

using NzIndexBufferConstListener = NzObjectListenerWrapper<const NzIndexBuffer>;
using NzIndexBufferConstRef = NzObjectRef<const NzIndexBuffer>;
using NzIndexBufferListener = NzObjectListenerWrapper<NzIndexBuffer>;
using NzIndexBufferRef = NzObjectRef<NzIndexBuffer>;

class NAZARA_API NzIndexBuffer : public NzRefCounted
{
	public:
		NzIndexBuffer() = default;
		NzIndexBuffer(bool largeIndices, NzBuffer* buffer);
		NzIndexBuffer(bool largeIndices, NzBuffer* buffer, unsigned int startOffset, unsigned int endOffset);
		NzIndexBuffer(bool largeIndices, unsigned int length, nzUInt32 storage = nzDataStorage_Software, nzBufferUsage usage = nzBufferUsage_Static);
		NzIndexBuffer(const NzIndexBuffer& indexBuffer);
		~NzIndexBuffer();

		unsigned int ComputeCacheMissCount() const;

		bool Fill(const void* data, unsigned int startIndex, unsigned int length, bool forceDiscard = false);
		bool FillRaw(const void* data, unsigned int offset, unsigned int size, bool forceDiscard = false);

		NzBuffer* GetBuffer() const;
		unsigned int GetEndOffset() const;
		unsigned int GetIndexCount() const;
		unsigned int GetStride() const;
		unsigned int GetStartOffset() const;

		bool HasLargeIndices() const;

		bool IsHardware() const;
		bool IsValid() const;

		void* Map(nzBufferAccess access, unsigned int startVertex = 0, unsigned int length = 0);
		void* Map(nzBufferAccess access, unsigned int startVertex = 0, unsigned int length = 0) const;
		void* MapRaw(nzBufferAccess access, unsigned int offset = 0, unsigned int size = 0);
		void* MapRaw(nzBufferAccess access, unsigned int offset = 0, unsigned int size = 0) const;

		void Optimize();

		void Reset();
		void Reset(bool largeIndices, NzBuffer* buffer);
		void Reset(bool largeIndices, NzBuffer* buffer, unsigned int startOffset, unsigned int endOffset);
		void Reset(bool largeIndices, unsigned int length, nzUInt32 storage = nzDataStorage_Software, nzBufferUsage usage = nzBufferUsage_Static);
		void Reset(const NzIndexBuffer& indexBuffer);

		bool SetStorage(nzUInt32 storage);

		void Unmap() const;

		NzIndexBuffer& operator=(const NzIndexBuffer& indexBuffer);

		template<typename... Args> static NzIndexBufferRef New(Args&&... args);

	private:
		NzBufferRef m_buffer;
		bool m_largeIndices;
		unsigned int m_endOffset;
		unsigned int m_indexCount;
		unsigned int m_startOffset;
};

#include <Nazara/Utility/IndexBuffer.inl>

#endif // NAZARA_INDEXBUFFER_HPP
