// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_SYSTEMS_VELOCITYSYSTEM_HPP
#define NDK_SYSTEMS_VELOCITYSYSTEM_HPP

#include <NazaraSDK/System.hpp>

namespace Ndk
{
	class NDK_API VelocitySystem : public System<VelocitySystem>
	{
		public:
			VelocitySystem();
			~VelocitySystem() = default;

			static SystemIndex systemIndex;

		private:
			void OnUpdate(float elapsedTime) override;
	};
}

#include <NazaraSDK/Systems/VelocitySystem.inl>

#endif // NDK_SYSTEMS_VELOCITYSYSTEM_HPP
