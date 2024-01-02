// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - ChipmunkPhysics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CHIPMUNKPHYSICS2D_COMPONENTS_CHIPMUNKRIGIDBODY2DCOMPONENT_HPP
#define NAZARA_CHIPMUNKPHYSICS2D_COMPONENTS_CHIPMUNKRIGIDBODY2DCOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/ChipmunkPhysics2D/ChipmunkRigidBody2D.hpp>
#include <variant>

namespace Nz
{
	class NAZARA_CHIPMUNKPHYSICS2D_API ChipmunkRigidBody2DComponent : public ChipmunkRigidBody2D
	{
		friend class ChipmunkPhysics2DSystem;

		public:
			inline ChipmunkRigidBody2DComponent(const ChipmunkRigidBody2D::DynamicSettings& settings);
			inline ChipmunkRigidBody2DComponent(const ChipmunkRigidBody2D::StaticSettings& settings);
			ChipmunkRigidBody2DComponent(const ChipmunkRigidBody2DComponent&) = default;
			ChipmunkRigidBody2DComponent(ChipmunkRigidBody2DComponent&&) noexcept = default;
			~ChipmunkRigidBody2DComponent() = default;

			ChipmunkRigidBody2DComponent& operator=(const ChipmunkRigidBody2DComponent&) = default;
			ChipmunkRigidBody2DComponent& operator=(ChipmunkRigidBody2DComponent&&) noexcept = default;

		private:
			inline void Construct(ChipmunkPhysWorld2D& world);

			using Setting = std::variant<ChipmunkRigidBody2D::DynamicSettings, ChipmunkRigidBody2D::StaticSettings>;
			std::unique_ptr<Setting> m_settings;
	};
}

#include <Nazara/ChipmunkPhysics2D/Components/ChipmunkRigidBody2DComponent.inl>

#endif // NAZARA_CHIPMUNKPHYSICS2D_COMPONENTS_CHIPMUNKRIGIDBODY2DCOMPONENT_HPP
