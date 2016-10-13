// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Physics/Debug.hpp>

namespace Nz
{
	template<typename... Args>
	BoxGeomRef BoxGeom::New(Args&&... args)
	{
		std::unique_ptr<BoxGeom> object(new BoxGeom(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}

	template<typename... Args>
	CapsuleGeomRef CapsuleGeom::New(Args&&... args)
	{
		std::unique_ptr<CapsuleGeom> object(new CapsuleGeom(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}

	template<typename... Args>
	CompoundGeomRef CompoundGeom::New(Args&&... args)
	{
		std::unique_ptr<CompoundGeom> object(new CompoundGeom(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}

	template<typename... Args>
	ConeGeomRef ConeGeom::New(Args&&... args)
	{
		std::unique_ptr<ConeGeom> object(new ConeGeom(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}

	template<typename... Args>
	ConvexHullGeomRef ConvexHullGeom::New(Args&&... args)
	{
		std::unique_ptr<ConvexHullGeom> object(new ConvexHullGeom(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}

	template<typename... Args>
	CylinderGeomRef CylinderGeom::New(Args&&... args)
	{
		std::unique_ptr<CylinderGeom> object(new CylinderGeom(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}

	template<typename... Args>
	NullGeomRef NullGeom::New(Args&&... args)
	{
		std::unique_ptr<NullGeom> object(new NullGeom(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}

	template<typename... Args>
	SphereGeomRef SphereGeom::New(Args&&... args)
	{
		std::unique_ptr<SphereGeom> object(new SphereGeom(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Physics/DebugOff.hpp>
