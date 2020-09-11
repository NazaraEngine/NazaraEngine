// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics2D/Physics2D.hpp>
#include <Nazara/Physics2D/Debug.hpp>

namespace Nz
{
	Physics2D::Physics2D() :
	ModuleBase("Physics2D", this)
	{
	}

	Physics2D* Physics2D::s_instance = nullptr;
}
