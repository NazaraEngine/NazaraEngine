// This file was automatically generated on 26 May 2014 at 01:05:31

#include <NDK/World.hpp>
#include <Nazara/Core/Error.hpp>

namespace Ndk
{
	Entity World::CreateEntity()
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
			// On alloue un nouvel identifiant
			m_entitiesCounter.resize(m_entitiesCounter.size() + 1);

			auto& counter = m_entitiesCounter.back();
			counter = 1;

			id.part.counter = counter;
			id.part.index = m_nextIndex;

			m_nextIndex++;
		}

		Entity entity(id, this);
		m_aliveEntities.push_back(entity);

		return entity;
	}

	void World::Clear()
	{
		///DOC: Les handles existants avant Clear ne sont plus garantis de ne pas être réutilisés
		///     et devraient être détruits avant la création d'une nouvelle entité.

		m_aliveEntities.clear();
		m_entitiesCounter.clear();
		m_freeIdList.clear();
		m_killedEntities.clear();

		m_nextIndex = 0;
	}

	void World::KillEntity(Entity& entity)
	{
		///DOC: Ignoré si l'entité est invalide

		if (IsEntityValid(entity))
			m_killedEntities.push_back(entity);
	}

	Entity World::GetEntity(Entity::Id id)
	{
		if (IsEntityIdValid(id))
			return Entity(id, this);
		else
		{
			NazaraError("Invalid ID");
			return Entity();
		}
	}

	void World::Update()
	{
		if (!m_killedEntities.empty())
		{
			for (unsigned int i = 0; i < m_killedEntities.size(); ++i)
			{
				Entity::Id e1 = m_aliveEntities[i].GetId();

				for (unsigned int j = 0; j < m_aliveEntities.size(); ++j)
				{
					Entity::Id e2 = m_killedEntities[j].GetId();
					if (e1 == e2)
					{
						// Remise en file de l'identifiant d'entité
						nzUInt32& counter = m_entitiesCounter[e1.part.index];
						counter++;

						e1.part.counter = counter;
						m_freeIdList.push_back(e1);

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
