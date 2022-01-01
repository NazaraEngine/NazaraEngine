// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics2D/Physics2D.hpp>
#include <Nazara/Physics2D/Debug.hpp>

namespace Nz
{
	Physics2D::Physics2D(Config /*config*/) :
	ModuleBase("Physics2D", this)
	{
	}

	Physics2D* Physics2D::s_instance = nullptr;
}
