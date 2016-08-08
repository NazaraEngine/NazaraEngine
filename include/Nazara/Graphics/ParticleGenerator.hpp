// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PARTICLEGENERATOR_HPP
#define NAZARA_PARTICLEGENERATOR_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/ObjectLibrary.hpp>
#include <Nazara/Core/ObjectRef.hpp>
#include <Nazara/Core/RefCounted.hpp>
#include <Nazara/Core/Signal.hpp>
#include <Nazara/Graphics/Config.hpp>

namespace Nz
{
	class ParticleGenerator;
	class ParticleMapper;
	class ParticleGroup;

	using ParticleGeneratorConstRef = ObjectRef<const ParticleGenerator>;
	using ParticleGeneratorLibrary = ObjectLibrary<ParticleGenerator>;
	using ParticleGeneratorRef = ObjectRef<ParticleGenerator>;

	class NAZARA_GRAPHICS_API ParticleGenerator : public RefCounted
	{
		friend ParticleGeneratorLibrary;
		friend class Graphics;

		public:
			ParticleGenerator() = default;
			ParticleGenerator(const ParticleGenerator& generator);
			virtual ~ParticleGenerator();

			virtual void Generate(ParticleGroup& system, ParticleMapper& mapper, unsigned int startId, unsigned int endId) = 0;

			// Signals:
			NazaraSignal(OnParticleGeneratorRelease, const ParticleGenerator* /*particleGenerator*/);

		private:
			static bool Initialize();
			static void Uninitialize();

			static ParticleGeneratorLibrary::LibraryMap s_library;
	};
}

#endif // NAZARA_PARTICLEGENERATOR_HPP
