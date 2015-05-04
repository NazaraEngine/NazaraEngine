// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Physics/Debug.hpp>

template<typename... Args>
NzBoxGeomRef NzBoxGeom::New(Args&&... args)
{
	std::unique_ptr<NzBoxGeom> object(new NzBoxGeom(std::forward<Args>(args)...));
	object->SetPersistent(false);

	return object.release();
}

template<typename... Args>
NzCapsuleGeomRef NzCapsuleGeom::New(Args&&... args)
{
	std::unique_ptr<NzCapsuleGeom> object(new NzCapsuleGeom(std::forward<Args>(args)...));
	object->SetPersistent(false);

	return object.release();
}

template<typename... Args>
NzCompoundGeomRef NzCompoundGeom::New(Args&&... args)
{
	std::unique_ptr<NzCompoundGeom> object(new NzCompoundGeom(std::forward<Args>(args)...));
	object->SetPersistent(false);

	return object.release();
}

template<typename... Args>
NzConeGeomRef NzConeGeom::New(Args&&... args)
{
	std::unique_ptr<NzConeGeom> object(new NzConeGeom(std::forward<Args>(args)...));
	object->SetPersistent(false);

	return object.release();
}

template<typename... Args>
NzConvexHullGeomRef NzConvexHullGeom::New(Args&&... args)
{
	std::unique_ptr<NzConvexHullGeom> object(new NzConvexHullGeom(std::forward<Args>(args)...));
	object->SetPersistent(false);

	return object.release();
}

template<typename... Args>
NzCylinderGeomRef NzCylinderGeom::New(Args&&... args)
{
	std::unique_ptr<NzCylinderGeom> object(new NzCylinderGeom(std::forward<Args>(args)...));
	object->SetPersistent(false);

	return object.release();
}

template<typename... Args>
NzNullGeomRef NzNullGeom::New(Args&&... args)
{
	std::unique_ptr<NzNullGeom> object(new NzNullGeom(std::forward<Args>(args)...));
	object->SetPersistent(false);

	return object.release();
}

template<typename... Args>
NzSphereGeomRef NzSphereGeom::New(Args&&... args)
{
	std::unique_ptr<NzSphereGeom> object(new NzSphereGeom(std::forward<Args>(args)...));
	object->SetPersistent(false);

	return object.release();
}

#include <Nazara/Physics/DebugOff.hpp>
