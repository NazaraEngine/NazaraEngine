// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_SERVER
#ifndef NDK_COMPONENTS_PARTICLEGROUPCOMPONENT_HPP
#define NDK_COMPONENTS_PARTICLEGROUPCOMPONENT_HPP

#include <Nazara/Graphics/ParticleGroup.hpp>
#include <NDK/Component.hpp>

namespace Ndk
{
	class ParticleGroupComponent;

	using ParticleGroupComponentHandle = Nz::ObjectHandle<ParticleGroupComponent>;

	class NDK_API ParticleGroupComponent : public Component<ParticleGroupComponent>, public Nz::ParticleGroup, public Nz::HandledObject<ParticleGroupComponent>
	{
		public:
			inline ParticleGroupComponent(unsigned int maxParticleCount, Nz::ParticleLayout layout);
			inline ParticleGroupComponent(unsigned int maxParticleCount, Nz::ParticleDeclarationConstRef declaration);
			ParticleGroupComponent(const ParticleGroupComponent&) = default;
			~ParticleGroupComponent() = default;

			void AddEmitter(Entity* emitter);
			using ParticleGroup::AddEmitter;

			void RemoveEmitter(Entity* emitter);
			using ParticleGroup::RemoveEmitter;

			static ComponentIndex componentIndex;
	};
}

#include <NDK/Components/ParticleGroupComponent.inl>

#endif // NDK_COMPONENTS_PARTICLEGROUPCOMPONENT_HPP
#endif // NDK_SERVER