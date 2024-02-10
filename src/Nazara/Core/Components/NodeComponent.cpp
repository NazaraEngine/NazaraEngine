// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Components/NodeComponent.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Components/SharedSkeletonComponent.hpp>
#include <Nazara/Core/Components/SkeletonComponent.hpp>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	void NodeComponent::SetParent(entt::handle entity, bool keepDerived)
	{
		NodeComponent* nodeComponent = entity.try_get<NodeComponent>();
		NazaraAssert(nodeComponent, "entity doesn't have a NodeComponent");

		Node::SetParent(nodeComponent, keepDerived);
	}

	void NodeComponent::SetParentJoint(entt::handle entity, std::string_view jointName, bool keepDerived)
	{
		SkeletonComponentBase* skeletonComponent = entity.try_get<SkeletonComponent>();
		if (!skeletonComponent)
			skeletonComponent = entity.try_get<SharedSkeletonComponent>();

		NazaraAssert(skeletonComponent, "entity doesn't have a SkeletonComponent nor a SharedSkeletonComponent");

		std::size_t jointIndex = skeletonComponent->FindJointByName(jointName);
		if (jointIndex == Skeleton::InvalidJointIndex)
		{
			NazaraErrorFmt("skeleton has no joint \"{0}\"", jointName);
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
