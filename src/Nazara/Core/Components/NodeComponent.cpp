// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Components/NodeComponent.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Components/SharedSkeletonComponent.hpp>
#include <Nazara/Core/Components/SkeletonComponent.hpp>

namespace Nz
{
	void NodeComponent::SetParent(entt::handle entity, bool keepDerived)
	{
		NodeComponent* nodeComponent = entity.try_get<NodeComponent>();
		NazaraAssertMsg(nodeComponent, "entity doesn't have a NodeComponent");

		Node::SetParent(nodeComponent, keepDerived);
	}

	void NodeComponent::SetParentJoint(entt::handle entity, std::string_view jointName, bool keepDerived)
	{
		SkeletonComponentBase* skeletonComponent = entity.try_get<SkeletonComponent>();
		if (!skeletonComponent)
			skeletonComponent = entity.try_get<SharedSkeletonComponent>();

		NazaraAssertMsg(skeletonComponent, "entity doesn't have a SkeletonComponent nor a SharedSkeletonComponent");

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

		NazaraAssertMsg(skeletonComponent, "entity doesn't have a SkeletonComponent nor a SharedSkeletonComponent");
		Node::SetParent(skeletonComponent->GetAttachedJoint(jointIndex), keepDerived);
	}
}
