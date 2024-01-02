// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_UTILITY_COMPONENTS_VELOCITYCOMPONENT_HPP
#define NAZARA_UTILITY_COMPONENTS_VELOCITYCOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Utility/Config.hpp>

namespace Nz
{
	class NAZARA_UTILITY_API VelocityComponent
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

#include <Nazara/Utility/Components/VelocityComponent.inl>

#endif // NAZARA_UTILITY_COMPONENTS_VELOCITYCOMPONENT_HPP
