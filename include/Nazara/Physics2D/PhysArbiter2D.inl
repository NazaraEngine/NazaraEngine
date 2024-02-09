// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/Physics2D/Debug.hpp>

namespace Nz
{
	inline PhysArbiter2D::PhysArbiter2D(cpArbiter* arbiter) :
	m_arbiter(arbiter)
	{
	}
}

#include <Nazara/Physics2D/DebugOff.hpp>
