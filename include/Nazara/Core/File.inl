// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Computes the hash for the file
	* \return ByteArray represing the result of the hash of the file
	*
	* \param hash Hash to execute
	* \param filePath Path for the file
	*/

	inline ByteArray File::ComputeHash(HashType hash, const String& filePath)
	{
		return ComputeHash(AbstractHash::Get(hash).get(), filePath);
	}

	/*!
	* \brief Computes the hash for the file
	* \return ByteArray represing the result of the hash of the file
	*
	* \param hash Hash to execute
	* \param filePath Path for the file
	*/

	inline ByteArray File::ComputeHash(AbstractHash* hash, const String& filePath)
	{
		return Nz::ComputeHash(hash, File(filePath));
	}
}

#include <Nazara/Core/DebugOff.hpp>
