// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_COMPONENTS_RIGIDBODY3DCOMPONENT_HPP
#define NAZARA_PHYSICS3D_COMPONENTS_RIGIDBODY3DCOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Physics3D/RigidBody3D.hpp>
#include <variant>

namespace Nz
{
	class NAZARA_PHYSICS3D_API RigidBody3DComponent : public RigidBody3D
	{
		friend class Physics3DSystem;

		public:
			inline RigidBody3DComponent(const RigidBody3D::DynamicSettings& settings);
			inline RigidBody3DComponent(const RigidBody3D::StaticSettings& settings);
			RigidBody3DComponent(const RigidBody3DComponent&) = default;
			RigidBody3DComponent(RigidBody3DComponent&&) noexcept = default;
			~RigidBody3DComponent() = default;

			RigidBody3DComponent& operator=(const RigidBody3DComponent&) = default;
			RigidBody3DComponent& operator=(RigidBody3DComponent&&) noexcept = default;

		private:
			inline void Construct(PhysWorld3D& world);

			using Setting = std::variant<RigidBody3D::DynamicSettings, RigidBody3D::StaticSettings>;
			std::unique_ptr<Setting> m_settings;
	};
}

#include <Nazara/Physics3D/Components/RigidBody3DComponent.inl>

#endif // NAZARA_PHYSICS3D_COMPONENTS_RIGIDBODY3DCOMPONENT_HPP
