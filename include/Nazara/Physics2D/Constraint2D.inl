// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics2D/Constraint2D.hpp>
#include <memory>
#include <Nazara/Physics2D/Debug.hpp>

namespace Nz
{
	template<typename... Args>
	DampedSpringConstraint2DRef DampedSpringConstraint2D::New(Args&&... args)
	{
		std::unique_ptr<DampedSpringConstraint2D> object(new DampedSpringConstraint2D(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}

	template<typename... Args>
	DampedRotarySpringConstraint2DRef DampedRotarySpringConstraint2D::New(Args&&... args)
	{
		std::unique_ptr<DampedRotarySpringConstraint2D> object(new DampedRotarySpringConstraint2D(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}

	template<typename... Args>
	GearConstraint2DRef GearConstraint2D::New(Args&&... args)
	{
		std::unique_ptr<GearConstraint2D> object(new GearConstraint2D(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}

	template<typename... Args>
	MotorConstraint2DRef MotorConstraint2D::New(Args&&... args)
	{
		std::unique_ptr<MotorConstraint2D> object(new MotorConstraint2D(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}

	template<typename... Args>
	PinConstraint2DRef PinConstraint2D::New(Args&&... args)
	{
		std::unique_ptr<PinConstraint2D> object(new PinConstraint2D(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}

	template<typename... Args>
	PivotConstraint2DRef PivotConstraint2D::New(Args&&... args)
	{
		std::unique_ptr<PivotConstraint2D> object(new PivotConstraint2D(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}

	template<typename... Args>
	RatchetConstraint2DRef RatchetConstraint2D::New(Args&&... args)
	{
		std::unique_ptr<RatchetConstraint2D> object(new RatchetConstraint2D(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}

	template<typename... Args>
	RotaryLimitConstraint2DRef RotaryLimitConstraint2D::New(Args&&... args)
	{
		std::unique_ptr<RotaryLimitConstraint2D> object(new RotaryLimitConstraint2D(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}

	template<typename... Args>
	SlideConstraint2DRef SlideConstraint2D::New(Args&&... args)
	{
		std::unique_ptr<SlideConstraint2D> object(new SlideConstraint2D(std::forward<Args>(args)...));
		object->SetPersistent(false);

		return object.release();
	}
}

#include <Nazara/Physics2D/DebugOff.hpp>
