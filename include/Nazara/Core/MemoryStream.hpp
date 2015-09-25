// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MEMORYSTREAM_HPP
#define NAZARA_MEMORYSTREAM_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/InputStream.hpp>

namespace Nz
{
	class NAZARA_CORE_API MemoryStream : public InputStream
	{
		public:
			MemoryStream(const void* ptr, UInt64 size);
			MemoryStream(const MemoryStream&) = delete;
			MemoryStream(MemoryStream&&) = delete; ///TODO
			~MemoryStream();

			bool EndOfStream() const;

			UInt64 GetCursorPos() const;
			UInt64 GetSize() const;

			std::size_t Read(void* buffer, std::size_t size);

			bool SetCursorPos(UInt64 offset);

			MemoryStream& operator=(const MemoryStream&) = delete;
			MemoryStream& operator=(MemoryStream&&) = delete; ///TODO

		private:
			const UInt8* m_ptr;
			UInt64 m_pos;
			UInt64 m_size;
	};
}

#endif // NAZARA_MEMORYSTREAM_HPP
