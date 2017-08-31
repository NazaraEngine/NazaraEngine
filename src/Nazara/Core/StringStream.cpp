// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::StringStream
	* \brief Core class that represents a stream of strings
	*/

	/*!
	* \brief Constructs a StringStream object by default
	*/
	StringStream::StringStream() :
	m_bufferSize(0)
	{
	}

	/*!
	* \brief Constructs a StringStream object with a string
	*
	* \param str First value of the stream
	*/
	StringStream::StringStream(const String& str) :
	m_bufferSize(str.GetSize())
	{
		m_strings.push_back(str);
	}

	/*!
	* \brief Resets the state of the stream, erasing every contained text
	*/
	void StringStream::Clear()
	{
		m_bufferSize = 0;
		m_strings.clear();
	}

	/*!
	* \brief Get the current buffer size
	* \return The internal accumulation buffer size, this is equivalent to the size of the final string
	*/
	std::size_t StringStream::GetBufferSize() const
	{
		return m_bufferSize;
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object where every objects of the stream has been converted with Nz::String
	*/
	String StringStream::ToString() const
	{
		String string;
		string.Reserve(m_bufferSize);

		for (const String& str : m_strings)
			string += str;

		return string;
	}

	/*!
	* \brief Adds the representation of the boolean
	* \return A reference to this
	*
	* \param boolean Boolean value
	*/
	StringStream& StringStream::operator<<(bool boolean)
	{
		m_strings.push_back(String::Boolean(boolean));
		m_bufferSize += m_strings.back().GetSize();

		return *this;
	}

	/*!
	* \brief Adds the representation of the short
	* \return A reference to this
	*
	* \param number Short value
	*/
	StringStream& StringStream::operator<<(short number)
	{
		m_strings.push_back(String::Number(number));
		m_bufferSize += m_strings.back().GetSize();

		return *this;
	}

	/*!
	* \brief Adds the representation of the unsigned short
	* \return A reference to this
	*
	* \param number Short value
	*/
	StringStream& StringStream::operator<<(unsigned short number)
	{
		m_strings.push_back(String::Number(number));
		m_bufferSize += m_strings.back().GetSize();

		return *this;
	}

	/*!
	* \brief Adds the representation of the int
	* \return A reference to this
	*
	* \param number Int value
	*/
	StringStream& StringStream::operator<<(int number)
	{
		m_strings.push_back(String::Number(number));
		m_bufferSize += m_strings.back().GetSize();

		return *this;
	}

	/*!
	* \brief Adds the representation of the unsigned int
	* \return A reference to this
	*
	* \param number Int value
	*/
	StringStream& StringStream::operator<<(unsigned int number)
	{
		m_strings.push_back(String::Number(number));
		m_bufferSize += m_strings.back().GetSize();

		return *this;
	}

	/*!
	* \brief Adds the representation of the long
	* \return A reference to this
	*
	* \param number Long value
	*/
	StringStream& StringStream::operator<<(long number)
	{
		m_strings.push_back(String::Number(number));
		m_bufferSize += m_strings.back().GetSize();

		return *this;
	}

	/*!
	* \brief Adds the representation of the unsigned long
	* \return A reference to this
	*
	* \param number Long value
	*/
	StringStream& StringStream::operator<<(unsigned long number)
	{
		m_strings.push_back(String::Number(number));
		m_bufferSize += m_strings.back().GetSize();

		return *this;
	}

	/*!
	* \brief Adds the representation of the long long
	* \return A reference to this
	*
	* \param number Long long value
	*/
	StringStream& StringStream::operator<<(long long number)
	{
		m_strings.push_back(String::Number(number));
		m_bufferSize += m_strings.back().GetSize();

		return *this;
	}

	/*!
	* \brief Adds the representation of the unsigned long long
	* \return A reference to this
	*
	* \param number Long long value
	*/
	StringStream& StringStream::operator<<(unsigned long long number)
	{
		m_strings.push_back(String::Number(number));
		m_bufferSize += m_strings.back().GetSize();

		return *this;
	}

	/*!
	* \brief Adds the representation of the float
	* \return A reference to this
	*
	* \param number Float value
	*/
	StringStream& StringStream::operator<<(float number)
	{
		m_strings.push_back(String::Number(number));
		m_bufferSize += m_strings.back().GetSize();

		return *this;
	}

	/*!
	* \brief Adds the representation of the double
	* \return A reference to this
	*
	* \param number Double value
	*/
	StringStream& StringStream::operator<<(double number)
	{
		m_strings.push_back(String::Number(number));
		m_bufferSize += m_strings.back().GetSize();

		return *this;
	}

	/*!
	* \brief Adds the representation of the long double
	* \return A reference to this
	*
	* \param number Long double value
	*/
	StringStream& StringStream::operator<<(long double number)
	{
		m_strings.push_back(String::Number(number));
		m_bufferSize += m_strings.back().GetSize();

		return *this;
	}

	/*!
	* \brief Adds the representation of the char
	* \return A reference to this
	*
	* \param character Char value
	*/
	StringStream& StringStream::operator<<(char character)
	{
		m_strings.push_back(String(character));
		m_bufferSize++;

		return *this;
	}

	/*!
	* \brief Adds the representation of the unsigned char
	* \return A reference to this
	*
	* \param character Char value
	*/
	StringStream& StringStream::operator<<(unsigned char character)
	{
		m_strings.push_back(String(static_cast<char>(character)));
		m_bufferSize++;

		return *this;
	}

	/*!
	* \brief Adds the representation of the const char*
	* \return A reference to this
	*
	* \param string String value
	*/
	StringStream& StringStream::operator<<(const char* string)
	{
		m_strings.push_back(string);
		m_bufferSize += m_strings.back().GetSize();

		return *this;
	}

	/*!
	* \brief Adds the representation of the std::string
	* \return A reference to this
	*
	* \param string String value
	*/
	StringStream& StringStream::operator<<(const std::string& string)
	{
		m_strings.push_back(string);
		m_bufferSize += string.size();

		return *this;
	}

	/*!
	* \brief Adds the representation of the Nz::String
	* \return A reference to this
	*
	* \param string String value
	*/
	StringStream& StringStream::operator<<(const String& string)
	{
		m_strings.push_back(string);
		m_bufferSize += string.GetSize();

		return *this;
	}

	/*!
	* \brief Adds the representation of the pointer
	* \return A reference to this
	*
	* \param ptr Pointer value
	*/
	StringStream& StringStream::operator<<(const void* ptr)
	{
		m_strings.push_back(String::Pointer(ptr));
		m_bufferSize += m_strings.back().GetSize();

		return *this;
	}

	/*!
	* \brief Converts this to Nz::String
	* \return The string representation of the stream
	*/
	StringStream::operator String() const
	{
		return ToString();
	}
}
