// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Systems/ListenerSystem.hpp>
#include <Nazara/Audio/Audio.hpp>
#include <NDK/Components/ListenerComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/VelocityComponent.hpp>

namespace Ndk
{
	ListenerSystem::ListenerSystem()
	{
		Requires<ListenerComponent, NodeComponent>();
	}

	void ListenerSystem::OnUpdate(float elapsedTime)
	{
		NazaraUnused(elapsedTime);

		unsigned int activeListenerCount = 0;

		for (const Ndk::EntityHandle& entity : GetEntities())
		{
			// Le listener est-il actif ?
			const ListenerComponent& listener = entity->GetComponent<ListenerComponent>();
			if (!listener.IsActive())
				continue;

			// On récupère la position et la rotation pour les affecter au listener
			const NodeComponent& node = entity->GetComponent<NodeComponent>();
			Nz::Audio::SetListenerPosition(node.GetPosition(Nz::CoordSys_Global));
			Nz::Audio::SetListenerRotation(node.GetRotation(Nz::CoordSys_Global));

			// On vérifie la présence d'une donnée de vitesse, et on l'affecte
			// (La vitesse du listener Audio ne le fait pas se déplacer, mais affecte par exemple l'effet Doppler)
			if (entity->HasComponent<VelocityComponent>())
			{
				const VelocityComponent& velocity = entity->GetComponent<VelocityComponent>();
				Nz::Audio::SetListenerVelocity(velocity.linearVelocity);
			}

			activeListenerCount++;
		}

		if (activeListenerCount > 1)
			NazaraWarning(Nz::String::Number(activeListenerCount) + " listeners were active in the same update loop");
	}

	SystemIndex ListenerSystem::systemIndex;
}
