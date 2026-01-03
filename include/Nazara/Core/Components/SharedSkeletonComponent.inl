// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline bool SharedSkeletonComponent::IsAttachedSkeletonOutdated() const
	{
		return m_skeletonJointInvalidated;
	}
}
