// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BUFFER_HPP
#define NAZARA_BUFFER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Enums.hpp>

namespace Nz
{
	class Buffer;

	using BufferConstRef = ObjectRef<const Buffer>;
	using BufferRef = ObjectRef<Buffer>;

	class AbstractBuffer;

	class NAZARA_UTILITY_API Buffer : public RefCounted
	{
		friend class Utility;

		public:
			using BufferFactory = AbstractBuffer* (*)(Buffer* parent, BufferType type);

			Buffer(BufferType type);
			Buffer(BufferType type, unsigned int size, UInt32 storage = DataStorage_Software, BufferUsage usage = BufferUsage_Static);
			Buffer(const Buffer&) = delete;
			Buffer(Buffer&&) = delete;
			~Buffer();

			bool CopyContent(const Buffer& buffer);

			bool Create(unsigned int size, UInt32 storage = DataStorage_Software, BufferUsage usage = BufferUsage_Static);
			void Destroy();

			bool Fill(const void* data, unsigned int offset, unsigned int size, bool forceDiscard = false);

			AbstractBuffer* GetImpl() const;
			unsigned int GetSize() const;
			UInt32 GetStorage() const;
			BufferType GetType() const;
			BufferUsage GetUsage() const;

			bool IsHardware() const;
			bool IsValid() const;

			void* Map(BufferAccess access, unsigned int offset = 0, unsigned int size = 0);
			void* Map(BufferAccess access, unsigned int offset = 0, unsigned int size = 0) const;

			bool SetStorage(UInt32 storage);

			void Unmap() const;

			Buffer& operator=(const Buffer&) = delete;
			Buffer& operator=(Buffer&&) = delete;

			static bool IsStorageSupported(UInt32 storage);
			template<typename... Args> static BufferRef New(Args&&... args);
			static void SetBufferFactory(UInt32 storage, BufferFactory func);

			// Signals:
			NazaraSignal(OnBufferDestroy, const Buffer* /*buffer*/);
			NazaraSignal(OnBufferRelease, const Buffer* /*buffer*/);

		private:
			static bool Initialize();
			static void Uninitialize();

			BufferType m_type;
			BufferUsage m_usage;
			UInt32 m_storage;
			AbstractBuffer* m_impl;
			unsigned int m_size;

			static BufferFactory s_bufferFactories[DataStorage_Max+1];
	};
}

#include <Nazara/Utility/Buffer.inl>

#endif // NAZARA_BUFFER_HPP
