// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_COMPONENTS_VELOCITYCOMPONENT_HPP
#define NAZARA_CORE_COMPONENTS_VELOCITYCOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Core/Export.hpp>
#include <Nazara/Math/Vector3.hpp>

namespace Nz
{
	class NAZARA_CORE_API VelocityComponent
	{
		public:
			inline VelocityComponent(const Vector3f& linearVelocity = Vector3f::Zero());
			VelocityComponent(const VelocityComponent&) = default;
			VelocityComponent(VelocityComponent&&) = default;
			~VelocityComponent() = default;

			inline const Vector3f& GetLinearVelocity() const;

			inline void UpdateLinearVelocity(const Vector3f& linearVelocity);

			VelocityComponent& operator=(const VelocityComponent&) = default;
			VelocityComponent& operator=(VelocityComponent&&) = default;

		private:
			Vector3f m_linearVelocity;
	};
}

#include <Nazara/Core/Components/VelocityComponent.inl>

#endif // NAZARA_CORE_COMPONENTS_VELOCITYCOMPONENT_HPP
