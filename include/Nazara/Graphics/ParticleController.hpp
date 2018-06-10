// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PARTICLECONTROLLER_HPP
#define NAZARA_PARTICLECONTROLLER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Graphics/Config.hpp>

namespace Nz
{
	class ParticleController;
	class ParticleMapper;
	class ParticleGroup;

	using ParticleControllerConstRef = ObjectRef<const ParticleController>;
	using ParticleControllerLibrary = ObjectLibrary<ParticleController>;
	using ParticleControllerRef = ObjectRef<ParticleController>;

	class NAZARA_GRAPHICS_API ParticleController : public RefCounted
	{
		friend ParticleControllerLibrary;
		friend class Graphics;

		public:
			ParticleController() = default;
			ParticleController(const ParticleController& controller);
			virtual ~ParticleController();

			virtual void Apply(ParticleGroup& system, ParticleMapper& mapper, unsigned int startId, unsigned int endId, float elapsedTime) = 0;

			// Signals:
			NazaraSignal(OnParticleControllerRelease, const ParticleController* /*particleController*/);

		private:
			static bool Initialize();
			static void Uninitialize();

			static ParticleControllerLibrary::LibraryMap s_library;
	};
}

#endif // NAZARA_PARTICLECONTROLLER_HPP
