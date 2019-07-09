// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#pragma once

#ifndef NDK_SDK_HPP
#define NDK_SDK_HPP

#include <NDK/Prerequisites.hpp>

namespace Ndk
{
	class NDK_API Sdk
	{
		public:
			Sdk() = delete;
			~Sdk() = delete;

			static bool Initialize();
			static bool IsInitialized();
			static void Uninitialize();

		private:
			static unsigned int s_referenceCounter;
	};
}

#include <NDK/Sdk.inl>

#endif // NDK_SDK_HPP
