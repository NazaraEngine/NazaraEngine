// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Uuid.hpp>
#include <ostream>

#ifdef NAZARA_PLATFORM_WINDOWS
#include <objbase.h>
#elif defined(NAZARA_PLATFORM_LINUX) || defined(NAZARA_PLATFORM_MACOS) || defined(NAZARA_PLATFORM_WEB) || defined(NAZARA_PLATFORM_BSD)
#include <uuid/uuid.h>
#endif


namespace Nz
{
	std::array<char, 37> Uuid::ToStringArray() const
	{
		std::array<char, 37> uuidStr; //< Including \0
		std::snprintf(uuidStr.data(), uuidStr.size(), "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
		              m_uuid[0], m_uuid[1], m_uuid[2], m_uuid[3], m_uuid[4], m_uuid[5], m_uuid[6], m_uuid[7],
		              m_uuid[8], m_uuid[9], m_uuid[10], m_uuid[11], m_uuid[12], m_uuid[13], m_uuid[14], m_uuid[15]);

		return uuidStr;
	}

	Uuid Uuid::Generate()
	{
		std::array<UInt8, 16> uuid;

#ifdef NAZARA_PLATFORM_WINDOWS
		GUID id;
		CoCreateGuid(&id);

		for (unsigned int i = 0; i < 4; ++i)
			uuid[i] = static_cast<UInt8>(id.Data1 >> ((3 - i) * 8 & 0xFF));

		for (unsigned int i = 0; i < 2; ++i)
			uuid[4 + i] = static_cast<UInt8>(id.Data2 >> ((1 - i) * 8 & 0xFF));

		for (unsigned int i = 0; i < 2; ++i)
			uuid[6 + i] = static_cast<UInt8>(id.Data3 >> ((1 - i) * 8 & 0xFF));

		for (unsigned int i = 0; i < 8; ++i)
			uuid[8 + i] = static_cast<UInt8>(id.Data4[i]);
#elif defined(NAZARA_PLATFORM_LINUX) || defined(NAZARA_PLATFORM_MACOS) || defined(NAZARA_PLATFORM_WEB) || defined(NAZARA_PLATFORM_BSD)
		uuid_t id;
		uuid_generate(id);

		std::copy(std::begin(id), std::end(id), uuid.begin());
#else
#error Missing platform support
#endif

		return uuid;
	}
}
