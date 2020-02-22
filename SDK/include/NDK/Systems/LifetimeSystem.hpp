// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_SYSTEMS_LIFETIMESYSTEM_HPP
#define NDK_SYSTEMS_LIFETIMESYSTEM_HPP

#include <NDK/System.hpp>

namespace Ndk
{
	class NDK_API LifetimeSystem : public System<LifetimeSystem>
	{
		public:
			LifetimeSystem();
			~LifetimeSystem() = default;

			static SystemIndex systemIndex;

		private:
			void OnUpdate(float elapsedTime) override;
	};
}

#include <NDK/Systems/LifetimeSystem.inl>

#endif // NDK_SYSTEMS_LIFETIMESYSTEM_HPP
