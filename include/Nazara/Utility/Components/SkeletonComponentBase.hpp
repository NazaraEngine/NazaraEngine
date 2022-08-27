// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_COMPONENTS_SKELETONCOMPONENTBASE_HPP
#define NAZARA_UTILITY_COMPONENTS_SKELETONCOMPONENTBASE_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Utility/Joint.hpp>
#include <Nazara/Utility/Skeleton.hpp>

namespace Nz
{
	class NAZARA_UTILITY_API SkeletonComponentBase
	{
		public:
			SkeletonComponentBase(const SkeletonComponentBase&) = default;
			SkeletonComponentBase(SkeletonComponentBase&&) noexcept = default;
			~SkeletonComponentBase() = default;

			virtual const Joint& GetAttachedJoint(std::size_t jointIndex) const = 0;
			inline const std::shared_ptr<Skeleton>& GetSkeleton() const;

			SkeletonComponentBase& operator=(const SkeletonComponentBase&) = default;
			SkeletonComponentBase& operator=(SkeletonComponentBase&&) noexcept = default;

		protected:
			SkeletonComponentBase(std::shared_ptr<Skeleton> skeleton);

			std::shared_ptr<Skeleton> m_referenceSkeleton;
	};
}

#include <Nazara/Utility/Components/SkeletonComponentBase.inl>

#endif // NAZARA_UTILITY_COMPONENTS_SKELETONCOMPONENTBASE_HPP
