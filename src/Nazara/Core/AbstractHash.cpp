// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/AbstractHash.hpp>
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
#include <NazaraUtils/Algorithm.hpp>

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
		NazaraAssertMsg(type <= HashType::Max, "hash type value out of enum");

		switch (type)
		{
			case HashType::Fletcher16:
				return std::make_unique<Fletcher16Hasher>();

			case HashType::CRC32:
				return std::make_unique<CRC32Hasher>();

			case HashType::CRC64:
				return std::make_unique<CRC64Hasher>();

			case HashType::MD5:
				return std::make_unique<MD5Hasher>();

			case HashType::SHA1:
				return std::make_unique<SHA1Hasher>();

			case HashType::SHA224:
				return std::make_unique<SHA224Hasher>();

			case HashType::SHA256:
				return std::make_unique<SHA256Hasher>();

			case HashType::SHA384:
				return std::make_unique<SHA384Hasher>();

			case HashType::SHA512:
				return std::make_unique<SHA512Hasher>();

			case HashType::Whirlpool:
				return std::make_unique<WhirlpoolHasher>();
		}

		NazaraInternalError("Hash type not handled ({0:#x})", UnderlyingCast(type));
		return nullptr;
	}
}
