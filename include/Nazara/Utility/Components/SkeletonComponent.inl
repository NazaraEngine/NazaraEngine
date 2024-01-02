// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline SkeletonComponent::SkeletonComponent(std::shared_ptr<Skeleton> skeleton) :
	SkeletonComponentBase(std::move(skeleton))
	{
	}

	inline Node* SkeletonComponent::GetRootNode()
	{
		return m_referenceSkeleton->GetRootJoint();
	}

	inline const Skeleton& SkeletonComponent::GetAttachedSkeleton() const
	{
		return *m_referenceSkeleton;
	}
}

#include <Nazara/Utility/DebugOff.hpp>
