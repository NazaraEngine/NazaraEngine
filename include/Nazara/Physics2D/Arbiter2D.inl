// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Physics 2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Physics2D/Arbiter2D.hpp>
#include <memory>
#include <Nazara/Physics2D/Debug.hpp>

namespace Nz
{
	inline Arbiter2D::Arbiter2D(cpArbiter* arbiter) :
	m_arbiter(arbiter)
	{
	}
}

#include <Nazara/Physics2D/DebugOff.hpp>
