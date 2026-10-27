// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_PHYSBODY3D_HPP
#define NAZARA_PHYSICS3D_PHYSBODY3D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Physics3D/Export.hpp>
#include <Nazara/Physics3D/PhysObjectLayer3D.hpp>
#include <memory>
#include <utility>

namespace Nz
{
	class Collider3D;

	class NAZARA_PHYSICS3D_API PhysBody3D
	{
		public:
			PhysBody3D() = default;
			PhysBody3D(const PhysBody3D&) = delete;
			PhysBody3D(PhysBody3D&&) = delete;
			virtual ~PhysBody3D();

			virtual Vector3f GetAngularVelocity() const = 0;
			virtual UInt32 GetBodyIndex() const = 0;
			virtual const std::shared_ptr<Collider3D>& GetCollider() const = 0;
			virtual Vector3f GetLinearVelocity() const = 0;
			virtual PhysObjectLayer3D GetObjectLayer() const = 0;
			virtual Vector3f GetPosition() const = 0;
			virtual std::pair<Vector3f, Quaternionf> GetPositionAndRotation() const = 0;
			virtual Quaternionf GetRotation() const = 0;

			virtual bool IsTrigger() const = 0;

			PhysBody3D& operator=(const PhysBody3D&) = delete;
			PhysBody3D& operator=(PhysBody3D&&) = delete;
	};
}

#include <Nazara/Physics3D/PhysBody3D.inl>

#endif // NAZARA_PHYSICS3D_PHYSBODY3D_HPP
