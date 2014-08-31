// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PARTICLECONTROLLER_HPP
#define NAZARA_PARTICLECONTROLLER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceRef.hpp>

class NzParticleController;
class NzParticleMapper;
class NzParticleSystem;

using NzParticleControllerConstRef = NzResourceRef<const NzParticleController>;
using NzParticleControllerRef = NzResourceRef<NzParticleController>;

class NAZARA_API NzParticleController : public NzResource
{
	public:
		NzParticleController() = default;
		NzParticleController(const NzParticleController& controller);
		virtual ~NzParticleController();

		virtual void Apply(NzParticleSystem& system, NzParticleMapper& mapper, unsigned int startId, unsigned int endId, float elapsedTime) = 0;
};

#endif // NAZARA_PARTICLECONTROLLER_HPP
