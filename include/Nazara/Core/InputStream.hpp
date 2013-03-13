// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_INPUTSTREAM_HPP
#define NAZARA_INPUTSTREAM_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Stream.hpp>

class NAZARA_API NzInputStream : public NzStream
{
	public:
		virtual ~NzInputStream();

		virtual bool EndOfStream() const = 0;

		virtual nzUInt64 GetSize() const = 0;

		virtual std::size_t Read(void* buffer, std::size_t size) = 0;
		virtual NzString ReadLine(unsigned int lineSize = 0);
};

#endif // NAZARA_INPUTSTREAM_HPP
