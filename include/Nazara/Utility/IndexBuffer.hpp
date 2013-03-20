// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_INDEXBUFFER_HPP
#define NAZARA_INDEXBUFFER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceRef.hpp>
#include <Nazara/Utility/Buffer.hpp>

class NzIndexBuffer;

using NzIndexBufferConstRef = NzResourceRef<const NzIndexBuffer>;
using NzIndexBufferRef = NzResourceRef<NzIndexBuffer>;

class NAZARA_API NzIndexBuffer : public NzResource
{
	public:
		NzIndexBuffer(NzBuffer* buffer, unsigned int startIndex, unsigned int indexCount);
		NzIndexBuffer(unsigned int length, bool largeIndices = false, nzBufferStorage storage = nzBufferStorage_Software, nzBufferUsage usage = nzBufferUsage_Static);
		NzIndexBuffer(const NzIndexBuffer& indexBuffer);
		~NzIndexBuffer();

		bool Fill(const void* data, unsigned int offset, unsigned int length, bool forceDiscard = false);

		NzBuffer* GetBuffer() const;
		unsigned int GetIndexCount() const;
		void* GetPointer();
		const void* GetPointer() const;
		unsigned int GetStartIndex() const;

		bool HasLargeIndices() const;

		bool IsHardware() const;
		bool IsSequential() const;

		void* Map(nzBufferAccess access, unsigned int offset = 0, unsigned int length = 0);
		void* Map(nzBufferAccess access, unsigned int offset = 0, unsigned int length = 0) const;

		bool SetStorage(nzBufferStorage storage);

		void Unmap() const;

	private:
		NzBufferRef m_buffer;
		bool m_ownsBuffer;
		unsigned int m_indexCount;
		unsigned int m_startIndex;
};

#endif // NAZARA_INDEXBUFFER_HPP
