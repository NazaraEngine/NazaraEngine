// Copyright (C) 2023 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <memory>
#include <Nazara/ChipmunkPhysics2D/Debug.hpp>

namespace Nz
{
	inline ChipmunkArbiter2D::ChipmunkArbiter2D(cpArbiter* arbiter) :
	m_arbiter(arbiter)
	{
	}
}

#include <Nazara/ChipmunkPhysics2D/DebugOff.hpp>
