// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

namespace std
{
    template<>
    struct hash<NzString>
    {
		public:
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
