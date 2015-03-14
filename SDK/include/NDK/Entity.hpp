// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_ENTITY_HPP
#define NDK_ENTITY_HPP

#include <NDK/Component.hpp>
#include <memory>
#include <vector>

namespace Ndk
{
	class EntityHandle;
	class World;

	class NDK_API Entity
	{
		friend EntityHandle;
		friend World;

		public:
			using Id = nzUInt32;

			Entity(const Entity&) = delete;
			Entity(Entity&& entity);
			~Entity();

			BaseComponent& AddComponent(std::unique_ptr<BaseComponent>&& component);
			template<typename ComponentType, typename... Args> ComponentType& AddComponent(Args&&... args);

			EntityHandle CreateHandle();

			BaseComponent& GetComponent(nzUInt32 componentId);
			template<typename ComponentType> ComponentType& GetComponent();
			Id GetId() const;
			World* GetWorld() const;

			bool HasComponent(nzUInt32 componentId) const;
			template<typename ComponentType> bool HasComponent() const;

			void Kill();

			bool IsValid() const;

			void RemoveAllComponents();
			void RemoveComponent(nzUInt32 componentId);
			template<typename ComponentType> void RemoveComponent();

			Entity& operator=(const Entity&) = delete;
			Entity& operator=(Entity&&) = delete;

		private:
			Entity(World& world, Id id);

			void Create();
			void Destroy();

			void RegisterHandle(EntityHandle* handle);
			void UnregisterHandle(EntityHandle* handle);

			std::vector<std::unique_ptr<BaseComponent>> m_components;
			std::vector<EntityHandle*> m_handles;
			Id m_id;
			World* m_world;
			bool m_valid;
	};
}

#include <NDK/Entity.inl>

#endif // NDK_ENTITY_HPP
