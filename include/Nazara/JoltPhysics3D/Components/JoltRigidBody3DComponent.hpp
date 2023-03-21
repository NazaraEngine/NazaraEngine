// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_JOLTPHYSICS3D_COMPONENTS_JOLTRIGIDBODY3DCOMPONENT_HPP
#define NAZARA_JOLTPHYSICS3D_COMPONENTS_JOLTRIGIDBODY3DCOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/JoltPhysics3D/JoltRigidBody3D.hpp>

namespace Nz
{
	class NAZARA_JOLTPHYSICS3D_API JoltRigidBody3DComponent : public JoltRigidBody3D
	{
		friend class JoltPhysics3DSystem;

		public:
			using JoltRigidBody3D::JoltRigidBody3D;
			JoltRigidBody3DComponent(const JoltRigidBody3DComponent&) = default;
			JoltRigidBody3DComponent(JoltRigidBody3DComponent&&) noexcept = default;
			~JoltRigidBody3DComponent() = default;

			JoltRigidBody3DComponent& operator=(const JoltRigidBody3DComponent&) = default;
			JoltRigidBody3DComponent& operator=(JoltRigidBody3DComponent&&) noexcept = default;
	};
}

#include <Nazara/JoltPhysics3D/Components/JoltRigidBody3DComponent.inl>

#endif // NAZARA_JOLTPHYSICS3D_COMPONENTS_JOLTRIGIDBODY3DCOMPONENT_HPP
