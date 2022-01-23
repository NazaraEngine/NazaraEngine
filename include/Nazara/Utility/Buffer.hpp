// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_BUFFER_HPP
#define NAZARA_UTILITY_BUFFER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Enums.hpp>
#include <functional>
#include <memory>

namespace Nz
{
	class Buffer;

	using BufferFactory = std::function<std::shared_ptr<Buffer>(BufferType type, UInt64 size, BufferUsageFlags usage, const void* initialData)>;

	class NAZARA_UTILITY_API Buffer
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

#include <Nazara/Utility/Buffer.inl>

#endif // NAZARA_UTILITY_BUFFER_HPP
