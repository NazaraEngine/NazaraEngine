// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Entity.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <algorithm>
#include <type_traits>
#include <utility>

namespace Ndk
{
	/*!
	* \brief Adds a component to the entity
	* \return A reference to the newly added component
	*
	* \param args Arguments to create in place the component to add to the entity
	*/

	template<typename ComponentType, typename... Args>
	ComponentType& Entity::AddComponent(Args&&... args)
	{
		static_assert(std::is_base_of<BaseComponent, ComponentType>::value, "ComponentType is not a component");

		// Affectation and return of the component
		std::unique_ptr<ComponentType> ptr(new ComponentType(std::forward<Args>(args)...));
		return static_cast<ComponentType&>(AddComponent(std::move(ptr)));
	}

	/*!
	* \brief Enables the entity
	*
	* \param enable Should the entity be enabled
	*/

	inline void Entity::Enable(bool enable)
	{
		if (m_enabled != enable)
		{
			m_enabled = enable;
			Invalidate();
		}
	}

	/*!
	* \brief Gets a component in the entity by index
	* \return A reference to the component
	*
	* \param index Index of the component
	*
	* \remark Produces a NazaraAssert if component is not available in this entity or is invalid
	*/

	inline BaseComponent& Entity::GetComponent(ComponentIndex index)
	{
		NazaraAssert(HasComponent(index), "This component is not part of the entity");

		BaseComponent* component = m_components[index].get();
		NazaraAssert(component, "Invalid component pointer");

		return *component;
	}

	/*!
	* \brief Gets a component in the entity by type
	* \return A reference to the component
	*
	* \remark Produces a NazaraAssert if component is not available in this entity
	*/

	template<typename ComponentType>
	ComponentType& Entity::GetComponent()
	{
		static_assert(std::is_base_of<BaseComponent, ComponentType>::value, "ComponentType is not a component");

		ComponentIndex index = GetComponentIndex<ComponentType>();
		return static_cast<ComponentType&>(GetComponent(index));
	}

	/*!
	* \brief Gets a component in the entity by index
	* \return A constant reference to the component
	*
	* \param index Index of the component
	*
	* \remark Produces a NazaraAssert if component is not available in this entity or is invalid
	*/

	inline const BaseComponent& Entity::GetComponent(ComponentIndex index) const
	{
		NazaraAssert(HasComponent(index), "This component is not part of the entity");

		BaseComponent* component = m_components[index].get();
		NazaraAssert(component, "Invalid component pointer");

		return *component;
	}

	/*!
	* \brief Gets a component in the entity by type
	* \return A constant reference to the component
	*
	* \remark Produces a NazaraAssert if component is not available in this entity
	*/

	template<typename ComponentType>
	const ComponentType& Entity::GetComponent() const
	{
		static_assert(std::is_base_of<BaseComponent, ComponentType>::value, "ComponentType is not a component");

		ComponentIndex index = GetComponentIndex<ComponentType>();
		return static_cast<ComponentType&>(GetComponent(index));
	}

	/*!
	* \brief Gets the bits representing the components in the entiy
	* \return A constant reference to the set of component's bits
	*/

	inline const Nz::Bitset<>& Entity::GetComponentBits() const
	{
		return m_componentBits;
	}

	/*!
	* \brief Gets the identifier of the entity
	* \return Identifier of the entity
	*/

	inline EntityId Entity::GetId() const
	{
		return m_id;
	}

	/*!
	* \brief Gets the bits representing the systems in the entiy
	* \return A constant reference to the set of system's bits
	*/

	inline const Nz::Bitset<>& Entity::GetSystemBits() const
	{
		return m_systemBits;
	}

	/*!
	* \brief Gets the world in which the entity is
	* \return Pointer to the world
	*/

	inline World* Entity::GetWorld() const
	{
		return m_world;
	}

	/*!
	* \brief Checks whether or not a component is present in the entity by index
	* \return true If it is the case
	*
	* \param index Index of the component
	*/

	inline bool Entity::HasComponent(ComponentIndex index) const
	{
		return m_componentBits.UnboundedTest(index);
	}

	/*!
	* \brief Checks whether or not a component is present in the entity by type
	* \return true If it is the case
	*/

	template<typename ComponentType>
	bool Entity::HasComponent() const
	{
		static_assert(std::is_base_of<BaseComponent, ComponentType>::value, "ComponentType is not a component");

		ComponentIndex index = GetComponentIndex<ComponentType>();
		return HasComponent(index);
	}

	/*!
	* \brief Checks whether or not the entity is enabled
	* \return true If it is the case
	*/

	inline bool Entity::IsEnabled() const
	{
		return m_enabled;
	}

	/*!
	* \brief Checks whether or not the entity is valid
	* \return true If it is the case
	*/

	inline bool Entity::IsValid() const
	{
		return m_valid;
	}

	/*!
	* \brief Removes every components
	*/

	inline void Entity::RemoveAllComponents()
	{
		m_removedComponentBits = m_componentBits;

		Invalidate();
	}

	/*!
	* \brief Removes a component in the entity by index
	*
	* \param index Index of the component
	*/

	inline void Entity::RemoveComponent(ComponentIndex index)
	{
		m_removedComponentBits.UnboundedSet(index);

		Invalidate();
	}

	/*!
	* \brief Removes a component in the entity by type
	*/

	template<typename ComponentType>
	void Entity::RemoveComponent()
	{
		static_assert(std::is_base_of<BaseComponent, ComponentType>(), "ComponentType is not a component");

		ComponentIndex index = GetComponentIndex<ComponentType>();
		RemoveComponent(index);
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object: "Entity(GetId())"
	*/

	inline Nz::String Entity::ToString() const
	{
		Nz::StringStream ss;
		return ss << "Entity(" << GetId() << ')';
	}

	/*!
	* \brief Gets the bits representing the removed components in the entiy
	* \return A constant reference to the set of remove component's bits
	*/

	inline Nz::Bitset<>& Entity::GetRemovedComponentBits()
	{
		return m_removedComponentBits;
	}

	/*!
	* \brief Registers a system for the entity
	*
	* \param index Index of the system
	*/

	inline void Entity::RegisterSystem(SystemIndex index)
	{
		m_systemBits.UnboundedSet(index);
	}

	/*!
	* \brief Sets the world of the entity
	*
	* \param world World in which the entity will be
	*
	* \remark Produces a NazaraAssert if world is invalid
	*/

	inline void Entity::SetWorld(World* world) noexcept
	{
		NazaraAssert(world, "An entity must be attached to a world at any time");

		m_world = world;
	}

	/*!
	* \brief Unregisters a system for the entity
	*
	* \param index Index of the system
	*/

	inline void Entity::UnregisterSystem(SystemIndex index)
	{
		m_systemBits.UnboundedReset(index);
	}
}

namespace std
{
	template<>
	struct hash<Ndk::EntityHandle>
	{
		/*!
		* \brief Specialisation of std to hash
		* \return Result of the hash
		*
		* \param handle Entity to hash
		*/
		size_t operator()(const Ndk::EntityHandle& handle) const
		{
			// Hash the pointer will work until the entity is updated and moved
			// so, we have to hash the ID of the entity (which is constant)
			Ndk::EntityId id = (handle.IsValid()) ? handle->GetId() : std::numeric_limits<Ndk::EntityId>::max();

			return hash<Ndk::EntityId>()(id);
		}
	};
}