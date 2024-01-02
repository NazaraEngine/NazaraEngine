// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/JoltPhysics3D/Debug.hpp>

namespace Nz
{
	Quaternionf FromJolt(const JPH::Quat& quat)
	{
		return Quaternionf(quat.GetW(), quat.GetX(), quat.GetY(), quat.GetZ());
	}

	Matrix4f FromJolt(const JPH::Mat44& transform)
	{
		const JPH::Vec4& row1 = transform.GetColumn4(0);
		const JPH::Vec4& row2 = transform.GetColumn4(1);
		const JPH::Vec4& row3 = transform.GetColumn4(2);
		const JPH::Vec4& row4 = transform.GetColumn4(3);

		return Matrix4f(
			row1.GetX(), row1.GetY(), row1.GetZ(), row1.GetW(),
			row2.GetX(), row2.GetY(), row2.GetZ(), row2.GetW(),
			row3.GetX(), row3.GetY(), row3.GetZ(), row3.GetW(),
			row4.GetX(), row4.GetY(), row4.GetZ(), row4.GetW());
	}

	inline Vector3f FromJolt(const JPH::Vec3& vec)
	{
		return Vector3f(vec.GetX(), vec.GetY(), vec.GetZ());
	}

	inline JPH::Mat44 ToJolt(const Matrix4f& transformMatrix)
	{
		JPH::Mat44 transform;
		transform.SetColumn4(0, JPH::Vec4{ transformMatrix.m11, transformMatrix.m12, transformMatrix.m13, transformMatrix.m14 });
		transform.SetColumn4(1, JPH::Vec4{ transformMatrix.m21, transformMatrix.m22, transformMatrix.m23, transformMatrix.m24 });
		transform.SetColumn4(2, JPH::Vec4{ transformMatrix.m31, transformMatrix.m32, transformMatrix.m33, transformMatrix.m34 });
		transform.SetColumn4(3, JPH::Vec4{ transformMatrix.m41, transformMatrix.m42, transformMatrix.m43, transformMatrix.m44 });

		return transform;
	}

	inline JPH::Quat ToJolt(const Quaternionf& quaternion)
	{
		return JPH::Quat(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	}

	inline JPH::Vec3 ToJolt(const Vector3f& vec)
	{
		return JPH::Vec3(vec.x, vec.y, vec.z);
	}

	inline JPH::Vec4 ToJolt(const Vector4f& vec)
	{
		return JPH::Vec4(vec.x, vec.y, vec.z, vec.w);
	}
}

#include <Nazara/JoltPhysics3D/DebugOff.hpp>
