// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Physics3D/PhysFilter3D.hpp>

namespace Nz
{
	PhysBodyFilter3D::~PhysBodyFilter3D() = default;

	PhysBroadphaseLayerInterface3D::~PhysBroadphaseLayerInterface3D() = default;

	PhysBroadphaseLayerFilter3D::~PhysBroadphaseLayerFilter3D() = default;

	PhysObjectLayerFilter3D::~PhysObjectLayerFilter3D() = default;

	PhysObjectLayerPairFilter3D::~PhysObjectLayerPairFilter3D() = default;

	PhysObjectVsBroadphaseLayerFilter3D::~PhysObjectVsBroadphaseLayerFilter3D() = default;
}
