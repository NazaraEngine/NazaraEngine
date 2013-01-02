// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VERTEXBUFFER_HPP
#define NAZARA_VERTEXBUFFER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Utility/Buffer.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>

class NAZARA_API NzVertexBuffer : public NzResource
{
	public:
		NzVertexBuffer(const NzVertexDeclaration* vertexDeclaration, NzBuffer* buffer, unsigned int startVertex, unsigned int vertexCount);
		NzVertexBuffer(const NzVertexDeclaration* vertexDeclaration, unsigned int length, nzBufferStorage storage = nzBufferStorage_Software, nzBufferUsage usage = nzBufferUsage_Static);
		NzVertexBuffer(const NzVertexBuffer& vertexBuffer);
		~NzVertexBuffer();

		bool Fill(const void* data, unsigned int offset, unsigned int length);

		NzBuffer* GetBuffer() const;
		void* GetPointer();
		const void* GetPointer() const;
		unsigned int GetStartVertex() const;
		nzUInt8 GetTypeSize() const;
		unsigned int GetVertexCount() const;
		const NzVertexDeclaration* GetVertexDeclaration() const;

		bool IsHardware() const;

		void* Map(nzBufferAccess access, unsigned int offset = 0, unsigned int length = 0);
		const void* Map(nzBufferAccess access, unsigned int offset = 0, unsigned int length = 0) const;

		bool SetStorage(nzBufferStorage storage);

		void Unmap() const;

	private:
		NzBuffer* m_buffer;
		const NzVertexDeclaration* m_vertexDeclaration;
		bool m_ownsBuffer;
		unsigned int m_startVertex;
		unsigned int m_vertexCount;
};

#endif // NAZARA_VERTEXBUFFER_HPP
