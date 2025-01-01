// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_COMPONENTS_SHAREDSKELETONCOMPONENT_HPP
#define NAZARA_CORE_COMPONENTS_SHAREDSKELETONCOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Skeleton.hpp>
#include <Nazara/Core/Components/SkeletonComponentBase.hpp>

namespace Nz
{
	class NAZARA_CORE_API SharedSkeletonComponent final : public SkeletonComponentBase
	{
		friend class SkeletonSystem;

		public:
			SharedSkeletonComponent(std::shared_ptr<Skeleton> skeleton);
			SharedSkeletonComponent(const SharedSkeletonComponent& sharedSkeletalComponent);
			SharedSkeletonComponent(SharedSkeletonComponent&& sharedSkeletalComponent) noexcept;
			~SharedSkeletonComponent() = default;

			SharedSkeletonComponent& operator=(const SharedSkeletonComponent& sharedSkeletalComponent);
			SharedSkeletonComponent& operator=(SharedSkeletonComponent&& sharedSkeletalComponent) noexcept;

		private:
			const Skeleton& GetAttachedSkeleton() const override;
			inline bool IsAttachedSkeletonOutdated() const;
			void OnReferenceJointsInvalidated(const Skeleton* skeleton);
			void SetSkeletonParent(Node* parent);
			void SetupSkeleton();
			void UpdateAttachedSkeletonJoints();

			NazaraSlot(Skeleton, OnSkeletonJointsInvalidated, m_onSkeletonJointsInvalidated);

			Skeleton m_attachedSkeleton;
			bool m_skeletonJointInvalidated;
	};
}

#include <Nazara/Core/Components/SharedSkeletonComponent.inl>

#endif // NAZARA_CORE_COMPONENTS_SHAREDSKELETONCOMPONENT_HPP
