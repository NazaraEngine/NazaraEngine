// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline SkeletonComponentBase::SkeletonComponentBase(std::shared_ptr<Skeleton> skeleton) :
	m_referenceSkeleton(std::move(skeleton))
	{
	}

	inline std::size_t SkeletonComponentBase::FindJointByName(std::string_view jointName) const
	{
		return m_referenceSkeleton->GetJointIndex(jointName);
	}

	inline const Joint& SkeletonComponentBase::GetAttachedJoint(std::size_t jointIndex) const
	{
		return *GetAttachedSkeleton().GetJoint(jointIndex);
	}

	inline const std::shared_ptr<Skeleton>& SkeletonComponentBase::GetSkeleton() const
	{
		return m_referenceSkeleton;
	}
}
