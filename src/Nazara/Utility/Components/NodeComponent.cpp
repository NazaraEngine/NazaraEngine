// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Components/NodeComponent.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Components/SharedSkeletonComponent.hpp>
#include <Nazara/Utility/Components/SkeletonComponent.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	void NodeComponent::SetParent(entt::handle entity, bool keepDerived)
	{
		NodeComponent* nodeComponent = entity.try_get<NodeComponent>();
		NazaraAssert(nodeComponent, "entity doesn't have a NodeComponent");

		Node::SetParent(nodeComponent, keepDerived);
	}

	void NodeComponent::SetParentJoint(entt::handle entity, const std::string& jointName, bool keepDerived)
	{
		SkeletonComponentBase* skeletonComponent = entity.try_get<SkeletonComponent>();
		if (!skeletonComponent)
			skeletonComponent = entity.try_get<SharedSkeletonComponent>();

		NazaraAssert(skeletonComponent, "entity doesn't have a SkeletonComponent nor a SharedSkeletonComponent");

		std::size_t jointIndex = skeletonComponent->FindJointByName(jointName);
		if (jointIndex == Skeleton::InvalidJointIndex)
		{
			NazaraError("skeleton has no joint \"{0}\"", jointName);
			return;
		}

		Node::SetParent(skeletonComponent->GetAttachedJoint(jointIndex), keepDerived);
	}

	void NodeComponent::SetParentJoint(entt::handle entity, std::size_t jointIndex, bool keepDerived)
	{
		SkeletonComponentBase* skeletonComponent = entity.try_get<SkeletonComponent>();
		if (!skeletonComponent)
			skeletonComponent = entity.try_get<SharedSkeletonComponent>();

		NazaraAssert(skeletonComponent, "entity doesn't have a SkeletonComponent nor a SharedSkeletonComponent");
		Node::SetParent(skeletonComponent->GetAttachedJoint(jointIndex), keepDerived);
	}
}
