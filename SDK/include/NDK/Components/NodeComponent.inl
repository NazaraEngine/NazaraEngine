// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/Error.hpp>
#include <NDK/Entity.hpp>

namespace Ndk
{
	/*!
	* \brief Sets the parent node of the entity
	*
	* \param entity Pointer to the entity considered as parent
	* \param keepDerived Should this component considered as a derived
	*
	* \remark Produces a NazaraAssert if entity has no component NodeComponent
	*/

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
