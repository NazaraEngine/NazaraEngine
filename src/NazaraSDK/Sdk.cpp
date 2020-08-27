// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NazaraSDK/Sdk.hpp>
#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Physics2D/Physics2D.hpp>
#include <Nazara/Physics3D/Physics3D.hpp>
#include <Nazara/Platform/Platform.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <NazaraSDK/Algorithm.hpp>
#include <NazaraSDK/BaseSystem.hpp>
#include <NazaraSDK/Components/CollisionComponent2D.hpp>
#include <NazaraSDK/Components/CollisionComponent3D.hpp>
#include <NazaraSDK/Components/LifetimeComponent.hpp>
#include <NazaraSDK/Components/NodeComponent.hpp>
#include <NazaraSDK/Components/PhysicsComponent2D.hpp>
#include <NazaraSDK/Components/PhysicsComponent3D.hpp>
#include <NazaraSDK/Components/VelocityComponent.hpp>
#include <NazaraSDK/Components/ConstraintComponent2D.hpp>
#include <NazaraSDK/Systems/LifetimeSystem.hpp>
#include <NazaraSDK/Systems/PhysicsSystem2D.hpp>
#include <NazaraSDK/Systems/PhysicsSystem3D.hpp>
#include <NazaraSDK/Systems/VelocitySystem.hpp>

#ifndef NDK_SERVER
#include <NazaraSDK/Components/ListenerComponent.hpp>
#include <NazaraSDK/Systems/ListenerSystem.hpp>
#endif

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

	bool Sdk::Initialize()
	{
		if (s_referenceCounter++ > 0)
			return true; // Already initialized

		try
		{
			Nz::ErrorFlags errFlags(Nz::ErrorFlag_ThrowException, true);

			// Initialize the engine first

			// Shared modules
			Nz::Physics2D::Initialize();
			Nz::Physics3D::Initialize();
			Nz::Utility::Initialize();

			#ifndef NDK_SERVER
			// Client modules
			Nz::Audio::Initialize();
			#endif

			// SDK Initialization

			// Components
			BaseComponent::Initialize();

			// Shared components
			InitializeComponent<CollisionComponent2D>("NdkColl2");
			InitializeComponent<CollisionComponent3D>("NdkColl3");
			InitializeComponent<LifetimeComponent>("NdkLiftm");
			InitializeComponent<NodeComponent>("NdkNode");
			InitializeComponent<PhysicsComponent2D>("NdkPhys2");
			InitializeComponent<PhysicsComponent3D>("NdkPhys3");
			InitializeComponent<VelocityComponent>("NdkVeloc");
			InitializeComponent<VelocityComponent>("NdkCons2");

			#ifndef NDK_SERVER
			// Client components
			InitializeComponent<ListenerComponent>("NdkList");
			#endif

			// Systems

			BaseSystem::Initialize();

			// Shared systems
			InitializeSystem<LifetimeSystem>();
			InitializeSystem<PhysicsSystem2D>();
			InitializeSystem<PhysicsSystem3D>();
			InitializeSystem<VelocitySystem>();

			#ifndef NDK_SERVER
			// Client systems
			InitializeSystem<ListenerSystem>();
			#endif

			NazaraNotice("Initialized: SDK");
			return true;
		}
		catch (const std::exception& e)
		{
			NazaraError("Failed to initialize NDK: " + Nz::String(e.what()));
			return false;
		}
	}

	/*!
	* \brief Uninitializes the Sdk module
	*
	* \remark Produces a NazaraNotice
	*/

	void Sdk::Uninitialize()
	{
		if (s_referenceCounter != 1)
		{
			// Either the module is not initialized, either it was initialized multiple times
			if (s_referenceCounter > 1)
				s_referenceCounter--;

			return;
		}

		// Uninitialize the SDK
		s_referenceCounter = 0;

		// Components
		BaseComponent::Uninitialize();

		// Systems
		BaseSystem::Uninitialize();

		// Uninitialize the engine

		#ifndef NDK_SERVER
		// Client modules
		Nz::Audio::Uninitialize();
		#endif

		// Shared modules
		Nz::Physics2D::Uninitialize();
		Nz::Physics3D::Uninitialize();
		Nz::Utility::Uninitialize();

		#ifndef NDK_SERVER
		#endif

		NazaraNotice("Uninitialized: SDK");
	}

	unsigned int Sdk::s_referenceCounter = 0;
}
