// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Systems/ListenerSystem.hpp>
#include <Nazara/Audio/Audio.hpp>
#include <NDK/Components/ListenerComponent.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/VelocityComponent.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::ListenerSystem
	* \brief NDK class that represents the audio system
	*
	* \remark This system is enabled if the entity owns the trait: ListenerComponent and NodeComponent
	*/

	/*!
	* \brief Constructs an ListenerSystem object by default
	*/

	ListenerSystem::ListenerSystem()
	{
		Requires<ListenerComponent, NodeComponent>();
		SetUpdateOrder(100); //< Update last, after every movement is done
	}

	/*!
	* \brief Operation to perform when system is updated
	*
	* \param elapsedTime Delta time used for the update
	*/

	void ListenerSystem::OnUpdate(float /*elapsedTime*/)
	{
		std::size_t activeListenerCount = 0;

		for (const Ndk::EntityHandle& entity : GetEntities())
		{
			// Is the listener actif ?
			const ListenerComponent& listener = entity->GetComponent<ListenerComponent>();
			if (!listener.IsActive())
				continue;

			// We get the position and the rotation to affect these to the listener
			const NodeComponent& node = entity->GetComponent<NodeComponent>();
			Nz::Audio::SetListenerPosition(node.GetPosition(Nz::CoordSys_Global));
			Nz::Audio::SetListenerRotation(node.GetRotation(Nz::CoordSys_Global));

			// We verify the presence of a component of velocity
			// (The listener'speed does not move it, but disturbs the sound like Doppler effect)
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
