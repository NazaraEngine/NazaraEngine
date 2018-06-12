// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UNIFORMBUFFER_HPP
#define NAZARA_UNIFORMBUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Utility/Buffer.hpp>

namespace Nz
{
	class UniformBuffer;

	using UniformBufferConstRef = ObjectRef<const UniformBuffer>;
	using UniformBufferRef = ObjectRef<UniformBuffer>;

	class NAZARA_UTILITY_API UniformBuffer : public RefCounted
	{
		public:
			UniformBuffer() = default;
			UniformBuffer(BufferRef buffer);
			UniformBuffer(BufferRef buffer, UInt32 offset, UInt32 size);
			UniformBuffer(UInt32 length, DataStorage storage, BufferUsageFlags usage);
			UniformBuffer(const UniformBuffer& uniformBuffer);
			UniformBuffer(UniformBuffer&&) = delete;
			~UniformBuffer();

			bool Fill(const void* data, UInt32 offset, UInt32 size);

			inline const BufferRef& GetBuffer() const;
			inline UInt32 GetEndOffset() const;
			inline UInt32 GetStartOffset() const;

			inline bool IsValid() const;

			void* Map(BufferAccess access, UInt32 offset = 0, UInt32 size = 0);
			void* Map(BufferAccess access, UInt32 offset = 0, UInt32 size = 0) const;

			void Reset();
			void Reset(BufferRef buffer);
			void Reset(BufferRef buffer, UInt32 offset, UInt32 size);
			void Reset(UInt32 size, DataStorage storage, BufferUsageFlags usage);
			void Reset(const UniformBuffer& uniformBuffer);

			void Unmap() const;

			UniformBuffer& operator=(const UniformBuffer& uniformBuffer);
			UniformBuffer& operator=(UniformBuffer&&) = delete;

			template<typename... Args> static UniformBufferRef New(Args&&... args);

			// Signals:
			NazaraSignal(OnUniformBufferRelease, const UniformBuffer* /*UniformBuffer*/);

		private:
			BufferRef m_buffer;
			UInt32 m_endOffset;
			UInt32 m_startOffset;
	};
}

#include <Nazara/Utility/UniformBuffer.inl>

#endif // NAZARA_UNIFORMBUFFER_HPP
