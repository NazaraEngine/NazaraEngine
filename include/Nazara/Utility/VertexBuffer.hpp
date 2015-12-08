// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VERTEXBUFFER_HPP
#define NAZARA_VERTEXBUFFER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Utility/Buffer.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>

namespace Nz
{
	class VertexBuffer;

	using VertexBufferConstRef = ObjectRef<VertexBuffer>;
	using VertexBufferRef = ObjectRef<VertexBuffer>;

	class NAZARA_UTILITY_API VertexBuffer : public RefCounted
	{
		public:
			VertexBuffer() = default;
			VertexBuffer(const VertexDeclaration* vertexDeclaration, Buffer* buffer);
			VertexBuffer(const VertexDeclaration* vertexDeclaration, Buffer* buffer, unsigned int startOffset, unsigned int endOffset);
			VertexBuffer(const VertexDeclaration* vertexDeclaration, unsigned int length, UInt32 storage = DataStorage_Software, BufferUsage usage = BufferUsage_Static);
			VertexBuffer(const VertexBuffer& vertexBuffer);
			~VertexBuffer();

			bool Fill(const void* data, unsigned int startVertex, unsigned int length, bool forceDiscard = false);
			bool FillRaw(const void* data, unsigned int offset, unsigned int size, bool forceDiscard = false);

			Buffer* GetBuffer() const;
			unsigned int GetEndOffset() const;
			unsigned int GetStartOffset() const;
			unsigned int GetStride() const;
			unsigned int GetVertexCount() const;
			const VertexDeclaration* GetVertexDeclaration() const;

			bool IsHardware() const;
			bool IsValid() const;

			void* Map(BufferAccess access, unsigned int startVertex = 0, unsigned int length = 0);
			void* Map(BufferAccess access, unsigned int startVertex = 0, unsigned int length = 0) const;
			void* MapRaw(BufferAccess access, unsigned int offset = 0, unsigned int size = 0);
			void* MapRaw(BufferAccess access, unsigned int offset = 0, unsigned int size = 0) const;

			void Reset();
			void Reset(const VertexDeclaration* vertexDeclaration, Buffer* buffer);
			void Reset(const VertexDeclaration* vertexDeclaration, Buffer* buffer, unsigned int startOffset, unsigned int endOffset);
			void Reset(const VertexDeclaration* vertexDeclaration, unsigned int length, UInt32 storage = DataStorage_Software, BufferUsage usage = BufferUsage_Static);
			void Reset(const VertexBuffer& vertexBuffer);

			bool SetStorage(UInt32 storage);
			void SetVertexDeclaration(const VertexDeclaration* vertexDeclaration);

			void Unmap() const;

			VertexBuffer& operator=(const VertexBuffer& vertexBuffer);

			template<typename... Args> static VertexBufferRef New(Args&&... args);

			// Signals:
			NazaraSignal(OnVertexBufferRelease, const VertexBuffer* /*vertexBuffer*/);

		private:
			BufferRef m_buffer;
			VertexDeclarationConstRef m_vertexDeclaration;
			unsigned int m_endOffset;
			unsigned int m_startOffset;
			unsigned int m_vertexCount;
	};
}

#include <Nazara/Utility/VertexBuffer.inl>

#endif // NAZARA_VERTEXBUFFER_HPP
