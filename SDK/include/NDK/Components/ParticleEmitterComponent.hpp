// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_SERVER
#ifndef NDK_COMPONENTS_PARTICLEEMITTERCOMPONENT_HPP
#define NDK_COMPONENTS_PARTICLEEMITTERCOMPONENT_HPP

#include <Nazara/Graphics/ParticleEmitter.hpp>
#include <Nazara/Graphics/ParticleGroup.hpp>
#include <NDK/Component.hpp>

namespace Ndk
{
	class NDK_API ParticleEmitterComponent : public Component<ParticleEmitterComponent>, public Nz::ParticleEmitter
	{
		public:
			using SetupFunc = std::function<void(const EntityHandle& /*entity*/, Nz::ParticleMapper& /*mapper*/, unsigned int /*count*/)>;

			inline ParticleEmitterComponent();
			ParticleEmitterComponent(const ParticleEmitterComponent& emitter) = default;
			ParticleEmitterComponent(ParticleEmitterComponent&& emitter) = default;
			~ParticleEmitterComponent() = default;

			inline void Enable(bool active = true);

			inline bool IsActive() const;

			inline void SetSetupFunc(SetupFunc func);

			static ComponentIndex componentIndex;

		private:
			void SetupParticles(Nz::ParticleMapper& mapper, unsigned int count) const override;

			SetupFunc m_setupFunc;
			bool m_isActive;
	};
}

#include <NDK/Components/ParticleEmitterComponent.inl>

#endif // NDK_COMPONENTS_PARTICLEEMITTERCOMPONENT_HPP
#endif // NDK_SERVER