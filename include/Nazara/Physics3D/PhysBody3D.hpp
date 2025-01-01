// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_PHYSBODY3D_HPP
#define NAZARA_PHYSICS3D_PHYSBODY3D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Physics3D/Export.hpp>

namespace Nz
{
	class NAZARA_PHYSICS3D_API PhysBody3D
	{
		public:
			PhysBody3D() = default;
			PhysBody3D(const PhysBody3D&) = delete;
			PhysBody3D(PhysBody3D&&) = delete;
			virtual ~PhysBody3D();

			virtual UInt32 GetBodyIndex() const = 0;

			PhysBody3D& operator=(const PhysBody3D&) = delete;
			PhysBody3D& operator=(PhysBody3D&&) = delete;
	};
}

#include <Nazara/Physics3D/PhysBody3D.inl>

#endif // NAZARA_PHYSICS3D_PHYSBODY3D_HPP
