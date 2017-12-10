// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSWORLD_HPP
#define NAZARA_PHYSWORLD_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Physics3D/Config.hpp>
#include <unordered_map>

class NewtonJoint;
class NewtonWorld;

namespace Nz
{
	class RigidBody3D;

	class NAZARA_PHYSICS3D_API PhysWorld3D
	{
		public:
			using CollisionCallback = std::function<bool(const RigidBody3D& firstBody, const RigidBody3D& secondBody)>;

			PhysWorld3D();
			PhysWorld3D(const PhysWorld3D&) = delete;
			PhysWorld3D(PhysWorld3D&&) = delete; ///TODO
			~PhysWorld3D();

			int CreateMaterial(Nz::String name = Nz::String());

			Vector3f GetGravity() const;
			NewtonWorld* GetHandle() const;
			int GetMaterial(const Nz::String& name);
			float GetStepSize() const;

			void SetGravity(const Vector3f& gravity);
			void SetSolverModel(unsigned int model);
			void SetStepSize(float stepSize);

			void SetMaterialCollisionCallback(int firstMaterial, int secondMaterial, CollisionCallback callback);

			void Step(float timestep);

			PhysWorld3D& operator=(const PhysWorld3D&) = delete;
			PhysWorld3D& operator=(PhysWorld3D&&) = delete; ///TODO

		private:
			struct Callback
			{
				CollisionCallback collisionCallback;
			};

			static void ProcessContact(const NewtonJoint* const contact, float timestep, int threadIndex);

			std::unordered_map<Nz::UInt64, std::unique_ptr<Callback>> m_callbacks;
			std::unordered_map<Nz::String, int> m_materialIds;
			Vector3f m_gravity;
			NewtonWorld* m_world;
			float m_stepSize;
			float m_timestepAccumulator;
	};
}

#endif // NAZARA_PHYSWORLD_HPP
