// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Components/SharedSkeletonComponent.hpp>
#include <Nazara/Core/Joint.hpp>
#include <Nazara/Core/Debug.hpp>

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

	const Skeleton& SharedSkeletonComponent::GetAttachedSkeleton() const
	{
		return m_attachedSkeleton;
	}

	void SharedSkeletonComponent::OnReferenceJointsInvalidated(const Skeleton* /*skeleton*/)
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
		std::size_t jointCount = m_referenceSkeleton->GetJointCount();
		assert(jointCount == m_attachedSkeleton.GetJointCount());
		if (jointCount == 0)
			return;

		const Joint* referenceJoints = m_referenceSkeleton->GetJoints();
		Joint* attachedJoints = m_attachedSkeleton.GetJoints();

		for (std::size_t i = 0; i < jointCount; ++i)
			attachedJoints[i].SetTransform(referenceJoints[i].GetPosition(), referenceJoints[i].GetRotation(), referenceJoints[i].GetScale(), CoordSys::Local, Node::Invalidation::DontInvalidate);

		m_attachedSkeleton.GetRootJoint()->Invalidate();

		m_skeletonJointInvalidated = false;
	}
}
