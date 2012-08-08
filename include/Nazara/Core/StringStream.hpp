// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_STRINGSTREAM_HPP
#define NAZARA_STRINGSTREAM_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <string>
#include <vector>

#if NAZARA_CORE_THREADSAFE && NAZARA_THREADSAFETY_STRINGSTREAM
#include <Nazara/Core/ThreadSafety.hpp>
#else
#include <Nazara/Core/ThreadSafetyOff.hpp>
#endif

class NAZARA_API NzStringStream
{
	public:
		NzStringStream();
		NzStringStream(const NzString& str);

		NzString ToString() const;

		NzStringStream& operator<<(bool boolean);
		NzStringStream& operator<<(short number);
		NzStringStream& operator<<(unsigned short number);
		NzStringStream& operator<<(int number);
		NzStringStream& operator<<(unsigned int number);
		NzStringStream& operator<<(long number);
		NzStringStream& operator<<(unsigned long number);
		NzStringStream& operator<<(long long number);
		NzStringStream& operator<<(unsigned long long number);
		NzStringStream& operator<<(float number);
		NzStringStream& operator<<(double number);
		NzStringStream& operator<<(long double number);
		NzStringStream& operator<<(char character);
		NzStringStream& operator<<(unsigned char character);
		NzStringStream& operator<<(const char* string);
		NzStringStream& operator<<(const std::string& string);
		NzStringStream& operator<<(const NzString& string);
		NzStringStream& operator<<(const void* ptr);

		operator NzString() const;

	private:
		NazaraMutexAttrib(m_mutex, mutable)

		std::vector<NzString> m_strings;
		unsigned int m_bufferSize;
};

#endif // NAZARA_STRINGSTREAM_HPP
