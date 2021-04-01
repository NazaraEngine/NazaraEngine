// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_CLIENTSDK_HPP
#define NDK_CLIENTSDK_HPP

#include <NDK/ClientPrerequisites.hpp>

namespace Ndk
{
	class NDK_CLIENT_API ClientSdk
	{
		public:
			ClientSdk() = delete;
			~ClientSdk() = delete;

			static bool Initialize();
			static bool IsInitialized();
			static void Uninitialize();

		private:
			static unsigned int s_referenceCounter;
	};
}

#include <NDK/ClientSdk.inl>

#endif // NDK_CLIENTSDK_HPP
