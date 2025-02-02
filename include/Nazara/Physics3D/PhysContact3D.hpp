// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_PHYSCONTACT3D_HPP
#define NAZARA_PHYSICS3D_PHYSCONTACT3D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <NazaraUtils/FixedVector.hpp>
#include <Nazara/Math/Vector3.hpp>

namespace Nz
{
	struct PhysContact3D
	{
		FixedVector<Vector3f, 64> relativeContactPositions1;
		FixedVector<Vector3f, 64> relativeContactPositions2;
		Vector3f baseOffset;
		Vector3f normal;
		float penetrationDepth;
		UInt32 subshapeID1;
		UInt32 subshapeID2;

		inline PhysContact3D SwapBodies() const;
	};
}

#include <Nazara/Physics3D/PhysContact3D.inl>

#endif // NAZARA_PHYSICS3D_PHYSCONTACT3D_HPP
