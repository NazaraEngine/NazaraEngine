// Copyright (C) 2017 Jérôme Leclercq
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
			VertexBuffer(VertexDeclarationConstRef vertexDeclaration, BufferRef buffer);
			VertexBuffer(VertexDeclarationConstRef vertexDeclaration, BufferRef buffer, UInt32 offset, UInt32 size);
			VertexBuffer(VertexDeclarationConstRef vertexDeclaration, UInt32 length, DataStorage storage, BufferUsageFlags usage);
			VertexBuffer(const VertexBuffer& vertexBuffer);
			VertexBuffer(VertexBuffer&&) = delete;
			~VertexBuffer();

			bool Fill(const void* data, UInt32 startVertex, UInt32 length);
			bool FillRaw(const void* data, UInt32 offset, UInt32 size);

			inline const BufferRef& GetBuffer() const;
			inline UInt32 GetEndOffset() const;
			inline UInt32 GetStartOffset() const;
			inline UInt32 GetStride() const;
			inline UInt32 GetVertexCount() const;
			inline const VertexDeclarationConstRef& GetVertexDeclaration() const;

			inline bool IsValid() const;

			void* Map(BufferAccess access, UInt32 startVertex = 0, UInt32 length = 0);
			void* Map(BufferAccess access, UInt32 startVertex = 0, UInt32 length = 0) const;
			void* MapRaw(BufferAccess access, UInt32 offset = 0, UInt32 size = 0);
			void* MapRaw(BufferAccess access, UInt32 offset = 0, UInt32 size = 0) const;

			void Reset();
			void Reset(VertexDeclarationConstRef vertexDeclaration, BufferRef buffer);
			void Reset(VertexDeclarationConstRef vertexDeclaration, BufferRef buffer, UInt32 offset, UInt32 size);
			void Reset(VertexDeclarationConstRef vertexDeclaration, UInt32 length, DataStorage storage, BufferUsageFlags usage);
			void Reset(const VertexBuffer& vertexBuffer);

			void SetVertexDeclaration(VertexDeclarationConstRef vertexDeclaration);

			void Unmap() const;

			VertexBuffer& operator=(const VertexBuffer& vertexBuffer);
			VertexBuffer& operator=(VertexBuffer&&) = delete;

			template<typename... Args> static VertexBufferRef New(Args&&... args);

			// Signals:
			NazaraSignal(OnVertexBufferRelease, const VertexBuffer* /*vertexBuffer*/);

		private:
			BufferRef m_buffer;
			UInt32 m_endOffset;
			UInt32 m_startOffset;
			UInt32 m_vertexCount;
			VertexDeclarationConstRef m_vertexDeclaration;
	};
}

#include <Nazara/Utility/VertexBuffer.inl>

#endif // NAZARA_VERTEXBUFFER_HPP
