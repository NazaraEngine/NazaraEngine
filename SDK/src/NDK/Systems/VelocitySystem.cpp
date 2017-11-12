// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Development Kit"
// For conditions of distribution and use, see copyright notice in Prerequesites.hpp

#include <NDK/Systems/VelocitySystem.hpp>
#include <NDK/Components/NodeComponent.hpp>
#include <NDK/Components/PhysicsComponent2D.hpp>
#include <NDK/Components/PhysicsComponent3D.hpp>
#include <NDK/Components/VelocityComponent.hpp>

namespace Ndk
{
	/*!
	* \ingroup NDK
	* \class Ndk::VelocitySystem
	* \brief NDK class that represents the velocity system
	*
	* \remark This system is enabled if the entity owns the traits NodeComponent and VelocityComponent
	* but it's disabled with the traits: PhysicsComponent2D, PhysicsComponent3D
	*/

	/*!
	* \brief Constructs an VelocitySystem object by default
	*/

	VelocitySystem::VelocitySystem()
	{
		Excludes<PhysicsComponent2D, PhysicsComponent3D>();
		Requires<NodeComponent, VelocityComponent>();
		SetUpdateOrder(10); //< Since some systems may want to stop us
	}

	/*!
	* \brief Function used to smooth the movements
	*
	* I didn't write this function myself
	* I used the one of the program "Floaty Camera Example" and adapted it to C++
	* Found here: http://nccastaff.bournemouth.ac.uk/jmacey/RobTheBloke/www/opengl_programming.html#4
	* All credits goes to the author (Who enable me to improve demos, even the engine)
	*
	* \param currentPos Current position of the node
	* \param targetPos Node position targeted
	* \param frametime Current delta time of the frame
	* \param springStrength Increases the further away final position is from the target
	*/

	Nz::Vector3f VelocitySystem::DampedString(const Nz::Vector3f& currentPos, const Nz::Vector3f& targetPos, float frametime, float springStrength) const
	{
		// calculate the displacement between the target and the current position
		Nz::Vector3f displacement = targetPos - currentPos;
		
		// whats the distance between them?
		float displacementLength = displacement.GetLength();
		
		// Stops small position fluctuations (integration errors probably - since only using euler)
		if (Nz::NumberEquals(displacementLength, 0.f))
			return currentPos;
		
		float invDisplacementLength = 1.f / displacementLength;
		
		const float dampConstant = 0.000065f; // Something v.small to offset 1/ displacement length
		
		// the strength of the spring increases the further away the camera is from the target.
		float springMagitude = springStrength*displacementLength + dampConstant*invDisplacementLength;
		
		// Normalise the displacement and scale by the spring magnitude
		// and the amount of time passed
		float scalar = std::min(invDisplacementLength * springMagitude * frametime, 1.f);
		displacement *= scalar;
		
		// move the camera a bit towards the target
		return currentPos + displacement;
	}

	/*!
	* \brief Operation to perform when system is updated
	*
	* \param elapsedTime Delta time used for the update
	*/

	void VelocitySystem::OnUpdate(float elapsedTime)
	{
		for (const Ndk::EntityHandle& entity : GetEntities())
		{
			NodeComponent& node = entity->GetComponent<NodeComponent>();
			const VelocityComponent& velocity = entity->GetComponent<VelocityComponent>();

			const Nz::Vector3f initialPos = node.GetPosition();
			Nz::Vector3f targetPos = initialPos + (velocity.linearVelocity * elapsedTime);

			node.SetPosition((velocity.damped ? DampedString(initialPos, targetPos, elapsedTime) : targetPos), Nz::CoordSys_Global);
		}
	}

	SystemIndex VelocitySystem::systemIndex;
}
