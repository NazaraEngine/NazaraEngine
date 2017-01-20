// Copyright (C) 2017 Jérôme Leclercq
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
	/*!
	* \ingroup core
	* \class Nz::AbstractHash
	* \brief Core class that represents the behaviour of the hash classes
	*
	* \remark This class is abstract
	*/

	AbstractHash::~AbstractHash() = default;

	/*!
	* \brief Factory of Hash classes in function of HashType
	* \return A new instance of the Hash class according to the HashType
	*
	* \param type Enumeration of type HashType
	*
	* \remark If enumeration is not defined in HashType, a NazaraInternalError is thrown and nullptr is returned
	*/

	std::unique_ptr<AbstractHash> AbstractHash::Get(HashType type)
	{
		NazaraAssert(type <= HashType_Max, "Hash type value out of enum");

		switch (type)
		{
			case HashType_Fletcher16:
				return std::make_unique<HashFletcher16>();

			case HashType_CRC32:
				return std::make_unique<HashCRC32>();

			case HashType_MD5:
				return std::make_unique<HashMD5>();

			case HashType_SHA1:
				return std::make_unique<HashSHA1>();

			case HashType_SHA224:
				return std::make_unique<HashSHA224>();

			case HashType_SHA256:
				return std::make_unique<HashSHA256>();

			case HashType_SHA384:
				return std::make_unique<HashSHA384>();

			case HashType_SHA512:
				return std::make_unique<HashSHA512>();

			case HashType_Whirlpool:
				return std::make_unique<HashWhirlpool>();
		}

		NazaraInternalError("Hash type not handled (0x" + String::Number(type, 16) + ')');
		return nullptr;
	}
}
