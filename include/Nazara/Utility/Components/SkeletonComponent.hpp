// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_COMPONENTS_SKELETONCOMPONENT_HPP
#define NAZARA_UTILITY_COMPONENTS_SKELETONCOMPONENT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <Nazara/Utility/Components/SkeletonComponentBase.hpp>

namespace Nz
{
	class Node;

	class NAZARA_UTILITY_API SkeletonComponent final : public SkeletonComponentBase
	{
		public:
			SkeletonComponent(std::shared_ptr<Skeleton> skeleton);
			SkeletonComponent(const SkeletonComponent&) = delete;
			SkeletonComponent(SkeletonComponent&& skeletalComponent) noexcept = default;
			~SkeletonComponent() = default;

			Node* GetRootNode();

			SkeletonComponent& operator=(const SkeletonComponent&) = delete;
			SkeletonComponent& operator=(SkeletonComponent&& skeletalComponent) noexcept = default;

		private:
			const Skeleton& GetAttachedSkeleton() const override;
	};
}

#include <Nazara/Utility/Components/SkeletonComponent.inl>

#endif // NAZARA_UTILITY_COMPONENTS_SKELETONCOMPONENT_HPP
