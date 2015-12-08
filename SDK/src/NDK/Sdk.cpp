// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Sdk.hpp>
#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Graphics/Graphics.hpp>
#include <Nazara/Lua/Lua.hpp>
#include <Nazara/Noise/Noise.hpp>
#include <Nazara/Physics/Physics.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <NDK/Algorithm.hpp>
#include <NDK/BaseSystem.hpp>
#include <NDK/Components/CameraComponent.hpp>
#include <NDK/Components/CollisionComponent.hpp>
#include <NDK/Components/LightComponent.hpp>
#include <NDK/Components/ListenerComponent.hpp>
#include <NDK/Components/GraphicsComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/PhysicsComponent.hpp>
#include <NDK/Components/VelocityComponent.hpp>
#include <NDK/Systems/ListenerSystem.hpp>
#include <NDK/Systems/PhysicsSystem.hpp>
#include <NDK/Systems/RenderSystem.hpp>
#include <NDK/Systems/VelocitySystem.hpp>

namespace Ndk
{
	bool Sdk::Initialize()
	{
		if (s_referenceCounter++ > 0)
			return true; // Déjà initialisé

		try
		{
			Nz::ErrorFlags errFlags(Nz::ErrorFlag_ThrowException, true);

			// Initialisation du moteur

			// Modules clients
			Nz::Audio::Initialize();
			Nz::Graphics::Initialize();

			// Modules serveurs
			Nz::Lua::Initialize();
			Nz::Noise::Initialize();
			Nz::Physics::Initialize();
			Nz::Utility::Initialize();

			// Initialisation du SDK

			// Initialisation des composants et systèmes
			BaseComponent::Initialize();
			BaseSystem::Initialize();

			// Composants
			InitializeComponent<CameraComponent>("NdkCam");
			InitializeComponent<CollisionComponent>("NdkColli");
			InitializeComponent<LightComponent>("NdkLight");
			InitializeComponent<ListenerComponent>("NdkList");
			InitializeComponent<GraphicsComponent>("NdkGfx");
			InitializeComponent<NodeComponent>("NdkNode");
			InitializeComponent<PhysicsComponent>("NdkPhys");
			InitializeComponent<VelocityComponent>("NdkVeloc");

			// Systèmes
			InitializeSystem<ListenerSystem>();
			InitializeSystem<PhysicsSystem>();
			InitializeSystem<RenderSystem>();
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

	void Sdk::Uninitialize()
	{
		if (s_referenceCounter != 1)
		{
			// Le module est soit encore utilisé, soit pas initialisé
			if (s_referenceCounter > 1)
				s_referenceCounter--;

			return;
		}

		// Libération du SDK
		s_referenceCounter = 0;

		// Libération du moteur

		// Modules clients
		Nz::Audio::Uninitialize();
		Nz::Graphics::Uninitialize();

		// Modules serveurs
		Nz::Lua::Uninitialize();
		Nz::Noise::Uninitialize();
		Nz::Physics::Uninitialize();
		Nz::Utility::Uninitialize();

		NazaraNotice("Uninitialized: SDK");
	}

	unsigned int Sdk::s_referenceCounter = 0;
}
