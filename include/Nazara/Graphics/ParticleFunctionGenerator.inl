// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace Nz
{
	inline ParticleFunctionGenerator::ParticleFunctionGenerator(Generator generator) :
	m_generator(std::move(generator))
	{
	}

	/*!
	* \brief Gets the generator function
	*
	* \return Generator function responsible for particle creation
	*/
	inline const ParticleFunctionGenerator::Generator& ParticleFunctionGenerator::GetGenerator() const
	{
		return m_generator;
	}

	/*!
	* \brief Sets the generator function
	*
	* \remark The generator function must be valid
	*/
	inline void ParticleFunctionGenerator::SetGenerator(Generator generator)
	{
		m_generator = std::move(generator);
	}

	template<typename... Args>
	ParticleFunctionGeneratorRef ParticleFunctionGenerator::New(Args&&... args)
	{
		std::unique_ptr<ParticleFunctionGenerator> object(new ParticleFunctionGenerator(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Graphics/DebugOff.hpp>
