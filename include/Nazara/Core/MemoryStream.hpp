// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MEMORYSTREAM_HPP
#define NAZARA_MEMORYSTREAM_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/InputStream.hpp>

class NAZARA_API NzMemoryStream : public NzInputStream
{
	public:
		NzMemoryStream(const void* ptr, nzUInt64 size);
		~NzMemoryStream();

		bool EndOfStream() const;

		nzUInt64 GetCursorPos() const;
		nzUInt64 GetSize() const;

		std::size_t Read(void* buffer, std::size_t size);

		bool SetCursorPos(nzUInt64 offset);

	private:
		const nzUInt8* m_ptr;
		nzUInt64 m_pos;
		nzUInt64 m_size;
};

#endif // NAZARA_MEMORYSTREAM_HPP
