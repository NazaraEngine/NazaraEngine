// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_SYSTEMS_LISTENERSYSTEM_HPP
#define NDK_SYSTEMS_LISTENERSYSTEM_HPP

 #include <NazaraSDK/ClientPrerequisites.hpp>
 #include <NazaraSDK/System.hpp>

namespace Ndk
{
	class NDK_CLIENT_API ListenerSystem : public System<ListenerSystem>
	{
		public:
			ListenerSystem();
			~ListenerSystem() = default;

			static SystemIndex systemIndex;

		private:
			void OnUpdate(float elapsedTime) override;
	};
}

#include <NazaraSDK/Systems/ListenerSystem.inl>

#endif // NDK_SYSTEMS_LISTENERSYSTEM_HPP
