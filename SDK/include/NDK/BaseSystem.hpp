// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_BASESYSTEM_HPP
#define NDK_BASESYSTEM_HPP

#include <Nazara/Core/Bitset.hpp>
#include <NDK/EntityHandle.hpp>
#include <vector>
#include <unordered_set>

namespace Ndk
{
	class World;

	class NDK_API BaseSystem
	{
		friend class Entity;
		friend World;

		public:
			BaseSystem(SystemId systemId);
			virtual ~BaseSystem();

			virtual BaseSystem* Clone() const = 0;

			bool Filters(const EntityHandle& entity) const;

			const std::vector<EntityHandle>& GetEntities() const;
			SystemId GetId() const;
			World& GetWorld() const;

			bool HasEntity(const EntityHandle& entity) const;

		protected:
			template<typename ComponentType> void Excludes();
			template<typename ComponentType1, typename ComponentType2, typename... Rest> void Excludes();
			void ExcludesComponent(ComponentId componentId);

			template<typename ComponentType> void Requires();
			template<typename ComponentType1, typename ComponentType2, typename... Rest> void Requires();
			void RequiresComponent(ComponentId componentId);

		private:
			void AddEntity(const EntityHandle& entity);

			virtual void OnEntityAdded(const EntityHandle& entity);
			virtual void OnEntityRemoved(const EntityHandle& entity);

			void RemoveEntity(const EntityHandle& entity);

			void SetWorld(World& world);

			std::vector<EntityHandle> m_entities;
			NzBitset<nzUInt64> m_entityBits;
			std::unordered_set<ComponentId> m_excludedComponents;
			std::unordered_set<ComponentId> m_requiredComponents;
			SystemId m_systemId;
			World* m_world;
	};
}

#include <NDK/BaseSystem.inl>

#endif // NDK_BASESYSTEM_HPP
