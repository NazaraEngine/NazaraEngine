// Copyright (C) 2022 Full Cycle Games
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Uuid.hpp>
#include <ostream>

#ifdef NAZARA_PLATFORM_WINDOWS
#include <objbase.h>
#elif defined(NAZARA_PLATFORM_LINUX) || defined(NAZARA_PLATFORM_MACOSX)
#include <uuid/uuid.h>
#endif

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	std::array<char, 37> Uuid::ToStringArray() const
	{
		std::array<char, 37> guidStr; //< Including \0
		std::snprintf(guidStr.data(), guidStr.size(), "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
		              m_guid[0], m_guid[1], m_guid[2], m_guid[3], m_guid[4], m_guid[5], m_guid[6], m_guid[7],
		              m_guid[8], m_guid[9], m_guid[10], m_guid[11], m_guid[12], m_guid[13], m_guid[14], m_guid[15]);

		return guidStr;
	}

	Uuid Uuid::Generate()
	{
		std::array<UInt8, 16> guid;

#ifdef NAZARA_PLATFORM_WINDOWS
		GUID id;
		CoCreateGuid(&id);

		for (unsigned int i = 0; i < 4; ++i)
			guid[i] = static_cast<UInt8>(id.Data1 >> ((3 - i) * 8 & 0xFF));

		for (unsigned int i = 0; i < 2; ++i)
			guid[4 + i] = static_cast<UInt8>(id.Data2 >> ((1 - i) * 8 & 0xFF));

		for (unsigned int i = 0; i < 2; ++i)
			guid[6 + i] = static_cast<UInt8>(id.Data3 >> ((1 - i) * 8 & 0xFF));

		for (unsigned int i = 0; i < 8; ++i)
			guid[8 + i] = static_cast<UInt8>(id.Data4[i]);
#elif defined(NAZARA_PLATFORM_LINUX) || defined(NAZARA_PLATFORM_MACOSX)
		uuid_t id;
		uuid_generate(id);

		std::copy(std::begin(id), std::end(id), guid.begin());
#else
#error Missing platform support
#endif

		return guid;
	}

	std::ostream& operator<<(std::ostream& out, const Uuid& guid)
	{
		std::array<char, 37> guidStr = guid.ToStringArray();

		return out << guidStr.data();
	}
}
