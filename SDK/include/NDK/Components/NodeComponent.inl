// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/Error.hpp>
#include <NDK/Entity.hpp>

namespace Ndk
{
	inline void NodeComponent::SetParent(Entity* entity, bool keepDerived)
	{
		if (entity)
		{
			NazaraAssert(entity->HasComponent<NodeComponent>(), "Entity must have a NodeComponent");

			Nz::Node::SetParent(entity->GetComponent<NodeComponent>(), keepDerived);
		}
		else
			Nz::Node::SetParent(nullptr, keepDerived);
	}
}
