// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSOBJECT_HPP
#define NAZARA_PHYSOBJECT_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Enums.hpp>
#include <Nazara/Core/NonCopyable.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>

class NzBaseGeom;
class NzPhysWorld;
struct NewtonBody;

class NAZARA_API NzPhysObject : NzNonCopyable
{
	public:
		NzPhysObject(NzPhysWorld* world, const NzMatrix4f& mat = NzMatrix4f::Identity());
		NzPhysObject(NzPhysWorld* world, const NzBaseGeom* geom, const NzMatrix4f& mat = NzMatrix4f::Identity());
		~NzPhysObject();

		void AddForce(const NzVector3f& force, nzCoordSys coordSys = nzCoordSys_Global);
		void AddForce(const NzVector3f& force, const NzVector3f& point, nzCoordSys coordSys = nzCoordSys_Global);
		void AddTorque(const NzVector3f& torque, nzCoordSys coordSys = nzCoordSys_Global);

		void EnableAutoSleep(bool autoSleep);

		NzVector3f GetAngularVelocity() const;
		float GetGravityFactor() const;
		NewtonBody* GetHandle() const;
		float GetMass() const;
		NzVector3f GetMassCenter(nzCoordSys coordSys = nzCoordSys_Local) const;
		const NzMatrix4f& GetMatrix() const;
		NzVector3f GetPosition() const;
		NzQuaternionf GetRotation() const;
		NzVector3f GetVelocity() const;

		bool IsAutoSleepEnabled() const;
		bool IsMoveable() const;
		bool IsSleeping() const;

		void SetGravityFactor(float gravityFactor);
		void SetMass(float mass);
		void SetMassCenter(const NzVector3f& center);
		void SetPosition(const NzVector3f& position);
		void SetRotation(const NzQuaternionf& rotation);

	private:
		void UpdateBody();
		static void ForceAndTorqueCallback(const NewtonBody* body, float timeStep, int threadIndex);
		static void TransformCallback(const NewtonBody* body, const float* matrix, int threadIndex);

		NzMatrix4f m_matrix;
		NzVector3f m_forceAccumulator;
		NzVector3f m_torqueAccumulator;
		NewtonBody* m_body;
		const NzBaseGeom* m_geom;
		NzPhysWorld* m_world;
		bool m_ownsGeom;
		float m_gravityFactor;
		float m_mass;
};

#endif // NAZARA_PHYSOBJECT_HPP
