// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Algorithm.hpp>
#include <type_traits>

namespace Ndk
{
	template<typename ComponentType>
	Component<ComponentType>::Component() :
	BaseComponent(GetComponentIndex<ComponentType>())
	{
	}

	template<typename ComponentType>
	Component<ComponentType>::~Component() = default;

	template<typename ComponentType>
	BaseComponent* Component<ComponentType>::Clone() const
	{
		///FIXME: Pas encore supporté par GCC (4.9.2)
		//static_assert(std::is_trivially_copy_constructible<ComponentType>::value, "ComponentType must be copy-constructible");

		return new ComponentType(static_cast<const ComponentType&>(*this));
	}

	template<typename ComponentType>
	ComponentIndex Component<ComponentType>::RegisterComponent(ComponentId id)
	{
		// On utilise les lambda pour créer une fonction factory
		auto factory = []() -> BaseComponent*
		{
			return nullptr; //< Temporary workaround to allow non-default-constructed components, will be updated for serialization
			//return new ComponentType;
		};

		return BaseComponent::RegisterComponent(id, factory);
	}

	template<typename ComponentType>
	template<unsigned int N>
	ComponentIndex Component<ComponentType>::RegisterComponent(const char (&name)[N])
	{
		// On récupère la chaîne de caractère sous la forme d'un nombre qui servira d'identifiant unique
		ComponentId id = BuildComponentId(name);
		return RegisterComponent(id);
	}
}
