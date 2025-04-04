// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_UNIFORMBUFFER_HPP
#define NAZARA_CORE_UNIFORMBUFFER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Buffer.hpp>

namespace Nz
{
	class NAZARA_CORE_API UniformBuffer
	{
		public:
			UniformBuffer(std::shared_ptr<Buffer> buffer);
			UniformBuffer(std::shared_ptr<Buffer> buffer, UInt64 offset, UInt64 size);
			UniformBuffer(UInt64 size, BufferUsageFlags usage, const BufferFactory& bufferFactory, const void* initialData = nullptr);
			UniformBuffer(const UniformBuffer&) = default;
			UniformBuffer(UniformBuffer&&) noexcept = default;
			~UniformBuffer() = default;

			bool Fill(const void* data, UInt64 offset, UInt64 size);

			inline const std::shared_ptr<Buffer>& GetBuffer() const;
			inline UInt64 GetEndOffset() const;
			inline UInt64 GetStartOffset() const;

			void* Map(UInt64 offset = 0, UInt64 size = 0);
			void* Map(UInt64 offset = 0, UInt64 size = 0) const;

			void Unmap() const;

			UniformBuffer& operator=(const UniformBuffer&) = default;
			UniformBuffer& operator=(UniformBuffer&&) noexcept = default;

		private:
			std::shared_ptr<Buffer> m_buffer;
			UInt64 m_endOffset;
			UInt64 m_startOffset;
	};
}

#include <Nazara/Core/UniformBuffer.inl>

#endif // NAZARA_CORE_UNIFORMBUFFER_HPP
