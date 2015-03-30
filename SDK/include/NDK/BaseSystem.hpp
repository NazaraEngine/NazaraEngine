// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_BASESYSTEM_HPP
#define NDK_BASESYSTEM_HPP

#include <Nazara/Core/Bitset.hpp>
#include <NDK/EntityHandle.hpp>
#include <vector>

namespace Ndk
{
	class World;

	class NDK_API BaseSystem
	{
		friend class Entity;
		friend World;

		public:
			BaseSystem(SystemIndex systemId);
			virtual ~BaseSystem();

			virtual BaseSystem* Clone() const = 0;

			bool Filters(const Entity* entity) const;

			const std::vector<EntityHandle>& GetEntities() const;
			SystemIndex GetIndex() const;
			World& GetWorld() const;

			bool HasEntity(const Entity* entity) const;

			static SystemIndex GetNextIndex();

		protected:
			template<typename ComponentType> void Excludes();
			template<typename ComponentType1, typename ComponentType2, typename... Rest> void Excludes();
			void ExcludesComponent(ComponentIndex index);

			template<typename ComponentType> void Requires();
			template<typename ComponentType1, typename ComponentType2, typename... Rest> void Requires();
			void RequiresComponent(ComponentIndex index);

		private:
			void AddEntity(Entity* entity);

			virtual void OnEntityAdded(Entity* entity);
			virtual void OnEntityRemoved(Entity* entity);

			void RemoveEntity(Entity* entity);

			void SetWorld(World& world);

			std::vector<EntityHandle> m_entities;
			NzBitset<nzUInt64> m_entityBits;
			NzBitset<> m_excludedComponents;
			mutable NzBitset<> m_filterResult;
			NzBitset<> m_requiredComponents;
			SystemIndex m_systemIndex;
			World* m_world;

			static SystemIndex s_nextIndex;
	};
}

#include <NDK/BaseSystem.inl>

#endif // NDK_BASESYSTEM_HPP
