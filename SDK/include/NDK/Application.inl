// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <Nazara/Core/ErrorFlags.hpp>
#include <NDK/Sdk.hpp>

namespace Ndk
{
	inline Application::Application()
	{
		NzErrorFlags errFlags(nzErrorFlag_ThrowException, true);

		// Initialisation du SDK
		Sdk::Initialize();
	}

	inline Application::~Application()
	{
		// Libération du SDK
		Sdk::Uninitialize();

		// Libération automatique des modules
	}
}
