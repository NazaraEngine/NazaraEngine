// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Systems/SkeletonSystem.hpp>
#include <Nazara/Core/Components/NodeComponent.hpp>
#include <Nazara/Core/Components/SharedSkeletonComponent.hpp>
#include <Nazara/Core/Components/SkeletonComponent.hpp>

namespace Nz
{
	SkeletonSystem::SkeletonSystem(flecs::world& world) :
	m_world(world)
	{
		m_sharedSkeletonObserver = m_world.observer<NodeComponent, SharedSkeletonComponent>()
		.event(flecs::OnAdd)
		.yield_existing()
		.each([](NodeComponent& entityNode, SharedSkeletonComponent& entitySkeleton)
		{
			entitySkeleton.SetSkeletonParent(&entityNode);
		});

		m_skeletonObserver = m_world.observer<NodeComponent, SkeletonComponent>()
		.event(flecs::OnAdd)
		.yield_existing()
		.each([](NodeComponent& entityNode, SkeletonComponent& entitySkeleton)
		{
			// TODO: When attaching for the first time, set the skeleton to the position of the node before attaching the node
			entityNode.SetParent(entitySkeleton.GetRootNode());
		});
	}

	SkeletonSystem::~SkeletonSystem()
	{
		m_sharedSkeletonObserver.destruct();
		m_skeletonObserver.destruct();
	}

	void SkeletonSystem::Update(Time /*elapsedTime*/)
	{
		// Updated attached skeleton joints (TODO: Only do this if necessary)
		m_world.each([](NodeComponent& /*entityNode*/, SharedSkeletonComponent& sharedSkeletonComponent)
		{
			if (sharedSkeletonComponent.IsAttachedSkeletonOutdated())
				sharedSkeletonComponent.UpdateAttachedSkeletonJoints();
		});
	}
}
