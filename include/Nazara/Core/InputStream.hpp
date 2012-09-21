// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_INPUTSTREAM_HPP
#define NAZARA_INPUTSTREAM_HPP

#include <Nazara/Prerequesites.hpp>

class NzString;

class NzInputStream
{
	public:
		virtual ~NzInputStream();

		virtual bool EndOfStream() const = 0;

		virtual nzUInt64 GetCursorPos() const = 0;
		virtual NzString GetLine(unsigned int lineSize = 0);
		virtual nzUInt64 GetSize() const = 0;

		virtual std::size_t Read(void* buffer, std::size_t size) = 0;

		virtual bool SetCursorPos(nzUInt64 offset) = 0;
};

#endif // NAZARA_INPUTSTREAM_HPP
