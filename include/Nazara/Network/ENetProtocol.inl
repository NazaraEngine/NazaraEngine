// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Network module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline UInt32 ENetTimeDifference(UInt32 a, UInt32 b)
	{
		return (ENetTimeLess(a, b)) ? b - a : a - b;
	}

	inline bool ENetTimeLess(UInt32 a, UInt32 b)
	{
		return (a - b >= ENetTimeOverflow);
	}

	inline bool ENetTimeLessEqual(UInt32 a, UInt32 b)
	{
		return !ENetTimeGreater(a, b);
	}

	inline bool ENetTimeGreater(UInt32 a, UInt32 b)
	{
		return ENetTimeLess(b, a);
	}

	inline bool ENetTimeGreaterEqual(UInt32 a, UInt32 b)
	{
		return !ENetTimeLess(a, b);
	}
}
