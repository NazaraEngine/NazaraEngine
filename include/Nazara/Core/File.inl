// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/AbstractHash.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline ByteArray File::ComputeHash(HashType hash, const String& filePath)
	{
		return ComputeHash(AbstractHash::Get(hash).get(), filePath);
	}

	inline ByteArray File::ComputeHash(AbstractHash* hash, const String& filePath)
	{
		return Nz::ComputeHash(hash, File(filePath));
	}
}

#include <Nazara/Core/DebugOff.hpp>
