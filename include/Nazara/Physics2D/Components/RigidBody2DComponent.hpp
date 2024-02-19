// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PHYSICS2D_COMPONENTS_RIGIDBODY2DCOMPONENT_HPP
#define NAZARA_PHYSICS2D_COMPONENTS_RIGIDBODY2DCOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Physics2D/RigidBody2D.hpp>
#include <variant>

namespace Nz
{
	class NAZARA_PHYSICS2D_API RigidBody2DComponent : public RigidBody2D
	{
		friend class Physics2DSystem;

		public:
			inline RigidBody2DComponent(const RigidBody2D::DynamicSettings& settings);
			inline RigidBody2DComponent(const RigidBody2D::StaticSettings& settings);
			RigidBody2DComponent(const RigidBody2DComponent&) = delete;
			RigidBody2DComponent(RigidBody2DComponent&&) noexcept = default;
			~RigidBody2DComponent() = default;

			RigidBody2DComponent& operator=(const RigidBody2DComponent&) = delete;
			RigidBody2DComponent& operator=(RigidBody2DComponent&&) noexcept = default;

		private:
			inline void Construct(PhysWorld2D& world);

			using Setting = std::variant<RigidBody2D::DynamicSettings, RigidBody2D::StaticSettings>;
			std::unique_ptr<Setting> m_settings;
	};
}

#include <Nazara/Physics2D/Components/RigidBody2DComponent.inl>

#endif // NAZARA_PHYSICS2D_COMPONENTS_RIGIDBODY2DCOMPONENT_HPP
