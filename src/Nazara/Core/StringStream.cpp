// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Core/Debug.hpp>

NzStringStream::NzStringStream() :
m_bufferSize(0)
{
}

NzStringStream::NzStringStream(const NzString& str) :
m_bufferSize(str.GetSize())
{
	m_strings.push_back(str);
}

NzString NzStringStream::ToString() const
{
	NazaraLock(m_mutex)

	NzString string;
	string.Reserve(m_bufferSize);

	for (const NzString& str : m_strings)
		string += str;

	return string;
}

NzStringStream& NzStringStream::operator<<(bool boolean)
{
	NazaraLock(m_mutex)

	m_strings.push_back(NzString::Boolean(boolean));
	m_bufferSize += m_strings.back().GetSize();

	return *this;
}

NzStringStream& NzStringStream::operator<<(short number)
{
	NazaraLock(m_mutex)

	m_strings.push_back(NzString::Number(number));
	m_bufferSize += m_strings.back().GetSize();

	return *this;
}

NzStringStream& NzStringStream::operator<<(unsigned short number)
{
	NazaraLock(m_mutex)

	m_strings.push_back(NzString::Number(number));
	m_bufferSize += m_strings.back().GetSize();

	return *this;
}

NzStringStream& NzStringStream::operator<<(int number)
{
	NazaraLock(m_mutex)

	m_strings.push_back(NzString::Number(number));
	m_bufferSize += m_strings.back().GetSize();

	return *this;
}

NzStringStream& NzStringStream::operator<<(unsigned int number)
{
	NazaraLock(m_mutex)

	m_strings.push_back(NzString::Number(number));
	m_bufferSize += m_strings.back().GetSize();

	return *this;
}

NzStringStream& NzStringStream::operator<<(long number)
{
	NazaraLock(m_mutex)

	m_strings.push_back(NzString::Number(number));
	m_bufferSize += m_strings.back().GetSize();

	return *this;
}

NzStringStream& NzStringStream::operator<<(unsigned long number)
{
	NazaraLock(m_mutex)

	m_strings.push_back(NzString::Number(number));
	m_bufferSize += m_strings.back().GetSize();

	return *this;
}

NzStringStream& NzStringStream::operator<<(long long number)
{
	NazaraLock(m_mutex)

	m_strings.push_back(NzString::Number(number));
	m_bufferSize += m_strings.back().GetSize();

	return *this;
}

NzStringStream& NzStringStream::operator<<(unsigned long long number)
{
	NazaraLock(m_mutex)

	m_strings.push_back(NzString::Number(number));
	m_bufferSize += m_strings.back().GetSize();

	return *this;
}

NzStringStream& NzStringStream::operator<<(float number)
{
	NazaraLock(m_mutex)

	m_strings.push_back(NzString::Number(number));
	m_bufferSize += m_strings.back().GetSize();

	return *this;
}

NzStringStream& NzStringStream::operator<<(double number)
{
	NazaraLock(m_mutex)

	m_strings.push_back(NzString::Number(number));
	m_bufferSize += m_strings.back().GetSize();

	return *this;
}

NzStringStream& NzStringStream::operator<<(long double number)
{
	NazaraLock(m_mutex)

	m_strings.push_back(NzString::Number(number));
	m_bufferSize += m_strings.back().GetSize();

	return *this;
}

NzStringStream& NzStringStream::operator<<(char character)
{
	NazaraLock(m_mutex)

	m_strings.push_back(character);
	m_bufferSize++;

	return *this;
}

NzStringStream& NzStringStream::operator<<(unsigned char character)
{
	NazaraLock(m_mutex)

	m_strings.push_back(character);
	m_bufferSize++;

	return *this;
}

NzStringStream& NzStringStream::operator<<(const char* string)
{
	NazaraLock(m_mutex)

	m_strings.push_back(string);
	m_bufferSize += m_strings.back().GetSize();

	return *this;
}

NzStringStream& NzStringStream::operator<<(const std::string& string)
{
	NazaraLock(m_mutex)

	m_strings.push_back(string);
	m_bufferSize += string.size();

	return *this;
}

NzStringStream& NzStringStream::operator<<(const NzString& string)
{
	NazaraLock(m_mutex)

	m_strings.push_back(string);
	m_bufferSize += string.GetSize();

	return *this;
}

NzStringStream& NzStringStream::operator<<(const void* ptr)
{
	NazaraLock(m_mutex)

	m_strings.push_back(NzString::Pointer(ptr));
	m_bufferSize += sizeof(void*)*2;

	return *this;
}

NzStringStream::operator NzString() const
{
	return ToString();
}
