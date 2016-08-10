// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/World.hpp>
#include <Nazara/Core/Error.hpp>
#include <NDK/Systems/PhysicsSystem.hpp>
#include <NDK/Systems/VelocitySystem.hpp>

#ifndef NDK_SERVER
#include <NDK/Systems/ListenerSystem.hpp>
#include <NDK/Systems/ParticleSystem.hpp>
#include <NDK/Systems/RenderSystem.hpp>
#endif

namespace Ndk
{
	World::~World() noexcept
	{
		// La destruction doit se faire dans un ordre précis
		Clear();
	}

	void World::AddDefaultSystems()
	{
		AddSystem<PhysicsSystem>();
		AddSystem<VelocitySystem>();

		#ifndef NDK_SERVER
		AddSystem<ListenerSystem>();
		AddSystem<ParticleSystem>();
		AddSystem<RenderSystem>();
		#endif
	}

	const EntityHandle& World::CreateEntity()
	{
		EntityId id;
		if (!m_freeIdList.empty())
		{
			// On récupère un identifiant
			id = m_freeIdList.back();
			m_freeIdList.pop_back();
		}
		else
		{
			// On alloue une nouvelle entité
			id = m_entities.size();

			// Impossible d'utiliser emplace_back à cause de la portée
			m_entities.push_back(Entity(this, id));
		}

		// On initialise l'entité et on l'ajoute à la liste des entités vivantes
		Entity& entity = m_entities[id].entity;
		entity.Create();

		m_aliveEntities.emplace_back(&entity);
		m_entities[id].aliveIndex = m_aliveEntities.size()-1;

		return m_aliveEntities.back();
	}

	void World::Clear() noexcept
	{
		///DOC: Tous les handles sont correctement invalidés, les entités sont immédiatement invalidées

		// Destruction des entités d'abord, et des handles ensuite
		// ceci pour éviter que les handles n'informent les entités inutilement lors de leur destruction
		m_entities.clear();

		m_aliveEntities.clear();
		m_dirtyEntities.Clear();
		m_killedEntities.Clear();
	}

	void World::KillEntity(Entity* entity)
	{
		///DOC: Ignoré si l'entité est invalide

		if (IsEntityValid(entity))
			m_killedEntities.UnboundedSet(entity->GetId(), true);
	}

	const EntityHandle& World::GetEntity(EntityId id)
	{
		if (IsEntityIdValid(id))
			return m_aliveEntities[m_entities[id].aliveIndex];
		else
		{
			NazaraError("Invalid ID");
			return EntityHandle::InvalidHandle;
		}
	}

	void World::Update()
	{
		// Gestion des entités tuées depuis le dernier appel
		for (std::size_t i = m_killedEntities.FindFirst(); i != m_killedEntities.npos; i = m_killedEntities.FindNext(i))
		{
			EntityBlock& block = m_entities[i];
			Entity& entity = block.entity;

			NazaraAssert(entity.IsValid(), "Entity must be valid");

			// Remise en file d'attente de l'identifiant d'entité
			m_freeIdList.push_back(entity.GetId());

			// Destruction de l'entité (invalidation du handle par la même occasion)
			entity.Destroy();

			// Nous allons sortir le handle de la liste des entités vivantes
			// en swappant le handle avec le dernier handle, avant de pop

			NazaraAssert(block.aliveIndex < m_aliveEntities.size(), "Alive index out of range");

			if (block.aliveIndex < m_aliveEntities.size()-1) // S'il ne s'agit pas du dernier handle
			{
				EntityHandle& lastHandle = m_aliveEntities.back();
				EntityHandle& myHandle = m_aliveEntities[block.aliveIndex];

				myHandle = std::move(lastHandle);

				// On n'oublie pas de corriger l'indice associé à l'entité
				m_entities[myHandle->GetId()].aliveIndex = block.aliveIndex;
			}
			m_aliveEntities.pop_back();
		}
		m_killedEntities.Reset();

		// Gestion des entités nécessitant une mise à jour de leurs systèmes
		for (std::size_t i = m_dirtyEntities.FindFirst(); i != m_dirtyEntities.npos; i = m_dirtyEntities.FindNext(i))
		{
			NazaraAssert(i < m_entities.size(), "Entity index out of range");

			Entity* entity = &m_entities[i].entity;

			// Check entity validity (as it could have been reported as dirty and killed during the same iteration)
			if (!entity->IsValid())
				continue;

			Nz::Bitset<>& removedComponents = entity->GetRemovedComponentBits();
			for (std::size_t j = removedComponents.FindFirst(); j != m_dirtyEntities.npos; j = removedComponents.FindNext(j))
				entity->DestroyComponent(j);
			removedComponents.Reset();

			for (auto& system : m_systems)
			{
				// Ignore non-existent systems
				if (!system)
					continue;

				// Is our entity already part of this system?
				bool partOfSystem = system->HasEntity(entity);

				// Should it be part of it?
				if (entity->IsEnabled() && system->Filters(entity))
				{
					// Yes it should, add it to the system if not already done and validate it (again)
					if (!partOfSystem)
						system->AddEntity(entity);

					system->ValidateEntity(entity, !partOfSystem);
				}
				else
				{
					// No, it shouldn't, remove it if it's part of the system
					if (partOfSystem)
						system->RemoveEntity(entity);
				}
			}
		}
		m_dirtyEntities.Reset();
	}
}
