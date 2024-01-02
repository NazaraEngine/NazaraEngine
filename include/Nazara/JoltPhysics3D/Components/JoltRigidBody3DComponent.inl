// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <cassert>
#include <Nazara/JoltPhysics3D/Debug.hpp>

namespace Nz
{
	inline JoltRigidBody3DComponent::JoltRigidBody3DComponent(const JoltRigidBody3D::DynamicSettings& settings)
	{
		m_settings = std::make_unique<Setting>(settings);
	}

	inline JoltRigidBody3DComponent::JoltRigidBody3DComponent(const JoltRigidBody3D::StaticSettings& settings)
	{
		m_settings = std::make_unique<Setting>(settings);
	}

	inline void JoltRigidBody3DComponent::Construct(JoltPhysWorld3D& world)
	{
		assert(m_settings);
		std::visit([&](auto&& arg)
		{
			Create(world, arg);
		}, *m_settings);
		m_settings.reset();
	}
}

#include <Nazara/JoltPhysics3D/DebugOff.hpp>
