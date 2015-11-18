// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline String::String(std::shared_ptr<SharedString>&& sharedString) :
	m_sharedString(std::move(sharedString))
	{
	}

	inline void String::ReleaseString()
	{
		m_sharedString = std::move(GetEmptyString());
	}

	inline String::SharedString::SharedString() : // Special case: empty string
	capacity(0),
	size(0)
	{
	}

	inline String::SharedString::SharedString(unsigned int strSize) :
	capacity(strSize), 
	size(strSize),
	string(new char[strSize + 1])
	{
		string[strSize] = '\0';
	}

	inline String::SharedString::SharedString(unsigned int strSize, unsigned int strCapacity) :
	capacity(strCapacity),
	size(strSize),
	string(new char[strCapacity + 1])
	{
		string[strSize] = '\0';
	}

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
		size_t operator()(const Nz::String& str) const
		{
			// Algorithme DJB2
			// http://www.cse.yorku.ca/~oz/hash.html

			size_t h = 5381;
			if (!str.IsEmpty())
			{
				const char* ptr = str.GetConstBuffer();

				do
					h = ((h << 5) + h) + *ptr;
				while (*++ptr);
			}

			return h;
		}
	};
}

#include <Nazara/Core/DebugOff.hpp>
