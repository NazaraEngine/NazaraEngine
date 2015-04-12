// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_SYSTEMS_VELOCITYSYSTEM_HPP
#define NDK_SYSTEMS_VELOCITYSYSTEM_HPP

#include <NDK/System.hpp>

namespace Ndk
{
	class NDK_API VelocitySystem : public System<VelocitySystem>
	{
		public:
			VelocitySystem();
			~VelocitySystem() = default;

			void Update(float elapsedTime);

			static SystemIndex systemIndex;
	};
}

#include <NDK/Systems/VelocitySystem.inl>

#endif // NDK_SYSTEMS_VELOCITYSYSTEM_HPP
