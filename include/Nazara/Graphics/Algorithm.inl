// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <NazaraUtils/MathUtils.hpp>
#include <cstring>
#include <limits>

namespace Nz
{
	constexpr UInt32 DistanceAsSortKey(float distance)
	{
#if defined(arm) && \
    ((defined(__MAVERICK__) && defined(NAZARA_BIG_ENDIAN)) || \
    (!defined(__SOFTFP__) && !defined(__VFP_FP__) && !defined(__MAVERICK__)))
#error The following code relies on native-endian IEEE-754 representation, which your platform does not guarantee
#endif

		static_assert(sizeof(float) == sizeof(UInt32));
		static_assert(std::numeric_limits<float>::is_iec559);

		if (IsNaN(distance))
			return MaxValue<UInt32>();

		if (IsInfinity(distance))
			return MinValue<UInt32>();

		return ~BitCast<UInt32>(distance); //< Reverse distance to have back to front
	}
}
