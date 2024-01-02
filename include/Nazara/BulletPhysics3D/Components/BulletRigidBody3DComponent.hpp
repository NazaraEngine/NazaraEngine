// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - BulletPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BULLETPHYSICS3D_COMPONENTS_BULLETRIGIDBODY3DCOMPONENT_HPP
#define NAZARA_BULLETPHYSICS3D_COMPONENTS_BULLETRIGIDBODY3DCOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/BulletPhysics3D/BulletRigidBody3D.hpp>

namespace Nz
{
	class NAZARA_BULLETPHYSICS3D_API BulletRigidBody3DComponent : public BulletRigidBody3D
	{
		friend class BulletPhysics3DSystem;

		public:
			using BulletRigidBody3D::BulletRigidBody3D;
			BulletRigidBody3DComponent(const BulletRigidBody3DComponent&) = default;
			BulletRigidBody3DComponent(BulletRigidBody3DComponent&&) noexcept = default;
			~BulletRigidBody3DComponent() = default;

			BulletRigidBody3DComponent& operator=(const BulletRigidBody3DComponent&) = default;
			BulletRigidBody3DComponent& operator=(BulletRigidBody3DComponent&&) noexcept = default;
	};
}

#include <Nazara/BulletPhysics3D/Components/BulletRigidBody3DComponent.inl>

#endif // NAZARA_BULLETPHYSICS3D_COMPONENTS_BULLETRIGIDBODY3DCOMPONENT_HPP
