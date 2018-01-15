// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PARTICLEFUNCTIONCONTROLLER_HPP
#define NAZARA_PARTICLEFUNCTIONCONTROLLER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Graphics/ParticleController.hpp>
#include <functional>

namespace Nz
{
	class ParticleFunctionController;

	using ParticleFunctionControllerConstRef = ObjectRef<const ParticleFunctionController>;
	using ParticleFunctionControllerRef = ObjectRef<ParticleFunctionController>;

	class NAZARA_GRAPHICS_API ParticleFunctionController : public ParticleController
	{
		public:
			using Controller = std::function<void(ParticleGroup& /*group*/, ParticleMapper& /*mapper*/, unsigned int /*startId*/, unsigned int /*endId*/, float /*elapsedTime*/)>;

			inline ParticleFunctionController(Controller controller);
			ParticleFunctionController(const ParticleFunctionController&) = default;
			~ParticleFunctionController() = default;

			void Apply(ParticleGroup& group, ParticleMapper& mapper, unsigned int startId, unsigned int endId, float elapsedTime) override final;

			inline const Controller& GetController() const;

			inline void SetController(Controller controller);

			template<typename... Args> static ParticleFunctionControllerRef New(Args&&... args);

		private:
			Controller m_controller;
	};
}

#include <Nazara/Graphics/ParticleFunctionController.inl>

#endif // NAZARA_PARTICLEFUNCTIONCONTROLLER_HPP
