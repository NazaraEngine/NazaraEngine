// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_ENTITY_HPP
#define NDK_ENTITY_HPP

#include <Nazara/Core/Bitset.hpp>
#include <NDK/Algorithm.hpp>
#include <memory>
#include <vector>

namespace Ndk
{
	class BaseComponent;
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

			inline BaseComponent& GetComponent(ComponentIndex index);
			template<typename ComponentType> ComponentType& GetComponent();
			inline const NzBitset<>& GetComponentBits() const;
			inline EntityId GetId() const;
			inline const NzBitset<>& GetSystemBits() const;
			inline World* GetWorld() const;

			inline bool HasComponent(ComponentIndex index) const;
			template<typename ComponentType> bool HasComponent() const;

			void Kill();

			void Invalidate();
			inline bool IsValid() const;

			void RemoveAllComponents();
			void RemoveComponent(ComponentIndex index);
			template<typename ComponentType> void RemoveComponent();

			Entity& operator=(const Entity&) = delete;
			Entity& operator=(Entity&&) = delete;

		private:
			Entity(World& world, EntityId id);

			void Create();
			void Destroy();

			inline void RegisterHandle(EntityHandle* handle);
			inline void RegisterSystem(SystemIndex index);
			inline void UnregisterHandle(EntityHandle* handle);
			inline void UnregisterSystem(SystemIndex index);

			std::vector<std::unique_ptr<BaseComponent>> m_components;
			std::vector<EntityHandle*> m_handles;
			EntityId m_id;
			NzBitset<> m_componentBits;
			NzBitset<> m_systemBits;
			World* m_world;
			bool m_valid;
	};
}

#include <NDK/Entity.inl>

#endif // NDK_ENTITY_HPP
