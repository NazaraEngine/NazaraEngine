// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Components/SharedSkeletonComponent.hpp>
#include <Nazara/Utility/Joint.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	SharedSkeletonComponent::SharedSkeletonComponent(std::shared_ptr<Skeleton> skeleton) :
	SkeletonComponentBase(std::move(skeleton)),
	m_skeletonJointInvalidated(true)
	{
		SetupSkeleton();
	}

	SharedSkeletonComponent::SharedSkeletonComponent(const SharedSkeletonComponent& sharedSkeletalComponent) :
	SkeletonComponentBase(sharedSkeletalComponent),
	m_attachedSkeleton(sharedSkeletalComponent.m_attachedSkeleton),
	m_skeletonJointInvalidated(true)
	{
		SetupSkeleton();
	}

	SharedSkeletonComponent::SharedSkeletonComponent(SharedSkeletonComponent&& sharedSkeletalComponent) noexcept :
	SkeletonComponentBase(std::move(sharedSkeletalComponent)),
	m_attachedSkeleton(std::move(sharedSkeletalComponent.m_attachedSkeleton)),
	m_skeletonJointInvalidated(sharedSkeletalComponent.m_skeletonJointInvalidated)
	{
		SetupSkeleton();
	}

	const Joint& SharedSkeletonComponent::GetAttachedJoint(std::size_t jointIndex) const
	{
		return *m_attachedSkeleton.GetJoint(jointIndex);
	}

	SharedSkeletonComponent& SharedSkeletonComponent::operator=(const SharedSkeletonComponent& sharedSkeletalComponent)
	{
		SkeletonComponentBase::operator=(sharedSkeletalComponent);

		m_attachedSkeleton = sharedSkeletalComponent.m_attachedSkeleton;
		m_skeletonJointInvalidated = true;
		SetupSkeleton();

		return *this;
	}

	SharedSkeletonComponent& SharedSkeletonComponent::operator=(SharedSkeletonComponent&& sharedSkeletalComponent) noexcept
	{
		SkeletonComponentBase::operator=(std::move(sharedSkeletalComponent));

		m_attachedSkeleton = std::move(sharedSkeletalComponent.m_attachedSkeleton);
		m_skeletonJointInvalidated = sharedSkeletalComponent.m_skeletonJointInvalidated;
		SetupSkeleton();

		return *this;
	}

	void SharedSkeletonComponent::OnReferenceJointsInvalidated(const Skeleton* skeleton)
	{
		m_skeletonJointInvalidated = true;
	}

	void SharedSkeletonComponent::SetSkeletonParent(Node* parent)
	{
		m_attachedSkeleton.GetRootJoint()->SetParent(parent);
	}

	void SharedSkeletonComponent::SetupSkeleton()
	{
		assert(m_referenceSkeleton);
		m_attachedSkeleton = *m_referenceSkeleton;
		m_referenceSkeleton->OnSkeletonJointsInvalidated.Connect(this, &SharedSkeletonComponent::OnReferenceJointsInvalidated);
	}

	void SharedSkeletonComponent::UpdateAttachedSkeletonJoints()
	{
		assert(m_referenceSkeleton->GetJointCount() == m_attachedSkeleton.GetJointCount());
		std::size_t jointCount = m_referenceSkeleton->GetJointCount();

		// TODO: This will trigger a lot of invalidation which can be avoided
		for (std::size_t i = 0; i < jointCount; ++i)
		{
			const Joint* referenceJoint = m_referenceSkeleton->GetJoint(i);
			Joint* attachedJoint = m_attachedSkeleton.GetJoint(i);

			attachedJoint->SetPosition(referenceJoint->GetPosition());
			attachedJoint->SetRotation(referenceJoint->GetRotation());
			attachedJoint->SetScale(referenceJoint->GetScale());
		}

		m_skeletonJointInvalidated = false;
	}
}
