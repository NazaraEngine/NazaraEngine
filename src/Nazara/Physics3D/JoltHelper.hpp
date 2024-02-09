// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_JOLTHELPER_HPP
#define NAZARA_PHYSICS3D_JOLTHELPER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Jolt/Jolt.h>

namespace Nz
{
	inline Quaternionf FromJolt(const JPH::Quat& quat);
	inline Matrix4f FromJolt(const JPH::Mat44& matrix);
	inline Vector3f FromJolt(const JPH::Vec3& vec);

	inline JPH::Mat44 ToJolt(const Matrix4f& transformMatrix);
	inline JPH::Quat ToJolt(const Quaternionf& quaternion);
	inline JPH::Vec3 ToJolt(const Vector3f& vec);
	inline JPH::Vec4 ToJolt(const Vector4f& vec);
}

#include <Nazara/Physics3D/JoltHelper.inl>

#endif // NAZARA_PHYSICS3D_JOLTHELPER_HPP
