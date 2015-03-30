// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/Error.hpp>
#include <Ndk/Algorithm.hpp>
#include <type_traits>

namespace Ndk
{
	inline BaseComponent::BaseComponent(ComponentIndex index) :
	m_componentIndex(index)
	{
	}

	inline ComponentIndex BaseComponent::GetIndex() const
	{
		return m_componentIndex;
	}

	template<typename ComponentType, unsigned int N>
	ComponentIndex BaseComponent::Register(const char (&name)[N])
	{
		// Il faut que notre composant possède un constructeur par défaut (pour la factory)
		static_assert(std::is_default_constructible<ComponentType>::value, "ComponentType should be default-constructible");

		// On récupère la chaîne de caractère sous la forme d'un nombre qui servira d'identifiant unique
		ComponentId id = BuildComponentId(name);

		// On utilise les lambda pour créer une fonction factory
		auto factory = []() -> BaseComponent*
		{
			return new ComponentType;
		};

		return Register(id, factory);
	}

	inline ComponentIndex BaseComponent::Register(ComponentId id, Factory factoryFunc)
	{
		// Nous allons rajouter notre composant à la fin
		ComponentIndex index = s_entries.size();
		s_entries.resize(index + 1);

		// On récupère et on affecte
		ComponentEntry& entry = s_entries.back();
		entry.factory = factoryFunc;
		entry.id = id;

		// Une petite assertion pour s'assurer que l'identifiant n'est pas déjà utilisé
		NazaraAssert(s_idToIndex.find(id) == s_idToIndex.end(), "This id is already in use");

		s_idToIndex[id] = index;

		return index;
	}
}
