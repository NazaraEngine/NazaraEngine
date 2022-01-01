// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_UNIFORMBUFFER_HPP
#define NAZARA_UTILITY_UNIFORMBUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Utility/Buffer.hpp>

namespace Nz
{
	class NAZARA_UTILITY_API UniformBuffer
	{
		public:
			UniformBuffer() = default;
			UniformBuffer(std::shared_ptr<Buffer> buffer);
			UniformBuffer(std::shared_ptr<Buffer> buffer, UInt32 offset, UInt32 size);
			UniformBuffer(UInt32 length, DataStorage storage, BufferUsageFlags usage);
			UniformBuffer(const UniformBuffer&) = default;
			UniformBuffer(UniformBuffer&&) noexcept = default;
			~UniformBuffer() = default;

			bool Fill(const void* data, UInt32 offset, UInt32 size);

			inline const std::shared_ptr<Buffer>& GetBuffer() const;
			inline UInt32 GetEndOffset() const;
			inline UInt32 GetStartOffset() const;

			inline bool IsValid() const;

			void* Map(BufferAccess access, UInt32 offset = 0, UInt32 size = 0);
			void* Map(BufferAccess access, UInt32 offset = 0, UInt32 size = 0) const;

			void Reset();
			void Reset(std::shared_ptr<Buffer> buffer);
			void Reset(std::shared_ptr<Buffer> buffer, UInt32 offset, UInt32 size);
			void Reset(UInt32 size, DataStorage storage, BufferUsageFlags usage);
			void Reset(const UniformBuffer& uniformBuffer);

			void Unmap() const;

			UniformBuffer& operator=(const UniformBuffer&) = default;
			UniformBuffer& operator=(UniformBuffer&&) noexcept = default;

		private:
			std::shared_ptr<Buffer> m_buffer;
			UInt32 m_endOffset;
			UInt32 m_startOffset;
	};
}

#include <Nazara/Utility/UniformBuffer.inl>

#endif // NAZARA_UTILITY_UNIFORMBUFFER_HPP
