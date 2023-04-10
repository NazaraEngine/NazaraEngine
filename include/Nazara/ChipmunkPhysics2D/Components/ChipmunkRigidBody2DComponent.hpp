// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - ChipmunkPhysics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CHIPMUNKPHYSICS2D_COMPONENTS_CHIPMUNKRIGIDBODY2DCOMPONENT_HPP
#define NAZARA_CHIPMUNKPHYSICS2D_COMPONENTS_CHIPMUNKRIGIDBODY2DCOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/ChipmunkPhysics2D/ChipmunkRigidBody2D.hpp>

namespace Nz
{
	class NAZARA_CHIPMUNKPHYSICS2D_API ChipmunkRigidBody2DComponent : public ChipmunkRigidBody2D
	{
		friend class ChipmunkPhysics2DSystem;

		public:
			using ChipmunkRigidBody2D::ChipmunkRigidBody2D;
			ChipmunkRigidBody2DComponent(const ChipmunkRigidBody2DComponent&) = default;
			ChipmunkRigidBody2DComponent(ChipmunkRigidBody2DComponent&&) noexcept = default;
			~ChipmunkRigidBody2DComponent() = default;

			ChipmunkRigidBody2DComponent& operator=(const ChipmunkRigidBody2DComponent&) = default;
			ChipmunkRigidBody2DComponent& operator=(ChipmunkRigidBody2DComponent&&) noexcept = default;
	};
}

#include <Nazara/ChipmunkPhysics2D/Components/ChipmunkRigidBody2DComponent.inl>

#endif // NAZARA_CHIPMUNKPHYSICS2D_COMPONENTS_CHIPMUNKRIGIDBODY2DCOMPONENT_HPP
