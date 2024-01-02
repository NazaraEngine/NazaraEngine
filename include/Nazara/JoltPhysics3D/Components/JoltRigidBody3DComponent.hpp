// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_JOLTPHYSICS3D_COMPONENTS_JOLTRIGIDBODY3DCOMPONENT_HPP
#define NAZARA_JOLTPHYSICS3D_COMPONENTS_JOLTRIGIDBODY3DCOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/JoltPhysics3D/JoltRigidBody3D.hpp>
#include <variant>

namespace Nz
{
	class NAZARA_JOLTPHYSICS3D_API JoltRigidBody3DComponent : public JoltRigidBody3D
	{
		friend class JoltPhysics3DSystem;

		public:
			inline JoltRigidBody3DComponent(const JoltRigidBody3D::DynamicSettings& settings);
			inline JoltRigidBody3DComponent(const JoltRigidBody3D::StaticSettings& settings);
			JoltRigidBody3DComponent(const JoltRigidBody3DComponent&) = default;
			JoltRigidBody3DComponent(JoltRigidBody3DComponent&&) noexcept = default;
			~JoltRigidBody3DComponent() = default;

			JoltRigidBody3DComponent& operator=(const JoltRigidBody3DComponent&) = default;
			JoltRigidBody3DComponent& operator=(JoltRigidBody3DComponent&&) noexcept = default;

		private:
			inline void Construct(JoltPhysWorld3D& world);

			using Setting = std::variant<JoltRigidBody3D::DynamicSettings, JoltRigidBody3D::StaticSettings>;
			std::unique_ptr<Setting> m_settings;
	};
}

#include <Nazara/JoltPhysics3D/Components/JoltRigidBody3DComponent.inl>

#endif // NAZARA_JOLTPHYSICS3D_COMPONENTS_JOLTRIGIDBODY3DCOMPONENT_HPP
