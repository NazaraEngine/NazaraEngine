// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_SERVER
#ifndef NDK_SYSTEMS_PARTICLESYSTEM_HPP
#define NDK_SYSTEMS_PARTICLESYSTEM_HPP

#include <NDK/System.hpp>

namespace Ndk
{
	class NDK_API ParticleSystem : public System<ParticleSystem>
	{
		public:
			ParticleSystem();
			~ParticleSystem() = default;

			static SystemIndex systemIndex;

		private:
			void OnUpdate(float elapsedTime) override;
	};
}

#include <NDK/Systems/ParticleSystem.inl>

#endif // NDK_SYSTEMS_PARTICLESYSTEM_HPP
#endif // NDK_SERVER
