// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_SERVER
#ifndef NDK_COMPONENTS_PARTICLEEMITTERCOMPONENT_HPP
#define NDK_COMPONENTS_PARTICLEEMITTERCOMPONENT_HPP

#include <Nazara/Graphics/ParticleEmitter.hpp>
#include <NDK/Component.hpp>

namespace Ndk
{
	class NDK_API ParticleEmitterComponent : public Component<ParticleEmitterComponent>, public Nz::ParticleEmitter
	{
		public:
			using SetupFunc = std::function<void(const EntityHandle& /*entity*/, Nz::ParticleMapper& /*mapper*/, unsigned int /*count*/)>;

			ParticleEmitterComponent(Nz::ParticleGroup* group);
			ParticleEmitterComponent(const ParticleEmitterComponent& emitter);
			ParticleEmitterComponent(ParticleEmitterComponent&& emitter);
			~ParticleEmitterComponent();

			inline bool IsActive() const;

			void SetActive(bool active = true);
			void SetGroup(Nz::ParticleGroup* group);
			inline void SetSetupFunc(SetupFunc func);

			static ComponentIndex componentIndex;

		private:
			void OnParticleGroupRelease(const Nz::ParticleGroup* particleGroup);

			void SetupParticles(Nz::ParticleMapper& mapper, unsigned int count) const override;

			NazaraSlot(Nz::ParticleGroup, OnParticleGroupRelease, m_particleGroupRelease);

			SetupFunc m_setupFunc;
			Nz::ParticleGroup* m_particleGroup;
			bool m_isActive;
	};
}

#include <NDK/Components/ParticleEmitterComponent.inl>

#endif // NDK_COMPONENTS_PARTICLEEMITTERCOMPONENT_HPP
#endif // NDK_SERVER