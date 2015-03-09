// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/Error.hpp>
#include <algorithm>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace Ndk
{
	inline Entity::Entity(World& world, Id id) :
	m_id(id),
	m_world(&world)
	{
	}

	inline BaseComponent& Entity::AddComponent(std::unique_ptr<BaseComponent>&& component)
	{
		NazaraAssert(component, "Component must be valid");

		nzUInt32 componentId = component->GetId();

		// Nous supprimons l'ancien component, s'il existe
		RemoveComponent(componentId);

		// Nous nous assurons que le vecteur de component est suffisamment grand pour contenir le nouveau component
		if (m_components.size() <= componentId)
			m_components.resize(componentId + 1);

		// Affectation et retour du component
		m_components[componentId] = std::move(component);

		return *m_components[componentId].get();
	}

	template<typename ComponentType, typename... Args>
	ComponentType& Entity::AddComponent(Args&&... args)
	{
		static_assert(std::is_base_of<BaseComponent, ComponentType>(), "ComponentType is not a component");

		// Allocation et affectation du component
		std::unique_ptr<ComponentType> ptr(new ComponentType(std::forward(args)...));
		return static_cast<ComponentType&>(AddComponent(std::move(ptr)));
	}

	inline BaseComponent& Entity::GetComponent(nzUInt32 componentId)
	{
		if (!HasComponent(componentId))
			throw std::runtime_error("Tried to get a non-present component");

		BaseComponent* component = m_components[componentId].get();
		NazaraAssert(component, "Invalid component pointer");

		return *component;
	}

	template<typename ComponentType>
	ComponentType& Entity::GetComponent()
	{
		///DOC: Lance une exception si le component n'est pas présent
		static_assert(std::is_base_of<BaseComponent, ComponentType>(), "ComponentType is not a component");

		nzUInt32 componentId = GetComponentId<ComponentType>();
		return static_cast<ComponentType&>(GetComponent(componentId));
	}

	inline Entity::Id Entity::GetId() const
	{
		return m_id;
	}

	inline World* Entity::GetWorld() const
	{
		return m_world;
	}

	inline bool Entity::HasComponent(nzUInt32 componentId) const
	{
		return m_components.size() > componentId && m_components[componentId];
	}

	template<typename ComponentType>
	bool Entity::HasComponent() const
	{
		static_assert(std::is_base_of<BaseComponent, ComponentType>(), "ComponentType is not a component");

		nzUInt32 componentId = GetComponentId<ComponentType>();
		return HasComponent(componentId);
	}

	inline void Entity::RemoveAllComponent()
	{
		m_components.clear();
	}

	inline void Entity::RemoveComponent(nzUInt32 componentId)
	{
		if (HasComponent(componentId))
			m_components[componentId].reset();
	}

	template<typename ComponentType>
	void Entity::RemoveComponent()
	{
		static_assert(std::is_base_of<BaseComponent, ComponentType>(), "ComponentType is not a component");

		nzUInt32 componentId = GetComponentId<ComponentType>();
		RemoveComponent(componentId);
	}

	inline void Entity::RegisterHandle(EntityHandle* handle)
	{
		///DOC: Un handle ne doit être enregistré qu'une fois, des erreurs se produisent s'il l'est plus d'une fois
		m_handles.push_back(handle);
	}

	inline void Entity::UnregisterHandle(EntityHandle* handle)
	{
		///DOC: Un handle ne doit être libéré qu'une fois, et doit faire partie de la liste, sous peine de crash
		auto it = std::find(m_handles.begin(), m_handles.end(), handle);

		// On échange cet élément avec le dernier, et on diminue la taille du vector de 1
		std::swap(*it, m_handles.back());
		m_handles.pop_back();
	}
}
