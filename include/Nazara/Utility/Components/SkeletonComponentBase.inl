// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline SkeletonComponentBase::SkeletonComponentBase(std::shared_ptr<Skeleton> skeleton) :
	m_referenceSkeleton(std::move(skeleton))
	{
	}

	inline std::size_t SkeletonComponentBase::FindJointByName(const std::string& jointName) const
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

#include <Nazara/Utility/DebugOff.hpp>
