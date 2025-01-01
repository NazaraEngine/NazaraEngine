// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_JOLTHELPER_HPP
#define NAZARA_PHYSICS3D_JOLTHELPER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Geometry/AABox.h>

namespace Nz
{
	inline Boxf FromJolt(const JPH::AABox& aabb);
	inline Vector3f FromJolt(const JPH::Float3& f3);
	inline Quaternionf FromJolt(const JPH::Quat& quat);
	inline Matrix4f FromJolt(const JPH::Mat44& matrix);
	inline Vector3f FromJolt(const JPH::Vec3& vec);

	inline JPH::AABox ToJolt(const Boxf& aabb);
	inline JPH::Mat44 ToJolt(const Matrix4f& transformMatrix);
	inline JPH::Quat ToJolt(const Quaternionf& quaternion);
	inline JPH::Vec3 ToJolt(const Vector3f& vec);
	inline JPH::Vec4 ToJolt(const Vector4f& vec);
}

#include <Nazara/Physics3D/JoltHelper.inl>

#endif // NAZARA_PHYSICS3D_JOLTHELPER_HPP
