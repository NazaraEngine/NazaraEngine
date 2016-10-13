// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSOBJECT_HPP
#define NAZARA_PHYSOBJECT_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Physics3D/Config.hpp>
#include <Nazara/Physics3D/Geom.hpp>

class NewtonBody;

namespace Nz
{
	class PhysWorld;

	class NAZARA_PHYSICS3D_API PhysObject
	{
		public:
			PhysObject(PhysWorld* world, const Matrix4f& mat = Matrix4f::Identity());
			PhysObject(PhysWorld* world, PhysGeomRef geom, const Matrix4f& mat = Matrix4f::Identity());
			PhysObject(const PhysObject& object);
			PhysObject(PhysObject&& object);
			~PhysObject();

			void AddForce(const Vector3f& force, CoordSys coordSys = CoordSys_Global);
			void AddForce(const Vector3f& force, const Vector3f& point, CoordSys coordSys = CoordSys_Global);
			void AddTorque(const Vector3f& torque, CoordSys coordSys = CoordSys_Global);

			void EnableAutoSleep(bool autoSleep);

			Boxf GetAABB() const;
			Vector3f GetAngularVelocity() const;
			const PhysGeomRef& GetGeom() const;
			float GetGravityFactor() const;
			NewtonBody* GetHandle() const;
			float GetMass() const;
			Vector3f GetMassCenter(CoordSys coordSys = CoordSys_Local) const;
			const Matrix4f& GetMatrix() const;
			Vector3f GetPosition() const;
			Quaternionf GetRotation() const;
			Vector3f GetVelocity() const;

			bool IsAutoSleepEnabled() const;
			bool IsMoveable() const;
			bool IsSleeping() const;

			void SetAngularVelocity(const Vector3f& angularVelocity);
			void SetGeom(PhysGeomRef geom);
			void SetGravityFactor(float gravityFactor);
			void SetMass(float mass);
			void SetMassCenter(const Vector3f& center);
			void SetPosition(const Vector3f& position);
			void SetRotation(const Quaternionf& rotation);
			void SetVelocity(const Vector3f& velocity);

			PhysObject& operator=(const PhysObject& object);
			PhysObject& operator=(PhysObject&& object);

		private:
			void UpdateBody();
			static void ForceAndTorqueCallback(const NewtonBody* body, float timeStep, int threadIndex);
			static void TransformCallback(const NewtonBody* body, const float* matrix, int threadIndex);

			Matrix4f m_matrix;
			PhysGeomRef m_geom;
			Vector3f m_forceAccumulator;
			Vector3f m_torqueAccumulator;
			NewtonBody* m_body;
			PhysWorld* m_world;
			float m_gravityFactor;
			float m_mass;
	};
}

#endif // NAZARA_PHYSOBJECT_HPP
