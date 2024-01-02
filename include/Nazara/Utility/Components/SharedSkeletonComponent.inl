// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline bool SharedSkeletonComponent::IsAttachedSkeletonOutdated() const
	{
		return m_skeletonJointInvalidated;
	}
}

#include <Nazara/Utility/DebugOff.hpp>
