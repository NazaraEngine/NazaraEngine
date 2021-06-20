// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NazaraSDK/ClientSdk.hpp>
#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Physics2D/Physics2D.hpp>
#include <Nazara/Physics3D/Physics3D.hpp>
#include <Nazara/Platform/Platform.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <NazaraSDK/Algorithm.hpp>
#include <NazaraSDK/BaseSystem.hpp>
#include <NazaraSDK/Sdk.hpp>
#include <NazaraSDK/Components/ListenerComponent.hpp>
#include <NazaraSDK/Systems/ListenerSystem.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::Sdk
	* \brief NDK class that represents the software development kit, a set of tools made to ease the conception of application
	*/

	/*!
	* \brief Initializes the Sdk module
	* \return true if initialization is successful
	*
	* \remark Produces a NazaraNotice
	*/
	ClientSdk::ClientSdk(Config /*config*/) :
	ModuleBase("ClientSDK", this)
	{
		Nz::ErrorFlags errFlags(Nz::ErrorMode::ThrowException, true);

		// Client components
		InitializeComponent<ListenerComponent>("NdkList");

		// Systems

		// Client systems
		InitializeSystem<ListenerSystem>();
	}

	/*!
	* \brief Uninitializes the Sdk module
	*
	* \remark Produces a NazaraNotice
	*/
	ClientSdk::~ClientSdk() = default;

	ClientSdk* ClientSdk::s_instance = nullptr;
}
