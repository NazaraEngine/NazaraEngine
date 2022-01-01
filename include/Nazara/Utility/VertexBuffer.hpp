// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_VERTEXBUFFER_HPP
#define NAZARA_UTILITY_VERTEXBUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Utility/Buffer.hpp>
#include <Nazara/Utility/VertexDeclaration.hpp>

namespace Nz
{
	class NAZARA_UTILITY_API VertexBuffer
	{
		public:
			VertexBuffer() = default;
			VertexBuffer(std::shared_ptr<const VertexDeclaration> vertexDeclaration, std::shared_ptr<Buffer> buffer);
			VertexBuffer(std::shared_ptr<const VertexDeclaration> vertexDeclaration, std::shared_ptr<Buffer> buffer, std::size_t offset, std::size_t size);
			VertexBuffer(std::shared_ptr<const VertexDeclaration> vertexDeclaration, std::size_t length, DataStorage storage, BufferUsageFlags usage);
			VertexBuffer(const VertexBuffer&) = default;
			VertexBuffer(VertexBuffer&&) noexcept = default;
			~VertexBuffer() = default;

			bool Fill(const void* data, std::size_t startVertex, std::size_t length);
			bool FillRaw(const void* data, std::size_t offset, std::size_t size);

			inline const std::shared_ptr<Buffer>& GetBuffer() const;
			inline std::size_t GetEndOffset() const;
			inline std::size_t GetStartOffset() const;
			inline std::size_t GetStride() const;
			inline std::size_t GetVertexCount() const;
			inline const std::shared_ptr<const VertexDeclaration>& GetVertexDeclaration() const;

			inline bool IsValid() const;

			void* Map(BufferAccess access, std::size_t startVertex = 0, std::size_t length = 0);
			void* Map(BufferAccess access, std::size_t startVertex = 0, std::size_t length = 0) const;
			void* MapRaw(BufferAccess access, std::size_t offset = 0, std::size_t size = 0);
			void* MapRaw(BufferAccess access, std::size_t offset = 0, std::size_t size = 0) const;

			void Reset();
			void Reset(std::shared_ptr<const VertexDeclaration> vertexDeclaration, std::shared_ptr<Buffer> buffer);
			void Reset(std::shared_ptr<const VertexDeclaration> vertexDeclaration, std::shared_ptr<Buffer> buffer, std::size_t offset, std::size_t size);
			void Reset(std::shared_ptr<const VertexDeclaration> vertexDeclaration, std::size_t length, DataStorage storage, BufferUsageFlags usage);
			void Reset(const VertexBuffer& vertexBuffer);

			void SetVertexDeclaration(std::shared_ptr<const VertexDeclaration> vertexDeclaration);

			void Unmap() const;

			VertexBuffer& operator=(const VertexBuffer&) = default;
			VertexBuffer& operator=(VertexBuffer&&) noexcept = default;

		private:
			std::shared_ptr<Buffer> m_buffer;
			std::shared_ptr<const VertexDeclaration> m_vertexDeclaration;
			std::size_t m_endOffset;
			std::size_t m_startOffset;
			std::size_t m_vertexCount;
	};
}

#include <Nazara/Utility/VertexBuffer.inl>

#endif // NAZARA_UTILITY_VERTEXBUFFER_HPP
