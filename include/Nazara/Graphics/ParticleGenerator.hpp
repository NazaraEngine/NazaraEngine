// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PARTICLEGENERATOR_HPP
#define NAZARA_PARTICLEGENERATOR_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceRef.hpp>

class NzParticleEmitter;
class NzParticleGenerator;
class NzParticleMapper;

using NzParticleGeneratorConstRef = NzResourceRef<const NzParticleGenerator>;
using NzParticleGeneratorRef = NzResourceRef<NzParticleGenerator>;

class NAZARA_API NzParticleGenerator : public NzResource
{
	public:
		NzParticleGenerator() = default;
		NzParticleGenerator(const NzParticleGenerator& generator);
		virtual ~NzParticleGenerator();

		virtual void Generate(NzParticleEmitter& emitter, NzParticleMapper& mapper, unsigned int startId, unsigned int endId) = 0;
};

#endif // NAZARA_PARTICLEGENERATOR_HPP
