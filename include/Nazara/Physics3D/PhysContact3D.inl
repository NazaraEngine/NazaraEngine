// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

namespace Nz
{
	inline PhysContact3D PhysContact3D::SwapBodies() const
	{
		PhysContact3D contact(*this);
		contact.normal = -contact.normal;
		std::swap(contact.relativeContactPositions1, contact.relativeContactPositions2);
		std::swap(contact.subshapeID1, contact.subshapeID2);

		return contact;
	}
}
