// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp


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
