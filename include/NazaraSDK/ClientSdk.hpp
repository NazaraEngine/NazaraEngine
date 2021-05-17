// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_CLIENTSDK_HPP
#define NDK_CLIENTSDK_HPP

#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <NazaraSDK/ClientPrerequisites.hpp>
#include <NazaraSDK/Sdk.hpp>

namespace Ndk
{
	class NDK_CLIENT_API ClientSdk : public Nz::ModuleBase<ClientSdk>
	{
		friend ModuleBase;

		public:
			using Dependencies = Nz::TypeList<Ndk::Sdk, Nz::Audio, Nz::Graphics>;

			struct Config {};

			ClientSdk(Config /*config*/);
			~ClientSdk();

		private:
			static ClientSdk* s_instance;
	};
}

#include <NazaraSDK/ClientSdk.inl>

#endif // NDK_CLIENTSDK_HPP
