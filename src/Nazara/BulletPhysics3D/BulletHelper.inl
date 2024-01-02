// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - BulletPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/BulletPhysics3D/Debug.hpp>

namespace Nz
{
	Quaternionf FromBullet(const btQuaternion& quat)
	{
		return Quaternionf(quat.w(), quat.x(), quat.y(), quat.z());
	}

	Matrix4f FromBullet(const btTransform& transform)
	{
		const btMatrix3x3& basisMatrix = transform.getBasis();
		const btVector3& origin = transform.getOrigin();

		const btVector3& row1 = basisMatrix.getRow(0);
		const btVector3& row2 = basisMatrix.getRow(1);
		const btVector3& row3 = basisMatrix.getRow(2);

		return Matrix4f(
			row1.x(), row1.y(), row1.z(), 0.f,
			row2.x(), row2.y(), row2.z(), 0.f,
			row3.x(), row3.y(), row3.z(), 0.f,
			origin.x(), origin.y(), origin.z(), 1.f);
	}

	inline Vector3f FromBullet(const btVector3& vec)
	{
		return Vector3f(vec.x(), vec.y(), vec.z());
	}

	btTransform ToBullet(const Matrix4f& transformMatrix)
	{
		btTransform transform;
		transform.setBasis(btMatrix3x3(
			transformMatrix.m11, transformMatrix.m12, transformMatrix.m13,
			transformMatrix.m21, transformMatrix.m22, transformMatrix.m23,
			transformMatrix.m31, transformMatrix.m32, transformMatrix.m33
		));

		transform.setOrigin(btVector3(transformMatrix.m41, transformMatrix.m42, transformMatrix.m43));

		return transform;
	}

	btQuaternion ToBullet(const Quaternionf& rotation)
	{
		return btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w);
	}

	inline btVector3 ToBullet(const Vector3f& vec)
	{
		return btVector3(vec.x, vec.y, vec.z);
	}
}

#include <Nazara/BulletPhysics3D/DebugOff.hpp>
