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

		EntityHandle entity = m_entities[id].CreateHandle();

		// On initialise l'entité et on l'ajoute à la liste des entités vivantes
		entity->Create();
		m_aliveEntities.push_back(entity);

		return entity;
	}

	void World::Clear()
	{
		///DOC: Tous les handles sont correctement invalidés

		// Destruction des entités d'abord, et des handles ensuite
		// ceci pour éviter que les handles n'informent les entités inutilement lors de leur destruction
		m_entities.clear();

		m_aliveEntities.clear();
		m_killedEntities.clear();
	}

	void World::KillEntity(const EntityHandle& entity)
	{
		///DOC: Ignoré si l'entité est invalide

		if (IsEntityValid(entity))
			m_killedEntities.emplace_back(entity);
	}

	EntityHandle World::GetEntity(Entity::Id id)
	{
		if (IsEntityIdValid(id))
			return m_entities[id].CreateHandle();
		else
		{
			NazaraError("Invalid ID");
			return EntityHandle();
		}
	}

	void World::Update()
	{
		if (!m_killedEntities.empty())
		{
			for (unsigned int i = 0; i < m_killedEntities.size(); ++i)
			{
				const EntityHandle& entity = m_killedEntities[i];

				for (unsigned int j = 0; j < m_aliveEntities.size(); ++j)
				{
					if (entity == m_aliveEntities[j])
					{
						// Remise en file d'attente de l'identifiant d'entité
						m_freeIdList.push_back(entity->GetId());

						// Destruction de l'entité (invalidation du handle par la même occasion)
						entity->Destroy();

						// Suppression de l'entité des deux tableaux
						m_aliveEntities.erase(m_aliveEntities.begin() + j);
						m_killedEntities.erase(m_killedEntities.begin() + i);

						// Correction des indices (pour ne pas sauter une case)
						i--;
						j--;
						break;
					}
				}
			}
		}
	}
}
