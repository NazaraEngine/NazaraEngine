// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	StringStream::StringStream() :
	m_bufferSize(0)
	{
	}

	StringStream::StringStream(const String& str) :
	m_bufferSize(str.GetSize())
	{
		m_strings.push_back(str);
	}

	String StringStream::ToString() const
	{
		String string;
		string.Reserve(m_bufferSize);

		for (const String& str : m_strings)
			string += str;

		return string;
	}

	StringStream& StringStream::operator<<(bool boolean)
	{
		m_strings.push_back(String::Boolean(boolean));
		m_bufferSize += m_strings.back().GetSize();

		return *this;
	}

	StringStream& StringStream::operator<<(short number)
	{
		m_strings.push_back(String::Number(number));
		m_bufferSize += m_strings.back().GetSize();

		return *this;
	}

	StringStream& StringStream::operator<<(unsigned short number)
	{
		m_strings.push_back(String::Number(number));
		m_bufferSize += m_strings.back().GetSize();

		return *this;
	}

	StringStream& StringStream::operator<<(int number)
	{
		m_strings.push_back(String::Number(number));
		m_bufferSize += m_strings.back().GetSize();

		return *this;
	}

	StringStream& StringStream::operator<<(unsigned int number)
	{
		m_strings.push_back(String::Number(number));
		m_bufferSize += m_strings.back().GetSize();

		return *this;
	}

	StringStream& StringStream::operator<<(long number)
	{
		m_strings.push_back(String::Number(number));
		m_bufferSize += m_strings.back().GetSize();

		return *this;
	}

	StringStream& StringStream::operator<<(unsigned long number)
	{
		m_strings.push_back(String::Number(number));
		m_bufferSize += m_strings.back().GetSize();

		return *this;
	}

	StringStream& StringStream::operator<<(long long number)
	{
		m_strings.push_back(String::Number(number));
		m_bufferSize += m_strings.back().GetSize();

		return *this;
	}

	StringStream& StringStream::operator<<(unsigned long long number)
	{
		m_strings.push_back(String::Number(number));
		m_bufferSize += m_strings.back().GetSize();

		return *this;
	}

	StringStream& StringStream::operator<<(float number)
	{
		m_strings.push_back(String::Number(number));
		m_bufferSize += m_strings.back().GetSize();

		return *this;
	}

	StringStream& StringStream::operator<<(double number)
	{
		m_strings.push_back(String::Number(number));
		m_bufferSize += m_strings.back().GetSize();

		return *this;
	}

	StringStream& StringStream::operator<<(long double number)
	{
		m_strings.push_back(String::Number(number));
		m_bufferSize += m_strings.back().GetSize();

		return *this;
	}

	StringStream& StringStream::operator<<(char character)
	{
		m_strings.push_back(String(character));
		m_bufferSize++;

		return *this;
	}

	StringStream& StringStream::operator<<(unsigned char character)
	{
		m_strings.push_back(String(static_cast<char>(character)));
		m_bufferSize++;

		return *this;
	}

	StringStream& StringStream::operator<<(const char* string)
	{
		m_strings.push_back(string);
		m_bufferSize += m_strings.back().GetSize();

		return *this;
	}

	StringStream& StringStream::operator<<(const std::string& string)
	{
		m_strings.push_back(string);
		m_bufferSize += string.size();

		return *this;
	}

	StringStream& StringStream::operator<<(const String& string)
	{
		m_strings.push_back(string);
		m_bufferSize += string.GetSize();

		return *this;
	}

	StringStream& StringStream::operator<<(const void* ptr)
	{
		m_strings.push_back(String::Pointer(ptr));
		m_bufferSize += m_strings.back().GetSize();

		return *this;
	}

	StringStream::operator String() const
	{
		return ToString();
	}
}
