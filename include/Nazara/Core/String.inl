// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

inline NzString::NzString(std::shared_ptr<SharedString>&& sharedString) :
m_sharedString(std::move(sharedString))
{
}

inline void NzString::ReleaseString()
{
	m_sharedString = std::move(GetEmptyString());
}

inline NzString::SharedString::SharedString() : // Special case: empty string
capacity(0),
size(0)
{
}

inline NzString::SharedString::SharedString(unsigned int strSize) :
capacity(strSize),
size(strSize),
string(new char[strSize + 1])
{
	string[strSize] = '\0';
}


namespace std
{
	template<>
	struct hash<NzString>
	{
		size_t operator()(const NzString& str) const
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
