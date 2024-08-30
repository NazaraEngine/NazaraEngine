// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_COMPONENTS_PHYSCHARACTER3DCOMPONENT_HPP
#define NAZARA_PHYSICS3D_COMPONENTS_PHYSCHARACTER3DCOMPONENT_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Physics3D/PhysCharacter3D.hpp>

namespace Nz
{
	class NAZARA_PHYSICS3D_API PhysCharacter3DComponent : public PhysCharacter3D
	{
		friend class Physics3DSystem;

		public:
			inline PhysCharacter3DComponent(const PhysCharacter3D::Settings& settings);
			PhysCharacter3DComponent(const PhysCharacter3DComponent&) = delete;
			PhysCharacter3DComponent(PhysCharacter3DComponent&&) noexcept = default;
			~PhysCharacter3DComponent() = default;

			PhysCharacter3DComponent& operator=(const PhysCharacter3DComponent&) = delete;
			PhysCharacter3DComponent& operator=(PhysCharacter3DComponent&&) noexcept = default;

		private:
			inline void Construct(PhysWorld3D& world);

			std::unique_ptr<PhysCharacter3D::Settings> m_settings;
	};
}

#include <Nazara/Physics3D/Components/PhysCharacter3DComponent.inl>

#endif // NAZARA_PHYSICS3D_COMPONENTS_PHYSCHARACTER3DCOMPONENT_HPP
