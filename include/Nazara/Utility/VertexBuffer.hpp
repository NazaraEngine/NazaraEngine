// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VERTEXBUFFER_HPP
#define NAZARA_VERTEXBUFFER_HPP

#include <Nazara/Prerequisites.hpp>
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
			VertexBuffer(VertexDeclarationConstRef vertexDeclaration, BufferRef buffer, std::size_t offset, std::size_t size);
			VertexBuffer(VertexDeclarationConstRef vertexDeclaration, std::size_t length, DataStorage storage, BufferUsageFlags usage);
			VertexBuffer(const VertexBuffer& vertexBuffer);
			VertexBuffer(VertexBuffer&&) = delete;
			~VertexBuffer();

			bool Fill(const void* data, std::size_t startVertex, std::size_t length);
			bool FillRaw(const void* data, std::size_t offset, std::size_t size);

			inline const BufferRef& GetBuffer() const;
			inline std::size_t GetEndOffset() const;
			inline std::size_t GetStartOffset() const;
			inline std::size_t GetStride() const;
			inline std::size_t GetVertexCount() const;
			inline const VertexDeclarationConstRef& GetVertexDeclaration() const;

			inline bool IsValid() const;

			void* Map(BufferAccess access, std::size_t startVertex = 0, std::size_t length = 0);
			void* Map(BufferAccess access, std::size_t startVertex = 0, std::size_t length = 0) const;
			void* MapRaw(BufferAccess access, std::size_t offset = 0, std::size_t size = 0);
			void* MapRaw(BufferAccess access, std::size_t offset = 0, std::size_t size = 0) const;

			void Reset();
			void Reset(VertexDeclarationConstRef vertexDeclaration, BufferRef buffer);
			void Reset(VertexDeclarationConstRef vertexDeclaration, BufferRef buffer, std::size_t offset, std::size_t size);
			void Reset(VertexDeclarationConstRef vertexDeclaration, std::size_t length, DataStorage storage, BufferUsageFlags usage);
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
			std::size_t m_endOffset;
			std::size_t m_startOffset;
			std::size_t m_vertexCount;
			VertexDeclarationConstRef m_vertexDeclaration;
	};
}

#include <Nazara/Utility/VertexBuffer.inl>

#endif // NAZARA_VERTEXBUFFER_HPP
