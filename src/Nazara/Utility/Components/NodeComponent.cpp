// Copyright (C) 2021 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <Nazara/Utility/Components/NodeComponent.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	void NodeComponent::SetParent(entt::registry& registry, entt::entity entity, bool keepDerived)
	{
		NodeComponent* nodeComponent = registry.try_get<NodeComponent>(entity);
		NazaraAssert(nodeComponent, "entity doesn't have a NodeComponent");

		Node::SetParent(nodeComponent, keepDerived);
	}
}
