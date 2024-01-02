// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/Debug.hpp>

namespace Nz
{
	UInt32 ENetTimeDifference(UInt32 a, UInt32 b)
	{
		return (ENetTimeLess(a, b)) ? b - a : a - b;
	}

	bool ENetTimeLess(UInt32 a, UInt32 b)
	{
		return (a - b >= ENetTimeOverflow);
	}

	bool ENetTimeLessEqual(UInt32 a, UInt32 b)
	{
		return !ENetTimeGreater(a, b);
	}

	bool ENetTimeGreater(UInt32 a, UInt32 b)
	{
		return ENetTimeLess(b, a);
	}

	bool ENetTimeGreaterEqual(UInt32 a, UInt32 b)
	{
		return !ENetTimeLess(a, b);
	}
}

#include <Nazara/Network/DebugOff.hpp>
