// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \brief Constructs a String object with a shared string by move semantic
	*
	* \param sharedString Shared string to move into this
	*/

	inline String::String(std::shared_ptr<SharedString>&& sharedString) :
	m_sharedString(std::move(sharedString))
	{
	}

	/*!
	* \brief Build a string using a format and returns it
	* \return Formatted string
	*
	* \param format String format
	* \param ... Format arguments
	*/
	String String::Format(const char* format, ...)
	{
		va_list args;
		va_start(args, format);
		String result = FormatVA(format, args);
		va_end(args);

		return result;
	}

	/*!
	* \brief Releases the content to the string
	*/

	inline void String::ReleaseString()
	{
		m_sharedString = std::move(GetEmptyString());
	}

	/*!
	* \brief Constructs a SharedString object by default
	*/

	inline String::SharedString::SharedString() : // Special case: empty string
	capacity(0),
	size(0)
	{
	}

	/*!
	* \brief Constructs a SharedString object with a size
	*
	* \param strSize Number of characters in the string
	*/

	inline String::SharedString::SharedString(std::size_t strSize) :
	capacity(strSize),
	size(strSize),
	string(new char[strSize + 1])
	{
		string[strSize] = '\0';
	}

	/*!
	* \brief Constructs a SharedString object with a size and a capacity
	*
	* \param strSize Number of characters in the string
	* \param strCapacity Capacity in characters in the string
	*/

	inline String::SharedString::SharedString(std::size_t strSize, std::size_t strCapacity) :
	capacity(strCapacity),
	size(strSize),
	string(new char[strCapacity + 1])
	{
		string[strSize] = '\0';
	}

	/*!
	* \brief Appends the string to the hash
	* \return true if hash is successful
	*
	* \param hash Hash to append data of the file
	* \param string String to hash
	*/

	inline bool HashAppend(AbstractHash* hash, const String& string)
	{
		hash->Append(reinterpret_cast<const UInt8*>(string.GetConstBuffer()), string.GetSize());
		return true;
	}
}

namespace std
{
	template<>
	struct hash<Nz::String>
	{
		/*!
		* \brief Specialisation of std to hash
		* \return Result of the hash
		*
		* \param str String to hash
		*/
		size_t operator()(const Nz::String& str) const
		{
			// Algorithme DJB2
			// http://www.cse.yorku.ca/~oz/hash.html

			size_t h = 5381;
			if (!str.IsEmpty())
			{
				const char* ptr = str.GetConstBuffer();

				do
					h = ((h << 5) + h) + static_cast<size_t>(*ptr);
				while (*++ptr);
			}

			return h;
		}
	};
}

#include <Nazara/Core/DebugOff.hpp>
