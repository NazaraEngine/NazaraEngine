// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_COMPONENTS_SKELETONCOMPONENTBASE_HPP
#define NAZARA_UTILITY_COMPONENTS_SKELETONCOMPONENTBASE_HPP

#include <NazaraUtils/Prerequisites.hpp>
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

			inline std::size_t FindJointByName(std::string_view jointName) const;

			inline const Joint& GetAttachedJoint(std::size_t jointIndex) const;
			inline const std::shared_ptr<Skeleton>& GetSkeleton() const;

			SkeletonComponentBase& operator=(const SkeletonComponentBase&) = default;
			SkeletonComponentBase& operator=(SkeletonComponentBase&&) noexcept = default;

		protected:
			SkeletonComponentBase(std::shared_ptr<Skeleton> skeleton);

			virtual const Skeleton& GetAttachedSkeleton() const = 0;

			std::shared_ptr<Skeleton> m_referenceSkeleton;
	};
}

#include <Nazara/Utility/Components/SkeletonComponentBase.inl>

#endif // NAZARA_UTILITY_COMPONENTS_SKELETONCOMPONENTBASE_HPP
