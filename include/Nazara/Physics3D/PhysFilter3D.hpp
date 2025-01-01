// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_PHYSICS3D_PHYSFILTER3D_HPP
#define NAZARA_PHYSICS3D_PHYSFILTER3D_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Physics3D/Enums.hpp>
#include <Nazara/Physics3D/Export.hpp>
#include <Nazara/Physics3D/PhysObjectLayer3D.hpp>

namespace Nz
{
	class PhysBody3D;

	struct NAZARA_PHYSICS3D_API PhysBodyFilter3D
	{
		virtual ~PhysBodyFilter3D();

		virtual bool ShouldCollide(UInt32 bodyIndex) const = 0;
		virtual bool ShouldCollideLocked(const PhysBody3D& body) const = 0;
	};

	struct NAZARA_PHYSICS3D_API PhysBroadphaseLayerInterface3D
	{
		virtual ~PhysBroadphaseLayerInterface3D();

		virtual PhysBroadphase3D GetBroadphaseLayer(PhysObjectLayer3D layer) const = 0;
		virtual unsigned int GetBroadphaseLayerCount() const = 0;
		virtual const char* GetBroadphaseLayerName(PhysBroadphase3D broadphaseLayer) const = 0;
	};

	struct NAZARA_PHYSICS3D_API PhysBroadphaseLayerFilter3D
	{
		virtual ~PhysBroadphaseLayerFilter3D();

		virtual bool ShouldCollide(PhysBroadphase3D layer) const = 0;
	};

	struct NAZARA_PHYSICS3D_API PhysObjectLayerFilter3D
	{
		virtual ~PhysObjectLayerFilter3D();

		virtual bool ShouldCollide(PhysObjectLayer3D layer) const = 0;
	};

	struct NAZARA_PHYSICS3D_API PhysObjectLayerPairFilter3D
	{
		virtual ~PhysObjectLayerPairFilter3D();

		virtual bool ShouldCollide(PhysObjectLayer3D object1, PhysObjectLayer3D object2) const = 0;
	};

	struct NAZARA_PHYSICS3D_API PhysObjectVsBroadphaseLayerFilter3D
	{
		virtual ~PhysObjectVsBroadphaseLayerFilter3D();

		virtual bool ShouldCollide(PhysObjectLayer3D objectLayer, PhysBroadphase3D broadphaseLayer) const = 0;
	};
}

#include <Nazara/Physics3D/PhysFilter3D.inl>

#endif // NAZARA_PHYSICS3D_PHYSFILTER3D_HPP
