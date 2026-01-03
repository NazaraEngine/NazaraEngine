// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Systems/SkeletonSystem.hpp>
#include <Nazara/Core/Components/DisabledComponent.hpp>
#include <Nazara/Core/Components/NodeComponent.hpp>
#include <Nazara/Core/Components/SharedSkeletonComponent.hpp>
#include <Nazara/Core/Components/SkeletonComponent.hpp>

namespace Nz
{
	SkeletonSystem::SkeletonSystem(entt::registry& registry) :
	m_registry(registry),
	m_sharedSkeletonConstructObserver(m_registry),
	m_skeletonConstructObserver(m_registry)
	{
		m_sharedSkeletonConstructObserver.OnEntityAdded.Connect([&](entt::entity entity)
		{
			NodeComponent& entityNode = m_registry.get<NodeComponent>(entity);
			SharedSkeletonComponent& entitySkeleton = m_registry.get<SharedSkeletonComponent>(entity);

			entitySkeleton.SetSkeletonParent(&entityNode);
		});
		m_sharedSkeletonConstructObserver.SignalExisting();

		m_skeletonConstructObserver.OnEntityAdded.Connect([&](entt::entity entity)
		{
			NodeComponent& entityNode = m_registry.get<NodeComponent>(entity);
			SkeletonComponent& entitySkeleton = m_registry.get<SkeletonComponent>(entity);

			// TODO: When attaching for the first time, set the skeleton to the position of the node before attaching the node
			entityNode.SetParent(entitySkeleton.GetRootNode());
		});
		m_skeletonConstructObserver.SignalExisting();
	}

	void SkeletonSystem::Update(Time /*elapsedTime*/)
	{
		// Updated attached skeleton joints (TODO: Only do this if necessary)
		for (entt::entity entity : m_sharedSkeletonConstructObserver)
		{
			auto& sharedSkeletonComponent = m_registry.get<SharedSkeletonComponent>(entity);
			if (sharedSkeletonComponent.IsAttachedSkeletonOutdated())
				sharedSkeletonComponent.UpdateAttachedSkeletonJoints();
		}
	}
}
