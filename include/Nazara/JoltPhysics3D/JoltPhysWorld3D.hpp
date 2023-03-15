// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_JOLTPHYSICS3D_JOLTPHYSWORLD3D_HPP
#define NAZARA_JOLTPHYSICS3D_JOLTPHYSWORLD3D_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/JoltPhysics3D/Config.hpp>
#include <Nazara/Core/Time.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utils/FunctionRef.hpp>
#include <Nazara/Utils/MovablePtr.hpp>

namespace JPH
{
	class PhysicsSystem;
}

namespace Nz
{
	class JoltRigidBody3D;

	class NAZARA_JOLTPHYSICS3D_API JoltPhysWorld3D
	{
		public:
			struct RaycastHit;

			JoltPhysWorld3D();
			JoltPhysWorld3D(const JoltPhysWorld3D&) = delete;
			JoltPhysWorld3D(JoltPhysWorld3D&& ph) = delete;
			~JoltPhysWorld3D();

			Vector3f GetGravity() const;
			std::size_t GetMaxStepCount() const;
			JPH::PhysicsSystem* GetPhysicsSystem();
			Time GetStepSize() const;

			bool RaycastQueryFirst(const Vector3f& from, const Vector3f& to, RaycastHit* hitInfo = nullptr);

			void SetGravity(const Vector3f& gravity);
			void SetMaxStepCount(std::size_t maxStepCount);
			void SetStepSize(Time stepSize);

			void Step(Time timestep);

			JoltPhysWorld3D& operator=(const JoltPhysWorld3D&) = delete;
			JoltPhysWorld3D& operator=(JoltPhysWorld3D&&) = delete;

			struct RaycastHit
			{
				float fraction;
				JoltRigidBody3D* hitBody = nullptr;
				Vector3f hitPosition;
				Vector3f hitNormal;
			};

		private:
			struct JoltWorld;

			std::size_t m_maxStepCount;
			std::unique_ptr<JoltWorld> m_world;
			Vector3f m_gravity;
			Time m_stepSize;
			Time m_timestepAccumulator;
	};
}

#endif // NAZARA_JOLTPHYSICS3D_JOLTPHYSWORLD3D_HPP
