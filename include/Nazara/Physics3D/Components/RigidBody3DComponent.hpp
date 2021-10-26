// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RIGIDBODYCOMPONENT_HPP
#define NAZARA_RIGIDBODYCOMPONENT_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Physics3D/RigidBody3D.hpp>

namespace Nz
{
	class NAZARA_PHYSICS3D_API RigidBody3DComponent : public RigidBody3D
	{
		public:
			using RigidBody3D::RigidBody3D;
			RigidBody3DComponent(const RigidBody3DComponent&) = default;
			RigidBody3DComponent(RigidBody3DComponent&&) noexcept = default;
			~RigidBody3DComponent() = default;

			RigidBody3DComponent& operator=(const RigidBody3DComponent&) = default;
			RigidBody3DComponent& operator=(RigidBody3DComponent&&) noexcept = default;
	};
}

#include <Nazara/Physics3D/Components/RigidBody3DComponent.inl>

#endif
