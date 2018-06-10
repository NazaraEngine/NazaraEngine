// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	/*!
	* \brief Gets a raw pointer to the particle buffer
	*
	* This can be useful when working directly with a struct, or needing to iterate over all particles.
	*
	* \return Pointer to the buffer
	*
	* \see GetParticleCount
	*/
	inline void* ParticleGroup::GetBuffer()
	{
		return m_buffer.data();
	}

	/*!
	* \brief Gets a raw pointer to the particle buffer
	*
	* This can be useful when working directly with a struct, or needing to iterate over all particles.
	*
	* \return Pointer to the buffer
	*
	* \see GetParticleCount
	*/
	inline const void* ParticleGroup::GetBuffer() const
	{
		return m_buffer.data();
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
