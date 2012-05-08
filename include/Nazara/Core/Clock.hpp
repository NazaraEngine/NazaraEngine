// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CLOCK_HPP
#define NAZARA_CLOCK_HPP

#include <Nazara/Prerequesites.hpp>

#define NAZARA_CLASS_CLOCK
#include <Nazara/Core/ThreadSafety.hpp>

class NAZARA_API NzClock
{
	public:
		NzClock();

		float GetSeconds() const;
		nzUInt64 GetMicroseconds() const;
		nzUInt64 GetMilliseconds() const;

		bool IsPaused() const;

		void Pause();
		void Restart();
		void Unpause();

	private:
		NazaraMutexAttrib(m_mutex, mutable)

		nzUInt64 m_elapsedTime;
		nzUInt64 m_refTime;
		bool m_paused;
};

typedef nzUInt64 (*NzClockFunction)();

extern NAZARA_API NzClockFunction NzGetMicroseconds;
extern NAZARA_API NzClockFunction NzGetMilliseconds;

#undef NAZARA_CLASS_CLOCK

#endif // NAZARA_CLOCK_HPP
