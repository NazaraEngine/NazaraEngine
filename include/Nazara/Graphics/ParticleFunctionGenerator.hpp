// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PARTICLEFUNCTIONGENERATOR_HPP
#define NAZARA_PARTICLEFUNCTIONGENERATOR_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/ParticleGenerator.hpp>
#include <functional>

namespace Nz
{
	class ParticleFunctionGenerator;

	using ParticleFunctionGeneratorConstRef = ObjectRef<const ParticleFunctionGenerator>;
	using ParticleFunctionGeneratorRef = ObjectRef<ParticleFunctionGenerator>;

	class NAZARA_GRAPHICS_API ParticleFunctionGenerator : public ParticleGenerator
	{
		public:
			using Generator = std::function<void(ParticleGroup& /*group*/, ParticleMapper& /*mapper*/, unsigned int /*startId*/, unsigned int /*endId*/)>;

			inline ParticleFunctionGenerator(Generator controller);
			ParticleFunctionGenerator(const ParticleFunctionGenerator&) = default;
			~ParticleFunctionGenerator() = default;

			void Generate(ParticleGroup& group, ParticleMapper& mapper, unsigned int startId, unsigned int endId) override final;

			inline const Generator& GetGenerator() const;

			inline void SetGenerator(Generator generator);

			template<typename... Args> static ParticleFunctionGeneratorRef New(Args&&... args);

		private:
			Generator m_generator;
	};
}

#include <Nazara/Graphics/ParticleFunctionGenerator.inl>

#endif // NAZARA_PARTICLEFUNCTIONGENERATOR_HPP
