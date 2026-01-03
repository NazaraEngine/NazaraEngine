// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <utility>

namespace Nz
{
	inline void PhysContactResponse3D::SwapBodies()
	{
		std::swap(invMassScale1, invMassScale2);
		std::swap(invInertiaScale1, invInertiaScale2);
		relativeLinearSurfaceVelocity = -relativeLinearSurfaceVelocity;
		relativeAngularSurfaceVelocity = -relativeAngularSurfaceVelocity;
	}
}
