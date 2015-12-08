// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Hash/CRC32.hpp>
#include <Nazara/Core/Hash/Fletcher16.hpp>
#include <Nazara/Core/Hash/MD5.hpp>
#include <Nazara/Core/Hash/SHA1.hpp>
#include <Nazara/Core/Hash/SHA224.hpp>
#include <Nazara/Core/Hash/SHA256.hpp>
#include <Nazara/Core/Hash/SHA384.hpp>
#include <Nazara/Core/Hash/SHA512.hpp>
#include <Nazara/Core/Hash/Whirlpool.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	AbstractHash::~AbstractHash() = default;

	std::unique_ptr<AbstractHash> AbstractHash::Get(HashType type)
	{
		NazaraAssert(type <= HashType_Max, "Hash type value out of enum");

		switch (type)
		{
			case HashType_Fletcher16:
				return std::unique_ptr<AbstractHash>(new HashFletcher16);

			case HashType_CRC32:
				return std::unique_ptr<AbstractHash>(new HashCRC32);

			case HashType_MD5:
				return std::unique_ptr<AbstractHash>(new HashMD5);

			case HashType_SHA1:
				return std::unique_ptr<AbstractHash>(new HashSHA1);

			case HashType_SHA224:
				return std::unique_ptr<AbstractHash>(new HashSHA224);

			case HashType_SHA256:
				return std::unique_ptr<AbstractHash>(new HashSHA256);

			case HashType_SHA384:
				return std::unique_ptr<AbstractHash>(new HashSHA384);

			case HashType_SHA512:
				return std::unique_ptr<AbstractHash>(new HashSHA512);

			case HashType_Whirlpool:
				return std::unique_ptr<AbstractHash>(new HashWhirlpool);
		}

		NazaraInternalError("Hash type not handled (0x" + String::Number(type, 16) + ')');
		return std::unique_ptr<AbstractHash>();
	}
}
