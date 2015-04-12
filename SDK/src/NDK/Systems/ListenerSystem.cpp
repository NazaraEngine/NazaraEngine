// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Systems/ListenerSystem.hpp>
#include <Nazara/Audio/Audio.hpp>
#include <NDK/Components/ListenerComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>

namespace Ndk
{
	ListenerSystem::ListenerSystem()
	{
		Requires<ListenerComponent, NodeComponent>();
	}

	void ListenerSystem::Update(float elapsedTime)
	{
		unsigned int activeListenerCount = 0;

		for (const Ndk::EntityHandle& entity : GetEntities())
		{
			// Le listener est-il actif ?
			const ListenerComponent& listener = entity->GetComponent<ListenerComponent>();
			if (!listener.IsActive())
				continue;

			const NodeComponent& node = entity->GetComponent<NodeComponent>();
			NzAudio::SetListenerPosition(node.GetPosition(nzCoordSys_Global));
			NzAudio::SetListenerRotation(node.GetRotation(nzCoordSys_Global));

			activeListenerCount++;
		}

		if (activeListenerCount > 1)
			NazaraWarning(NzString::Number(activeListenerCount) + " listeners were active in the same update loop");
	}

	SystemIndex ListenerSystem::systemIndex;
}
