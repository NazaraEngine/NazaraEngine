// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 3D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics3D/Physics3D.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Log.hpp>
#include <Nazara/Physics3D/Config.hpp>
#include <Nazara/Physics3D/Collider3D.hpp>
#include <newton/Newton.h>
#include <Nazara/Physics3D/Debug.hpp>

namespace Nz
{
	Physics3D::Physics3D(Config /*config*/) :
	ModuleBase("Physics3D", this)
	{
	}

	unsigned int Physics3D::GetMemoryUsed()
	{
		return NewtonGetMemoryUsed();
	}

	Physics3D* Physics3D::s_instance;
}
