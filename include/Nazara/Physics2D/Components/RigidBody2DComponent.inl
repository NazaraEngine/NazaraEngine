// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline RigidBody2DComponent::RigidBody2DComponent(const RigidBody2D::DynamicSettings& settings)
	{
		m_settings = std::make_unique<Setting>(settings);
	}

	inline RigidBody2DComponent::RigidBody2DComponent(const RigidBody2D::StaticSettings& settings)
	{
		m_settings = std::make_unique<Setting>(settings);
	}

	inline void RigidBody2DComponent::Construct(PhysWorld2D& world)
	{
		assert(m_settings);
		std::visit([&](auto&& arg)
		{
			Create(world, arg);
		}, *m_settings);
		m_settings.reset();
	}
}

