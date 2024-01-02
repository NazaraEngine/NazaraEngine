// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_VERTEXBUFFER_HPP
#define NAZARA_UTILITY_VERTEXBUFFER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Utility/Buffer.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>

namespace Nz
{
	class NAZARA_UTILITY_API VertexBuffer
	{
		public:
			VertexBuffer() = default;
			VertexBuffer(std::shared_ptr<const VertexDeclaration> vertexDeclaration, std::shared_ptr<Buffer> buffer);
			VertexBuffer(std::shared_ptr<const VertexDeclaration> vertexDeclaration, std::shared_ptr<Buffer> buffer, UInt64 offset, UInt64 size);
			VertexBuffer(std::shared_ptr<const VertexDeclaration> vertexDeclaration, UInt32 vertexCount, BufferUsageFlags usage, const BufferFactory& bufferFactory, const void* initialData = nullptr);
			VertexBuffer(const VertexBuffer&) = default;
			VertexBuffer(VertexBuffer&&) noexcept = default;
			~VertexBuffer() = default;

			bool Fill(const void* data, UInt64 startVertex, UInt64 length);
			bool FillRaw(const void* data, UInt64 offset, UInt64 size);

			inline const std::shared_ptr<Buffer>& GetBuffer() const;
			inline UInt64 GetEndOffset() const;
			inline UInt64 GetStartOffset() const;
			inline UInt64 GetStride() const;
			inline UInt32 GetVertexCount() const;
			inline const std::shared_ptr<const VertexDeclaration>& GetVertexDeclaration() const;

			inline bool IsValid() const;

			void* Map(UInt64 startVertex, UInt64 length);
			void* Map(UInt64 startVertex, UInt64 length) const;
			void* MapRaw(UInt64 offset, UInt64 size);
			void* MapRaw(UInt64 offset, UInt64 size) const;

			void SetVertexDeclaration(std::shared_ptr<const VertexDeclaration> vertexDeclaration);

			void Unmap() const;

			VertexBuffer& operator=(const VertexBuffer&) = default;
			VertexBuffer& operator=(VertexBuffer&&) noexcept = default;

		private:
			std::shared_ptr<Buffer> m_buffer;
			std::shared_ptr<const VertexDeclaration> m_vertexDeclaration;
			UInt32 m_vertexCount;
			UInt64 m_endOffset;
			UInt64 m_startOffset;
	};
}

#include <Nazara/Utility/VertexBuffer.inl>

#endif // NAZARA_UTILITY_VERTEXBUFFER_HPP
