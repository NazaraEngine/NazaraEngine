// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Components/SkeletonComponent.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	SkeletonComponent::SkeletonComponent(std::shared_ptr<Skeleton> skeleton) :
	SkeletonComponentBase(std::move(skeleton))
	{
	}

	Node* SkeletonComponent::GetRootNode()
	{
		return m_referenceSkeleton->GetRootJoint();
	}

	const Skeleton& SkeletonComponent::GetAttachedSkeleton() const
	{
		return *m_referenceSkeleton;
	}
}
