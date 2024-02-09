// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <cassert>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	inline RigidBody3DComponent::RigidBody3DComponent(const RigidBody3D::DynamicSettings& settings)
	{
		m_settings = std::make_unique<Setting>(settings);
	}

	inline RigidBody3DComponent::RigidBody3DComponent(const RigidBody3D::StaticSettings& settings)
	{
		m_settings = std::make_unique<Setting>(settings);
	}

	inline void RigidBody3DComponent::Construct(PhysWorld3D& world)
	{
		assert(m_settings);
		std::visit([&](auto&& arg)
		{
			Create(world, arg);
		}, *m_settings);
		m_settings.reset();
	}
}

#include <Nazara/Physics3D/DebugOff.hpp>
