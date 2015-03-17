// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_ENTITY_HPP
#define NDK_ENTITY_HPP

#include <Nazara/Core/Bitset.hpp>
#include <NDK/Component.hpp>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace Ndk
{
	class EntityHandle;
	class World;

	class NDK_API Entity
	{
		friend class BaseSystem;
		friend EntityHandle;
		friend World;

		public:
			Entity(const Entity&) = delete;
			Entity(Entity&& entity);
			~Entity();

			BaseComponent& AddComponent(std::unique_ptr<BaseComponent>&& component);
			template<typename ComponentType, typename... Args> ComponentType& AddComponent(Args&&... args);

			EntityHandle CreateHandle();

			BaseComponent& GetComponent(ComponentId componentId);
			template<typename ComponentType> ComponentType& GetComponent();
			EntityId GetId() const;
			World* GetWorld() const;

			bool HasComponent(ComponentId componentId) const;
			template<typename ComponentType> bool HasComponent() const;

			void Kill();

			bool IsValid() const;

			void RemoveAllComponents();
			void RemoveComponent(ComponentId componentId);
			template<typename ComponentType> void RemoveComponent();

			Entity& operator=(const Entity&) = delete;
			Entity& operator=(Entity&&) = delete;

		private:
			Entity(World& world, EntityId id);

			void Create();
			void Destroy();

			void RegisterHandle(EntityHandle* handle);
			void RegisterSystem(SystemId systemId);
			void UnregisterHandle(EntityHandle* handle);
			void UnregisterSystem(SystemId systemId);

			std::vector<EntityHandle*> m_handles;
			std::unordered_map<ComponentId, std::unique_ptr<BaseComponent>> m_components;
			std::unordered_set<SystemId> m_systems;
			EntityId m_id;
			World* m_world;
			bool m_valid;
	};
}

#include <NDK/Entity.inl>

#endif // NDK_ENTITY_HPP
