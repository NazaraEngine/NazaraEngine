// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_SYSTEMS_STATICCOLLISIONSYSTEM_HPP
#define NDK_SYSTEMS_STATICCOLLISIONSYSTEM_HPP

#include <NDK/System.hpp>

namespace Ndk
{
	class NDK_API StaticCollisionSystem : public System<StaticCollisionSystem>
	{
		public:
			StaticCollisionSystem();
			StaticCollisionSystem(const StaticCollisionSystem& system) = default;
			~StaticCollisionSystem() = default;

			void Update(float elapsedTime);

			static SystemIndex systemIndex;
	};
}

#include <NDK/Systems/StaticCollisionSystem.inl>

#endif // NDK_SYSTEMS_STATICCOLLISIONSYSTEM_HPP
