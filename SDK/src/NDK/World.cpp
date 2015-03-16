// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/World.hpp>
#include <Nazara/Core/Error.hpp>

namespace Ndk
{
	World::~World()
	{
		// La destruction doit se faire dans un ordre précis
		Clear();
	}

	EntityHandle World::CreateEntity()
	{
		Entity::Id id;
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
			m_entities.push_back(Entity(*this, id));
		}

		// On initialise l'entité et on l'ajoute à la liste des entités vivantes
		Entity& entity = m_entities[id].entity;
		entity.Create();

		EntityHandle handle = entity.CreateHandle();
		m_aliveEntities.push_back(handle);
		m_entities[id].aliveIndex = m_aliveEntities.size()-1;

		return handle;
	}

	void World::Clear()
	{
		///DOC: Tous les handles sont correctement invalidés

		// Destruction des entités d'abord, et des handles ensuite
		// ceci pour éviter que les handles n'informent les entités inutilement lors de leur destruction
		m_entities.clear();

		m_aliveEntities.clear();
		m_killedEntities.Clear();
	}

	void World::KillEntity(const EntityHandle& entity)
	{
		///DOC: Ignoré si l'entité est invalide

		if (IsEntityValid(entity))
			m_killedEntities.UnboundedSet(entity->GetId(), true);
	}

	const EntityHandle& World::GetEntity(Entity::Id id)
	{
		if (IsEntityIdValid(id))
			return m_aliveEntities[m_entities[id].aliveIndex];
		else
		{
			NazaraError("Invalid ID");
			return EntityHandle();
		}
	}

	void World::Update()
	{
		for (unsigned int i = m_killedEntities.FindFirst(); i != m_killedEntities.npos; i = m_killedEntities.FindNext(i))
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
	}
}
