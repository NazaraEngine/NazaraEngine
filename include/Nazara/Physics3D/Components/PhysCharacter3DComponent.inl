// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline PhysCharacter3DComponent::PhysCharacter3DComponent(const PhysCharacter3D::Settings& settings)
	{
		m_settings = std::make_unique<PhysCharacter3D::Settings>(settings);
	}

	inline void PhysCharacter3DComponent::Construct(PhysWorld3D& world)
	{
		assert(m_settings);
		Create(world, *m_settings);
		m_settings.reset();
	}
}

