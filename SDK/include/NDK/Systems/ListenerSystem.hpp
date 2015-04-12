// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#pragma once

#ifndef NDK_SYSTEMS_LISTENERSYSTEM_HPP
#define NDK_SYSTEMS_LISTENERSYSTEM_HPP

#include <NDK/System.hpp>

namespace Ndk
{
	class NDK_API ListenerSystem : public System<ListenerSystem>
	{
		public:
			ListenerSystem();
			~ListenerSystem() = default;

			void Update(float elapsedTime);

			static SystemIndex systemIndex;
	};
}

#include <NDK/Systems/ListenerSystem.inl>

#endif // NDK_SYSTEMS_LISTENERSYSTEM_HPP
