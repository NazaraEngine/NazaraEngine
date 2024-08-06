// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_ENUMS_HPP
#define NAZARA_PHYSICS3D_ENUMS_HPP

namespace Nz
{
	enum class ColliderType3D
	{
		Box,
		Capsule,
		Compound,
		Convex,
		Mesh,
		Sphere,

		ScaledDecoration,
		TranslatedRotatedDecoration,

		Max = TranslatedRotatedDecoration
	};

	enum class PhysContactValidateResult3D
	{
		AcceptAllContactsForThisBodyPair, ///< Accept this and any further contact points for this body pair
		AcceptContact,                    ///< Accept this contact only (and continue calling this callback for every contact manifold for the same body pair)
		RejectContact,                    ///< Reject this contact only (but process any other contact manifolds for the same body pair)
		RejectAllContactsForThisBodyPair  ///< Rejects this and any further contact points for this body pair
	};

	enum class PhysMotionQuality3D
	{
		Discrete,
		LinearCast
	};
}

#endif // NAZARA_PHYSICS3D_ENUMS_HPP
