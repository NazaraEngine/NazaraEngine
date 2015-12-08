// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ParticleMapper.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	ParticleMapper::ParticleMapper(void* buffer, const ParticleDeclaration* declaration) :
	m_declaration(declaration),
	m_ptr(static_cast<UInt8*>(buffer))
	{
	}

	ParticleMapper::~ParticleMapper() = default;

}
