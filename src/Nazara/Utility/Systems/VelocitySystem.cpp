// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Systems/VelocitySystem.hpp>
#include <Nazara/Utility/Components/NodeComponent.hpp>
#include <Nazara/Utility/Components/VelocityComponent.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	void VelocitySystem::Update(float elapsedTime)
	{
		auto view = m_registry.view<NodeComponent, VelocityComponent>();
		for (auto [entity, nodeComponent, velocityComponent] : view.each())
		{
			NazaraUnused(entity);
			nodeComponent.Move(velocityComponent.GetLinearVelocity() * elapsedTime);
		}
	}
}
