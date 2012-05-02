// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_INDEXBUFFER_HPP
#define NAZARA_INDEXBUFFER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/Buffer.hpp>

class NAZARA_API NzIndexBuffer
{
	public:
		NzIndexBuffer(NzBuffer* buffer, unsigned int startIndex, unsigned int indexCount);
		NzIndexBuffer(unsigned int length, nzUInt8 indexSize, nzBufferUsage usage = nzBufferUsage_Static);
		NzIndexBuffer(const NzIndexBuffer& indexBuffer);
		~NzIndexBuffer();

		bool Fill(const void* data, unsigned int offset, unsigned int length);

		NzBuffer* GetBuffer() const;
		void* GetBufferPtr();
		const void* GetBufferPtr() const;
		nzUInt8 GetIndexSize() const;
		unsigned int GetIndexCount() const;
		unsigned int GetStartIndex() const;

		bool IsHardware() const;
		bool IsSequential() const;

		void* Lock(nzBufferLock lock, unsigned int offset = 0, unsigned int length = 0);
		bool Unlock();

	private:
		NzBuffer* m_buffer;
		bool m_ownsBuffer;
		unsigned int m_indexCount;
		unsigned int m_startIndex;
};

#endif // NAZARA_INDEXBUFFER_HPP
