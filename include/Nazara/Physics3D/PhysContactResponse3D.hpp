// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_PHYSCONTACTRESPONSE3D_HPP
#define NAZARA_PHYSICS3D_PHYSCONTACTRESPONSE3D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Math/Vector3.hpp>

namespace Nz
{
NAZARA_WARNING_PUSH()
NAZARA_WARNING_MSVC_DISABLE(4324)

	// Made to match JPH::ContactSettings hoping that compiler optimizes it fully
	struct PhysContactResponse3D
	{
		float combinedFriction;
		float combinedRestitution;
		float invMassScale1;
		float invInertiaScale1;
		float invMassScale2;
		float invInertiaScale2;
		bool isSensor;
		alignas(16) Nz::Vector3f relativeLinearSurfaceVelocity;
		alignas(16) Nz::Vector3f relativeAngularSurfaceVelocity;

		inline void SwapBodies();
	};

NAZARA_WARNING_POP()
}

#include <Nazara/Physics3D/PhysContactResponse3D.inl>

#endif // NAZARA_PHYSICS3D_PHYSCONTACTRESPONSE3D_HPP
