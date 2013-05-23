// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSOBJECT_HPP
#define NAZARA_PHYSOBJECT_HPP

#include <Nazara/Prerequesites.hpp>
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

		float GetGravityFactor() const;
		float GetMass() const;
		NzVector3f GetMassCenter() const;
		NzVector3f GetPosition() const;
		NzQuaternionf GetRotation() const;
		NzVector3f GetVelocity() const;

		bool IsMoveable() const;

		void SetMass(float mass);
		void SetMassCenter(NzVector3f center);

	private:
		static void ForceAndTorqueCallback(const NewtonBody* body, float timeStep, int threadIndex);
		static void TransformCallback(const NewtonBody* body, const float* matrix, int threadIndex);

		NzMatrix4f m_matrix;
		NzVector3f m_forceAccumulator;
		NewtonBody* m_body;
		const NzBaseGeom* m_geom;
		NzPhysWorld* m_world;
		bool m_ownsGeom;
		float m_gravityFactor;
		float m_mass;
};

#endif // NAZARA_PHYSOBJECT_HPP
