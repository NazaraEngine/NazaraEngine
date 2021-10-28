// Copyright (C) 2021 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics3D/Physics3D.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/ECS.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Physics3D/Collider3D.hpp>
#include <Nazara/Physics3D/Config.hpp>
#include <newton/Newton.h>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	Physics3D::Physics3D(Config /*config*/) :
	ModuleBase("Physics3D", this)
	{
		ECS::RegisterComponents();
	}

	unsigned int Physics3D::GetMemoryUsed()
	{
		return NewtonGetMemoryUsed();
	}

	Physics3D* Physics3D::s_instance;
}
