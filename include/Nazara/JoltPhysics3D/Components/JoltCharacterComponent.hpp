// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - JoltPhysics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_JOLTPHYSICS3D_COMPONENTS_JOLTCHARACTERCOMPONENT_HPP
#define NAZARA_JOLTPHYSICS3D_COMPONENTS_JOLTCHARACTERCOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/JoltPhysics3D/JoltCharacter.hpp>

namespace Nz
{
	class NAZARA_JOLTPHYSICS3D_API JoltCharacterComponent : public JoltCharacter
	{
		friend class JoltPhysics3DSystem;

		public:
			inline JoltCharacterComponent(const JoltCharacter::Settings& settings);
			JoltCharacterComponent(const JoltCharacterComponent&) = default;
			JoltCharacterComponent(JoltCharacterComponent&&) noexcept = default;
			~JoltCharacterComponent() = default;

			JoltCharacterComponent& operator=(const JoltCharacterComponent&) = default;
			JoltCharacterComponent& operator=(JoltCharacterComponent&&) noexcept = default;

		private:
			inline void Construct(JoltPhysWorld3D& world);

			std::unique_ptr<JoltCharacter::Settings> m_settings;
	};
}

#include <Nazara/JoltPhysics3D/Components/JoltCharacterComponent.inl>

#endif // NAZARA_JOLTPHYSICS3D_COMPONENTS_JOLTCHARACTERCOMPONENT_HPP
