// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline bool HashAppend(AbstractHash* hash, const MemoryStream& stream)
	{
		return HashAppend(hash, stream.GetBuffer());
	}
}

#include <Nazara/Core/DebugOff.hpp>
