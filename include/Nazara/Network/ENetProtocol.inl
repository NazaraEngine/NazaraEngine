// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Network/ENetProtocol.hpp>
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
