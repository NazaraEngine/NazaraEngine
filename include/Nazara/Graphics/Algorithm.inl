// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <cmath>
#include <cstring>
#include <limits>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline UInt32 DistanceAsSortKey(float distance)
	{
#if defined(arm) && \
    ((defined(__MAVERICK__) && defined(NAZARA_BIG_ENDIAN)) || \
    (!defined(__SOFTFP__) && !defined(__VFP_FP__) && !defined(__MAVERICK__)))
#error The following code relies on native-endian IEEE-754 representation, which your platform does not guarantee
#endif

		static_assert(sizeof(float) == sizeof(UInt32));
		static_assert(std::numeric_limits<float>::is_iec559);

		if (std::isnan(distance))
			return std::numeric_limits<UInt32>::max();

		if (std::isinf(distance))
			return 0;

		UInt32 distanceInt;
		std::memcpy(&distanceInt, &distance, sizeof(UInt32));

		return ~distanceInt; //< Reverse distance to have back to front
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
