// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	template<typename... Args>
	BoxCollider3DRef BoxCollider3D::New(Args&&... args)
	{
		std::unique_ptr<BoxCollider3D> object(new BoxCollider3D(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}

	template<typename... Args>
	CapsuleCollider3DRef CapsuleCollider3D::New(Args&&... args)
	{
		std::unique_ptr<CapsuleCollider3D> object(new CapsuleCollider3D(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}

	template<typename... Args>
	CompoundCollider3DRef CompoundCollider3D::New(Args&&... args)
	{
		std::unique_ptr<CompoundCollider3D> object(new CompoundCollider3D(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}

	template<typename... Args>
	ConeCollider3DRef ConeCollider3D::New(Args&&... args)
	{
		std::unique_ptr<ConeCollider3D> object(new ConeCollider3D(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}

	template<typename... Args>
	ConvexCollider3DRef ConvexCollider3D::New(Args&&... args)
	{
		std::unique_ptr<ConvexCollider3D> object(new ConvexCollider3D(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}

	template<typename... Args>
	CylinderCollider3DRef CylinderCollider3D::New(Args&&... args)
	{
		std::unique_ptr<CylinderCollider3D> object(new CylinderCollider3D(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}

	template<typename... Args>
	NullCollider3DRef NullCollider3D::New(Args&&... args)
	{
		std::unique_ptr<NullCollider3D> object(new NullCollider3D(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}

	template<typename... Args>
	SphereCollider3DRef SphereCollider3D::New(Args&&... args)
	{
		std::unique_ptr<SphereCollider3D> object(new SphereCollider3D(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Physics3D/DebugOff.hpp>
