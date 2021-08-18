// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Hash/CRC32.hpp>
#include <Nazara/Core/Hash/CRC64.hpp>
#include <Nazara/Core/Hash/Fletcher16.hpp>
#include <Nazara/Core/Hash/MD5.hpp>
#include <Nazara/Core/Hash/SHA1.hpp>
#include <Nazara/Core/Hash/SHA224.hpp>
#include <Nazara/Core/Hash/SHA256.hpp>
#include <Nazara/Core/Hash/SHA384.hpp>
#include <Nazara/Core/Hash/SHA512.hpp>
#include <Nazara/Core/Hash/Whirlpool.hpp>
#include <Nazara/Math/Algorithm.hpp>
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
		NazaraAssert(type <= HashType::Max, "Hash type value out of enum");

		switch (type)
		{
			case HashType::Fletcher16:
				return std::make_unique<HashFletcher16>();

			case HashType::CRC32:
				return std::make_unique<HashCRC32>();

			case HashType::CRC64:
				return std::make_unique<HashCRC64>();

			case HashType::MD5:
				return std::make_unique<HashMD5>();

			case HashType::SHA1:
				return std::make_unique<HashSHA1>();

			case HashType::SHA224:
				return std::make_unique<HashSHA224>();

			case HashType::SHA256:
				return std::make_unique<HashSHA256>();

			case HashType::SHA384:
				return std::make_unique<HashSHA384>();

			case HashType::SHA512:
				return std::make_unique<HashSHA512>();

			case HashType::Whirlpool:
				return std::make_unique<HashWhirlpool>();
		}

		NazaraInternalError("Hash type not handled (0x" + NumberToString(UnderlyingCast(type), 16) + ')');
		return nullptr;
	}
}
