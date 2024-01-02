// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - BulletPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BULLETPHYSICS3D_BULLETHELPER_HPP
#define NAZARA_BULLETPHYSICS3D_BULLETHELPER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/BulletPhysics3D/Config.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
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

#include <Nazara/BulletPhysics3D/BulletHelper.inl>

#endif // NAZARA_BULLETPHYSICS3D_BULLETHELPER_HPP
