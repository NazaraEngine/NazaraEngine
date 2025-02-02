// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	Boxf FromJolt(const JPH::AABox& aabb)
	{
		return Boxf(FromJolt(aabb.mMin), FromJolt(aabb.GetSize()));
	}

	inline Vector3f FromJolt(const JPH::Float3& f3)
	{
		return Vector3f(f3.x, f3.y, f3.z);
	}

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

	inline PhysContact3D FromJolt(const JPH::ContactManifold& contactManifold)
	{
		PhysContact3D contact;
		contact.baseOffset = FromJolt(contactManifold.mBaseOffset);
		contact.normal = FromJolt(contactManifold.mWorldSpaceNormal);
		contact.penetrationDepth = contactManifold.mPenetrationDepth;
		contact.subshapeID1 = contactManifold.mSubShapeID1.GetValue();
		contact.subshapeID2 = contactManifold.mSubShapeID2.GetValue();

		static_assert(decltype(contact.relativeContactPositions1)::FixedCapacity == decltype(contactManifold.mRelativeContactPointsOn1)::Capacity);

		contact.relativeContactPositions1.resize(contactManifold.mRelativeContactPointsOn1.size());
		for (JPH::uint i = 0; i < contactManifold.mRelativeContactPointsOn1.size(); ++i)
			contact.relativeContactPositions1[i] = FromJolt(contactManifold.mRelativeContactPointsOn1[i]);

		static_assert(decltype(contact.relativeContactPositions2)::FixedCapacity == decltype(contactManifold.mRelativeContactPointsOn2)::Capacity);

		contact.relativeContactPositions2.resize(contactManifold.mRelativeContactPointsOn2.size());
		for (JPH::uint i = 0; i < contactManifold.mRelativeContactPointsOn2.size(); ++i)
			contact.relativeContactPositions2[i] = FromJolt(contactManifold.mRelativeContactPointsOn2[i]);

		return contact;
	}

	inline PhysContactResponse3D FromJolt(const JPH::ContactSettings& contactSettings)
	{
		PhysContactResponse3D contactResponse;
		contactResponse.combinedFriction = contactSettings.mCombinedFriction;
		contactResponse.combinedRestitution = contactSettings.mCombinedRestitution;
		contactResponse.invMassScale1 = contactSettings.mInvMassScale1;
		contactResponse.invInertiaScale1 = contactSettings.mInvInertiaScale1;
		contactResponse.invMassScale2 = contactSettings.mInvMassScale2;
		contactResponse.invInertiaScale2 = contactSettings.mInvInertiaScale2;
		contactResponse.isSensor = contactSettings.mIsSensor;
		contactResponse.relativeAngularSurfaceVelocity = FromJolt(contactSettings.mRelativeAngularSurfaceVelocity);
		contactResponse.relativeLinearSurfaceVelocity = FromJolt(contactSettings.mRelativeLinearSurfaceVelocity);

		return contactResponse;
	}


	inline JPH::AABox ToJolt(const Boxf& aabb)
	{
		return JPH::AABox(ToJolt(aabb.GetMinimum()), ToJolt(aabb.GetMaximum()));
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

	inline JPH::ContactSettings ToJolt(const PhysContactResponse3D& contactResponse)
	{
		JPH::ContactSettings contactSettings;
		contactSettings.mCombinedFriction = contactResponse.combinedFriction;
		contactSettings.mCombinedRestitution = contactResponse.combinedRestitution;
		contactSettings.mInvMassScale1 = contactResponse.invMassScale1;
		contactSettings.mInvInertiaScale1 = contactResponse.invInertiaScale1;
		contactSettings.mInvMassScale2 = contactResponse.invMassScale2;
		contactSettings.mInvInertiaScale2 = contactResponse.invInertiaScale2;
		contactSettings.mIsSensor = contactResponse.isSensor;
		contactSettings.mRelativeAngularSurfaceVelocity = ToJolt(contactResponse.relativeAngularSurfaceVelocity);
		contactSettings.mRelativeLinearSurfaceVelocity = ToJolt(contactResponse.relativeLinearSurfaceVelocity);

		return contactSettings;
	}
}
