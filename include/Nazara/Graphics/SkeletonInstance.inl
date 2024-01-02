// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline std::shared_ptr<RenderBuffer>& SkeletonInstance::GetSkeletalBuffer()
	{
		return m_skeletalDataBuffer;
	}

	inline const std::shared_ptr<RenderBuffer>& SkeletonInstance::GetSkeletalBuffer() const
	{
		return m_skeletalDataBuffer;
	}

	inline const std::shared_ptr<const Skeleton>& SkeletonInstance::GetSkeleton() const
	{
		return m_skeleton;
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
