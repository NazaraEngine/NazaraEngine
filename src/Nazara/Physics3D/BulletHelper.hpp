// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_BULLETHELPER_HPP
#define NAZARA_PHYSICS3D_BULLETHELPER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Physics3D/Config.hpp>
#include <LinearMath/btQuaternion.h>
#include <LinearMath/btTransform.h>
#include <LinearMath/btVector3.h>

namespace Nz
{
	inline Quaternionf FromBullet(const btQuaternion& quat);
	inline Matrix4f FromBullet(const btTransform& transform);
	inline Vector3f FromBullet(const btVector3& vec);

	inline btTransform ToBullet(const Matrix4f& transformMatrix);
	inline btQuaternion ToBullet(const Quaternionf& rotation);
	inline btVector3 ToBullet(const Vector3f& vec);
}

#include <Nazara/Physics3D/BulletHelper.inl>

#endif // NAZARA_PHYSICS3D_BULLETHELPER_HPP
