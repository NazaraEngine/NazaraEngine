// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_BUFFER_HPP
#define NAZARA_CORE_BUFFER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/Export.hpp>
#include <functional>
#include <memory>

namespace Nz
{
	class Buffer;

	using BufferFactory = std::function<std::shared_ptr<Buffer>(BufferType type, UInt64 size, BufferUsageFlags usage, const void* initialData)>;

	class NAZARA_CORE_API Buffer
	{
		public:
			inline Buffer(DataStorage storage, BufferType type, UInt64 size, BufferUsageFlags usage);
			Buffer(const Buffer&) = delete;
			Buffer(Buffer&&) = delete;
			virtual ~Buffer();

			std::shared_ptr<Buffer> CopyContent(const BufferFactory& bufferFactory);

			virtual bool Fill(const void* data, UInt64 offset, UInt64 size) = 0;

			inline UInt64 GetSize() const;
			inline DataStorage GetStorage() const;
			inline BufferType GetType() const;
			inline BufferUsageFlags GetUsageFlags() const;

			virtual void* Map(UInt64 offset, UInt64 size) = 0;
			virtual bool Unmap() = 0;

			Buffer& operator=(const Buffer&) = delete;
			Buffer& operator=(Buffer&&) = delete;

		private:
			BufferType m_type;
			BufferUsageFlags m_usage;
			DataStorage m_storage;
			UInt64 m_size;
	};
}

#include <Nazara/Core/Buffer.inl>

#endif // NAZARA_CORE_BUFFER_HPP
