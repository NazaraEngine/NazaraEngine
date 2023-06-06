// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringExt.hpp>
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
				return std::make_unique<Fletcher16Hash>();

			case HashType::CRC32:
				return std::make_unique<CRC32Hash>();

			case HashType::CRC64:
				return std::make_unique<CRC64Hash>();

			case HashType::MD5:
				return std::make_unique<MD5Hash>();

			case HashType::SHA1:
				return std::make_unique<SHA1Hash>();

			case HashType::SHA224:
				return std::make_unique<SHA224Hash>();

			case HashType::SHA256:
				return std::make_unique<SHA256Hash>();

			case HashType::SHA384:
				return std::make_unique<SHA384Hash>();

			case HashType::SHA512:
				return std::make_unique<SHA512Hash>();

			case HashType::Whirlpool:
				return std::make_unique<WhirlpoolHash>();
		}

		NazaraInternalError("Hash type not handled (0x" + NumberToString(UnderlyingCast(type), 16) + ')');
		return nullptr;
	}
}
