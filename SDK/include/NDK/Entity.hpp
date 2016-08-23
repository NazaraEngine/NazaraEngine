// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_ENTITY_HPP
#define NDK_ENTITY_HPP

#include <Nazara/Core/Bitset.hpp>
#include <Nazara/Core/HandledObject.hpp>
#include <NDK/Algorithm.hpp>
#include <memory>
#include <vector>

namespace Ndk
{
	class BaseComponent;
	class Entity;
	class World;

	using EntityHandle = Nz::ObjectHandle<Entity>;

	class NDK_API Entity : public Nz::HandledObject<Entity>
	{
		friend class BaseSystem;
		friend World;

		public:
			Entity(const Entity&) = delete;
			Entity(Entity&& entity);
			~Entity();

			BaseComponent& AddComponent(std::unique_ptr<BaseComponent>&& component);
			template<typename ComponentType, typename... Args> ComponentType& AddComponent(Args&&... args);

			const EntityHandle& Clone() const;

			inline void Enable(bool enable = true);

			inline BaseComponent& GetComponent(ComponentIndex index);
			template<typename ComponentType> ComponentType& GetComponent();
			inline const BaseComponent& GetComponent(ComponentIndex index) const;
			template<typename ComponentType> const ComponentType& GetComponent() const;
			inline const Nz::Bitset<>& GetComponentBits() const;
			inline EntityId GetId() const;
			inline const Nz::Bitset<>& GetSystemBits() const;
			inline World* GetWorld() const;

			inline bool HasComponent(ComponentIndex index) const;
			template<typename ComponentType> bool HasComponent() const;

			void Kill();

			void Invalidate();
			inline bool IsEnabled() const;
			inline bool IsValid() const;

			inline void RemoveAllComponents();
			inline void RemoveComponent(ComponentIndex index);
			template<typename ComponentType> void RemoveComponent();

			inline Nz::String ToString() const;

			Entity& operator=(const Entity&) = delete;
			Entity& operator=(Entity&&) = delete;

		private:
			Entity(World* world, EntityId id);

			void Create();
			void Destroy();

			void DestroyComponent(ComponentIndex index);

			inline Nz::Bitset<>& GetRemovedComponentBits();

			inline void RegisterSystem(SystemIndex index);

			inline void SetWorld(World* world) noexcept;

			inline void UnregisterSystem(SystemIndex index);

			std::vector<std::unique_ptr<BaseComponent>> m_components;
			Nz::Bitset<> m_componentBits;
			Nz::Bitset<> m_removedComponentBits;
			Nz::Bitset<> m_systemBits;
			EntityId m_id;
			World* m_world;
			bool m_enabled;
			bool m_valid;
	};
}

#include <NDK/Entity.inl>

#endif // NDK_ENTITY_HPP
