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
	namespace
	{
		bool ParseHexadecimalPair(Pointer<const char>& str, UInt8& number)
		{
			number = 0;

			for (UInt8 mul : { 0x10, 1 })
			{
				if (*str >= '0' && *str <= '9')
					number += (*str - '0') * mul;
				else if (((*str & 0x5F) >= 'A' && (*str & 0x5F) <= 'F'))
					number += ((*str & 0x5F) - 'A' + 10) * mul;
				else
					return false;

				str++;
			}

			return true;
		}
	}

	std::array<char, 37> Uuid::ToStringArray() const
	{
		std::array<char, 37> uuidStr; //< Including \0
		std::snprintf(uuidStr.data(), uuidStr.size(), "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
		              m_uuid[0], m_uuid[1], m_uuid[2], m_uuid[3], m_uuid[4], m_uuid[5], m_uuid[6], m_uuid[7],
		              m_uuid[8], m_uuid[9], m_uuid[10], m_uuid[11], m_uuid[12], m_uuid[13], m_uuid[14], m_uuid[15]);

		return uuidStr;
	}

	Uuid Uuid::FromString(std::string_view str)
	{
		if (str.size() != 36)
			return {};

		const char* ptr = str.data();

		std::array<UInt8, 16> uuid;
		UInt8* uuidPart = &uuid[0];

		bool first = true;
		for (std::size_t groupSize : { 4, 2, 2, 2, 6 })
		{
			if (!first && *ptr++ != '-')
				return {};

			first = false;

			for (std::size_t i = 0; i < groupSize; ++i)
			{
				if (!ParseHexadecimalPair(ptr, *uuidPart++))
					return {};
			}
		}

		return Uuid{ uuid };
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
#elif defined(NAZARA_PLATFORM_LINUX) || defined(NAZARA_PLATFORM_MACOSX)
		uuid_t id;
		uuid_generate(id);

		std::copy(std::begin(id), std::end(id), uuid.begin());
#else
#error Missing platform support
#endif

		return uuid;
	}

	std::ostream& operator<<(std::ostream& out, const Uuid& guid)
	{
		std::array<char, 37> uuidStr = guid.ToStringArray();

		return out << uuidStr.data();
	}
}
