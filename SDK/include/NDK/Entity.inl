// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/Error.hpp>
#include <algorithm>
#include <type_traits>
#include <utility>

namespace Ndk
{
	inline Entity::Entity(World& world, EntityId id) :
	m_id(id),
	m_world(&world)
	{
	}

	template<typename ComponentType, typename... Args>
	ComponentType& Entity::AddComponent(Args&&... args)
	{
		static_assert(std::is_base_of<BaseComponent, ComponentType>(), "ComponentType is not a component");

		// Allocation et affectation du component
		std::unique_ptr<ComponentType> ptr(new ComponentType(std::forward(args)...));
		return static_cast<ComponentType&>(AddComponent(std::move(ptr)));
	}

	inline BaseComponent& Entity::GetComponent(ComponentIndex index)
	{
		///DOC: Le component doit être présent
		NazaraAssert(HasComponent(index), "This component is not part of the entity");

		BaseComponent* component = m_components[index].get();
		NazaraAssert(component, "Invalid component pointer");

		return *component;
	}

	template<typename ComponentType>
	ComponentType& Entity::GetComponent()
	{
		///DOC: Le component doit être présent
		static_assert(std::is_base_of<BaseComponent, ComponentType>(), "ComponentType is not a component");

		ComponentIndex index = GetComponentIndex<ComponentType>();
		return static_cast<ComponentType&>(GetComponent(index));
	}

	inline const NzBitset<>& Entity::GetComponentBits() const
	{
		return m_componentBits;
	}

	inline EntityId Entity::GetId() const
	{
		return m_id;
	}

	inline const NzBitset<>& Entity::GetSystemBits() const
	{
		return m_systemBits;
	}

	inline World* Entity::GetWorld() const
	{
		return m_world;
	}

	inline bool Entity::HasComponent(ComponentIndex index) const
	{
		return m_componentBits.UnboundedTest(index);
	}

	template<typename ComponentType>
	bool Entity::HasComponent() const
	{
		static_assert(std::is_base_of<BaseComponent, ComponentType>(), "ComponentType is not a component");

		ComponentIndex index = GetComponentIndex<ComponentType>();
		return HasComponent(index);
	}

	inline bool Entity::IsValid() const
	{
		return m_valid;
	}

	template<typename ComponentType>
	void Entity::RemoveComponent()
	{
		static_assert(std::is_base_of<BaseComponent, ComponentType>(), "ComponentType is not a component");

		ComponentIndex index = GetComponentIndex<ComponentType>();
		RemoveComponent(index);
	}

	inline void Entity::RegisterHandle(EntityHandle* handle)
	{
		///DOC: Un handle ne doit être enregistré qu'une fois, des erreurs se produisent s'il l'est plus d'une fois
		m_handles.push_back(handle);
	}

	inline void Entity::RegisterSystem(SystemIndex index)
	{
		m_systemBits.UnboundedSet(index);
	}

	inline void Entity::UnregisterHandle(EntityHandle* handle)
	{
		///DOC: Un handle ne doit être libéré qu'une fois, et doit faire partie de la liste, sous peine de crash
		auto it = std::find(m_handles.begin(), m_handles.end(), handle);

		// On échange cet élément avec le dernier, et on diminue la taille du vector de 1
		std::swap(*it, m_handles.back());
		m_handles.pop_back();
	}

	inline void Entity::UnregisterSystem(SystemIndex index)
	{
		m_systemBits.UnboundedReset(index);
	}
}
