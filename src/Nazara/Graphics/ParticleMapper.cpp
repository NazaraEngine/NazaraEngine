// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/ParticleMapper.hpp>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup graphics
	* \class Nz::ParticleMapper
	* \brief Graphics class that represents the mapping between the internal buffer and the particle declaration
	*/

	/*!
	* \brief Constructs a ParticleMapper object with a raw buffer and a particle declaration
	*
	* \param buffer Raw buffer to store particles data
	* \param declaration Declaration of the particle
	*/

	ParticleMapper::ParticleMapper(void* buffer, const ParticleDeclaration* declaration) :
	m_declaration(declaration),
	m_ptr(static_cast<UInt8*>(buffer))
	{
	}

	ParticleMapper::~ParticleMapper() = default;

}
