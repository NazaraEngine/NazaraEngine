// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NazaraSDK/Sdk.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
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

	Sdk::Sdk(Config /*config*/) :
	ModuleBase("SDK", this)
	{
		Nz::ErrorFlags errFlags(Nz::ErrorFlag_ThrowException, true);

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
	}

	Sdk::~Sdk()
	{
		// Components
		BaseComponent::Uninitialize();

		// Systems
		BaseSystem::Uninitialize();
	}

	Sdk* Sdk::s_instance;
}
