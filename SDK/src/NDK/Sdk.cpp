// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequisites.hpp

#include <NDK/Sdk.hpp>
#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Lua/Lua.hpp>
#include <Nazara/Noise/Noise.hpp>
#include <Nazara/Physics2D/Physics2D.hpp>
#include <Nazara/Physics3D/Physics3D.hpp>
#include <Nazara/Platform/Platform.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <NDK/Algorithm.hpp>
#include <NDK/BaseSystem.hpp>
#include <NDK/Components/CollisionComponent2D.hpp>
#include <NDK/Components/CollisionComponent3D.hpp>
#include <NDK/Components/LifetimeComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/PhysicsComponent2D.hpp>
#include <NDK/Components/PhysicsComponent3D.hpp>
#include <NDK/Components/VelocityComponent.hpp>
#include <NDK/Components/ConstraintComponent2D.hpp>
#include <NDK/Systems/LifetimeSystem.hpp>
#include <NDK/Systems/PhysicsSystem2D.hpp>
#include <NDK/Systems/PhysicsSystem3D.hpp>
#include <NDK/Systems/VelocitySystem.hpp>

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
			Nz::Lua::Initialize();
			Nz::Noise::Initialize();
			Nz::Physics2D::Initialize();
			Nz::Physics3D::Initialize();
			Nz::Utility::Initialize();

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

			// Systems

			BaseSystem::Initialize();

			// Shared systems
			InitializeSystem<LifetimeSystem>();
			InitializeSystem<PhysicsSystem2D>();
			InitializeSystem<PhysicsSystem3D>();
			InitializeSystem<VelocitySystem>();

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

		// Shared modules
		Nz::Lua::Uninitialize();
		Nz::Noise::Uninitialize();
		Nz::Physics2D::Uninitialize();
		Nz::Physics3D::Uninitialize();
		Nz::Utility::Uninitialize();

		NazaraNotice("Uninitialized: SDK");
	}

	unsigned int Sdk::s_referenceCounter = 0;
}
