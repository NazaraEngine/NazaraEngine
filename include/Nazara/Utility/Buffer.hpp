// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BUFFER_HPP
#define NAZARA_BUFFER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Utility/AbstractBuffer.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <array>

namespace Nz
{
	class Buffer;

	using BufferConstRef = ObjectRef<const Buffer>;
	using BufferRef = ObjectRef<Buffer>;

	class NAZARA_UTILITY_API Buffer : public RefCounted
	{
		friend class Utility;

		public:
			using BufferFactory = AbstractBuffer* (*)(Buffer* parent, BufferType type);

			Buffer(BufferType type);
			Buffer(BufferType type, UInt32 size, DataStorage storage = DataStorage_Software, BufferUsageFlags usage = 0);
			Buffer(const Buffer&) = delete;
			Buffer(Buffer&&) = default;
			~Buffer();

			bool CopyContent(const BufferRef& buffer);

			bool Create(UInt32 size, DataStorage storage = DataStorage_Software, BufferUsageFlags usage = 0);
			void Destroy();

			bool Fill(const void* data, UInt32 offset, UInt32 size);

			inline AbstractBuffer* GetImpl() const;
			inline UInt32 GetSize() const;
			inline DataStorage GetStorage() const;
			inline BufferType GetType() const;
			inline BufferUsageFlags GetUsage() const;

			inline bool HasStorage(DataStorage storage) const;

			inline bool IsValid() const;

			void* Map(BufferAccess access, UInt32 offset = 0, UInt32 size = 0);
			void* Map(BufferAccess access, UInt32 offset = 0, UInt32 size = 0) const;

			bool SetStorage(DataStorage storage);

			void Unmap() const;

			Buffer& operator=(const Buffer&) = delete;
			Buffer& operator=(Buffer&&) = default;

			static bool IsStorageSupported(DataStorage storage);
			template<typename... Args> static BufferRef New(Args&&... args);
			static void SetBufferFactory(DataStorage storage, BufferFactory func);

			// Signals:
			NazaraSignal(OnBufferDestroy, const Buffer* /*buffer*/);
			NazaraSignal(OnBufferRelease, const Buffer* /*buffer*/);

		private:
			static bool Initialize();
			static void Uninitialize();

			std::unique_ptr<AbstractBuffer> m_impl;
			BufferType m_type;
			BufferUsageFlags m_usage;
			UInt32 m_size;

			static std::array<BufferFactory, DataStorage_Max + 1> s_bufferFactories;
	};
}

#include <Nazara/Utility/Buffer.inl>

#endif // NAZARA_BUFFER_HPP
