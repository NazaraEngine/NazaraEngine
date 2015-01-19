// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ParticleMapper.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Graphics/Debug.hpp>

NzParticleMapper::NzParticleMapper(void* buffer, const NzParticleDeclaration* declaration) :
m_declaration(declaration),
m_ptr(static_cast<nzUInt8*>(buffer))
{
}

NzParticleMapper::~NzParticleMapper() = default;
