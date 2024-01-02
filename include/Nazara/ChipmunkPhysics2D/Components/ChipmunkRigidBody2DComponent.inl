// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - ChipmunkPhysics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/ChipmunkPhysics2D/Debug.hpp>

namespace Nz
{
	inline ChipmunkRigidBody2DComponent::ChipmunkRigidBody2DComponent(const ChipmunkRigidBody2D::DynamicSettings& settings)
	{
		m_settings = std::make_unique<Setting>(settings);
	}

	inline ChipmunkRigidBody2DComponent::ChipmunkRigidBody2DComponent(const ChipmunkRigidBody2D::StaticSettings& settings)
	{
		m_settings = std::make_unique<Setting>(settings);
	}

	inline void ChipmunkRigidBody2DComponent::Construct(ChipmunkPhysWorld2D& world)
	{
		assert(m_settings);
		std::visit([&](auto&& arg)
		{
			Create(world, arg);
		}, *m_settings);
		m_settings.reset();
	}
}

#include <Nazara/ChipmunkPhysics2D/DebugOff.hpp>
