// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSICS2D_COMPONENTS_RIGIDBODY2DCOMPONENT_HPP
#define NAZARA_PHYSICS2D_COMPONENTS_RIGIDBODY2DCOMPONENT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Physics2D/RigidBody2D.hpp>

namespace Nz
{
	class NAZARA_PHYSICS2D_API RigidBody2DComponent : public RigidBody2D
	{
		friend class Physics2DSystem;

		public:
			using RigidBody2D::RigidBody2D;
			RigidBody2DComponent(const RigidBody2DComponent&) = default;
			RigidBody2DComponent(RigidBody2DComponent&&) noexcept = default;
			~RigidBody2DComponent() = default;

			RigidBody2DComponent& operator=(const RigidBody2DComponent&) = default;
			RigidBody2DComponent& operator=(RigidBody2DComponent&&) noexcept = default;
	};
}

#include <Nazara/Physics2D/Components/RigidBody2DComponent.inl>

#endif // NAZARA_PHYSICS2D_COMPONENTS_RIGIDBODY2DCOMPONENT_HPP
