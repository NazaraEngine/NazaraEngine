// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Components/SkeletonComponentBase.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	inline SkeletonComponentBase::SkeletonComponentBase(std::shared_ptr<Skeleton> skeleton) :
	m_referenceSkeleton(std::move(skeleton))
	{
	}

	inline const std::shared_ptr<Skeleton>& SkeletonComponentBase::GetSkeleton() const
	{
		return m_referenceSkeleton;
	}
}

#include <Nazara/Utility/DebugOff.hpp>
