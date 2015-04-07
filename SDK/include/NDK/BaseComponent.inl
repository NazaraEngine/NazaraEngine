// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/Error.hpp>

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

	inline ComponentIndex BaseComponent::RegisterComponent(ComponentId id, Factory factoryFunc)
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

	inline bool BaseComponent::Initialize()
	{
		// Rien à faire
		return true;
	}

	inline void BaseComponent::Uninitialize()
	{
		s_entries.clear();
		s_idToIndex.clear();
	}
}
