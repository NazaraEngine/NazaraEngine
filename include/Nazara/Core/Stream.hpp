// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_STREAM_HPP
#define NAZARA_STREAM_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/String.hpp>

class NAZARA_API NzStream
{
	public:
		NzStream() = default;
		virtual ~NzStream();

		virtual nzUInt64 GetCursorPos() const = 0;
		virtual NzString GetDirectory() const;
		virtual NzString GetPath() const;
		unsigned int GetStreamOptions() const;

		virtual bool SetCursorPos(nzUInt64 offset) = 0;
		void SetStreamOptions(unsigned int options);

	protected:
		unsigned int m_streamOptions = 0;
};

#endif // NAZARA_STREAM_HPP
