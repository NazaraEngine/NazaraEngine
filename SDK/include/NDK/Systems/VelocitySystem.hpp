// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_SYSTEMS_VELOCITYSYSTEM_HPP
#define NDK_SYSTEMS_VELOCITYSYSTEM_HPP

#include <Nazara/Math/Vector3.hpp>
#include <NDK/System.hpp>

namespace Ndk
{
	class NDK_API VelocitySystem : public System<VelocitySystem>
	{
		public:
			VelocitySystem();
			~VelocitySystem() = default;

			static SystemIndex systemIndex;

		private:
			Nz::Vector3f DampedString(const Nz::Vector3f& currentPos, const Nz::Vector3f& targetPos, float frametime, float springStrength = 3.f) const;
			void OnUpdate(float elapsedTime) override;
	};
}

#include <NDK/Systems/VelocitySystem.inl>

#endif // NDK_SYSTEMS_VELOCITYSYSTEM_HPP
