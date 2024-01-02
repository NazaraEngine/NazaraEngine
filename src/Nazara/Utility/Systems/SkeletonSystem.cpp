// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Systems/SkeletonSystem.hpp>
#include <Nazara/Core/Components/DisabledComponent.hpp>
#include <Nazara/Utility/Components/NodeComponent.hpp>
#include <Nazara/Utility/Components/SharedSkeletonComponent.hpp>
#include <Nazara/Utility/Components/SkeletonComponent.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	SkeletonSystem::SkeletonSystem(entt::registry& registry) :
	m_registry(registry),
	m_sharedSkeletonConstructObserver(registry, entt::collector.group<NodeComponent, SharedSkeletonComponent>(entt::exclude<SkeletonComponent>)),
	m_skeletonConstructObserver(registry, entt::collector.group<NodeComponent, SkeletonComponent>(entt::exclude<SharedSkeletonComponent>))
	{
	}

	SkeletonSystem::~SkeletonSystem()
	{
		m_sharedSkeletonConstructObserver.disconnect();
		m_skeletonConstructObserver.disconnect();
	}

	void SkeletonSystem::Update(Time /*elapsedTime*/)
	{
		m_sharedSkeletonConstructObserver.each([&](entt::entity entity)
		{
			NodeComponent& entityNode = m_registry.get<NodeComponent>(entity);
			SharedSkeletonComponent& entitySkeleton = m_registry.get<SharedSkeletonComponent>(entity);

			entitySkeleton.SetSkeletonParent(&entityNode);
		});
		
		m_skeletonConstructObserver.each([&](entt::entity entity)
		{
			NodeComponent& entityNode = m_registry.get<NodeComponent>(entity);
			SkeletonComponent& entitySkeleton = m_registry.get<SkeletonComponent>(entity);

			// TODO: When attaching for the first time, set the skeleton to the position of the node before attaching the node
			entityNode.SetParent(entitySkeleton.GetRootNode());
		});

		// Updated attached skeleton joints (TODO: Only do this if necessary)
		auto view = m_registry.view<NodeComponent, SharedSkeletonComponent>(entt::exclude<DisabledComponent>);
		for (auto entity : view)
		{
			auto& sharedSkeletonComponent = view.get<SharedSkeletonComponent>(entity);
			if (sharedSkeletonComponent.IsAttachedSkeletonOutdated())
				sharedSkeletonComponent.UpdateAttachedSkeletonJoints();
		}
	}
}
