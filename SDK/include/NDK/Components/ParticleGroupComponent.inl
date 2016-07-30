#include "ParticleGroupComponent.hpp"
// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

namespace Ndk
{
	inline ParticleGroupComponent::ParticleGroupComponent(unsigned int maxParticleCount, Nz::ParticleLayout layout) :
	ParticleGroup(maxParticleCount, layout)
	{
	}

	inline ParticleGroupComponent::ParticleGroupComponent(unsigned int maxParticleCount, Nz::ParticleDeclarationConstRef declaration) :
	ParticleGroup(maxParticleCount, std::move(declaration))
	{
	}
}
