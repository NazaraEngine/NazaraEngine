// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VERTEXBUFFER_HPP
#define NAZARA_VERTEXBUFFER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Renderer/Buffer.hpp>

class NAZARA_API NzVertexBuffer
{
	public:
		NzVertexBuffer(NzBuffer* buffer, unsigned int startVertex, unsigned int vertexCount);
		NzVertexBuffer(unsigned int length, nzUInt8 typeSize, nzBufferUsage usage = nzBufferUsage_Static);
		NzVertexBuffer(const NzVertexBuffer& vertexBuffer);
		~NzVertexBuffer();

		bool Fill(const void* data, unsigned int offset, unsigned int length);

		NzBuffer* GetBuffer() const;
		void* GetBufferPtr();
		const void* GetBufferPtr() const;
		unsigned int GetStartVertex() const;
		nzUInt8 GetTypeSize() const;
		unsigned int GetVertexCount() const;

		bool IsHardware() const;

		void* Map(nzBufferAccess access, unsigned int offset = 0, unsigned int length = 0);
		bool Unmap();

	private:
		NzBuffer* m_buffer;
		bool m_ownsBuffer;
		unsigned int m_startVertex;
		unsigned int m_vertexCount;
};

#endif // NAZARA_VERTEXBUFFER_HPP
