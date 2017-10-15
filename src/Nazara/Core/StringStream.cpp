// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <array>
#include <limits>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::StringStream
	* \brief Core class that represents a stream of strings
	*/

	/*!
	* \brief Constructs a StringStream object with a string
	*
	* \param str First value of the stream
	*/
	StringStream::StringStream(String str) :
	m_result(std::move(str))
	{
	}

	/*!
	* \brief Resets the state of the stream, erasing every contained text
	*/
	void StringStream::Clear()
	{
		m_result.Clear();
	}

	/*!
	* \brief Get the current buffer size
	* \return The internal accumulation buffer size, this is equivalent to the size of the final string
	*/
	std::size_t StringStream::GetBufferSize() const
	{
		return m_result.GetSize();
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object where every objects of the stream has been converted with Nz::String
	*/
	String StringStream::ToString() const
	{
		return m_result;
	}

	/*!
	* \brief Adds the representation of the boolean
	* \return A reference to this
	*
	* \param boolean Boolean value
	*/
	StringStream& StringStream::operator<<(bool boolean)
	{
		std::size_t size = (boolean) ? 4 : 5;
		std::size_t start = m_result.GetSize();
		m_result.Resize(start + size);
		std::memcpy(&m_result[start], (boolean) ? "true" : "false", size);

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
		constexpr std::size_t maxSize = std::numeric_limits<short>::digits10 + 2;
		std::size_t start = m_result.GetSize();
		m_result.Resize(start + maxSize);
		std::size_t written = std::snprintf(&m_result[start], maxSize + 1, "%hd", number);
		m_result.Resize(start + written);

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
		constexpr std::size_t maxSize = std::numeric_limits<unsigned short>::digits10 + 1;
		std::size_t start = m_result.GetSize();
		m_result.Resize(start + maxSize);
		std::size_t written = std::snprintf(&m_result[start], maxSize + 1, "%hu", number);
		m_result.Resize(start + written);

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
		constexpr std::size_t maxSize = std::numeric_limits<int>::digits10 + 2;
		std::size_t start = m_result.GetSize();
		m_result.Resize(start + maxSize);
		std::size_t written = std::snprintf(&m_result[start], maxSize + 1, "%d", number);
		m_result.Resize(start + written);

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
		constexpr std::size_t maxSize = std::numeric_limits<unsigned int>::digits10 + 1;
		std::size_t start = m_result.GetSize();
		m_result.Resize(start + maxSize);
		std::size_t written = std::snprintf(&m_result[start], maxSize + 1, "%u", number);
		m_result.Resize(start + written);

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
		constexpr std::size_t maxSize = std::numeric_limits<long>::digits10 + 2;
		std::size_t start = m_result.GetSize();
		m_result.Resize(start + maxSize);
		std::size_t written = std::snprintf(&m_result[start], maxSize + 1, "%ld", number);
		m_result.Resize(start + written);

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
		constexpr std::size_t maxSize = std::numeric_limits<unsigned long>::digits10 + 1;
		std::size_t start = m_result.GetSize();
		m_result.Resize(start + maxSize);
		std::size_t written = std::snprintf(&m_result[start], maxSize + 1, "%lu", number);
		m_result.Resize(start + written);


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
		constexpr std::size_t maxSize = std::numeric_limits<long long>::digits10 + 2;
		std::size_t start = m_result.GetSize();
		m_result.Resize(start + maxSize);
		std::size_t written = std::snprintf(&m_result[start], maxSize + 1, "%lld", number);
		m_result.Resize(start + written);

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
		constexpr std::size_t maxSize = std::numeric_limits<unsigned long long>::digits10 + 1;
		std::size_t start = m_result.GetSize();
		m_result.Resize(start + maxSize);
		std::size_t written = std::snprintf(&m_result[start], maxSize + 1, "%llu", number);
		m_result.Resize(start + written);

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
		return operator<<(double(number)); //< snprintf doesn't support float anyway
	}

	/*!
	* \brief Adds the representation of the double
	* \return A reference to this
	*
	* \param number Double value
	*/
	StringStream& StringStream::operator<<(double number)
	{
		// https://stackoverflow.com/questions/1701055/what-is-the-maximum-length-in-chars-needed-to-represent-any-double-value
		const std::size_t maxSize = 3 + std::numeric_limits<double>::digits - std::numeric_limits<double>::min_exponent;

		// Use a temporary buffer to prevent 1kb string capacity growth
		std::array<char, maxSize + 1> buffer;
		std::size_t written = std::snprintf(buffer.data(), buffer.size(), "%f", number);

		std::size_t start = m_result.GetSize();
		m_result.Resize(start + written);
		std::memcpy(&m_result[start], buffer.data(), written);

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
		// https://stackoverflow.com/questions/1701055/what-is-the-maximum-length-in-chars-needed-to-represent-any-double-value
		const std::size_t maxSize = 3 + std::numeric_limits<long double>::digits - std::numeric_limits<long double>::min_exponent;

		// Use a temporary buffer to prevent 1kb string capacity growth
		std::array<char, maxSize + 1> buffer;
		std::size_t written = std::snprintf(buffer.data(), buffer.size(), "%f", number);

		std::size_t start = m_result.GetSize();
		m_result.Resize(start + written);
		std::memcpy(&m_result[start], buffer.data(), written);

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
		m_result.Append(character);
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
		m_result.Append(static_cast<unsigned char>(character));
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
		m_result.Append(string);
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
		m_result.Append(string.data(), string.size());
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
		m_result.Append(string);

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
		constexpr std::size_t maxSize = sizeof(void*) * 2 + 2;
		std::size_t start = m_result.GetSize();
		m_result.Resize(start + maxSize);
		std::size_t written = std::snprintf(&m_result[start], maxSize + 1, "0x%p", ptr);
		m_result.Resize(start + written);

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
