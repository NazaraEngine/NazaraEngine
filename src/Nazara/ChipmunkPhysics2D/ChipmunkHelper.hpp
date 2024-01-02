// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - ChipmunkPhysics2D module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CHIPMUNKPHYSICS2D_CHIPMUNKHELPER_HPP
#define NAZARA_CHIPMUNKPHYSICS2D_CHIPMUNKHELPER_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <chipmunk/cpVect.h>

namespace Nz
{
	inline Vector2f FromChipmunk(const cpVect& vect);
	inline cpVect ToChipmunk(const Vector2f& vec);
}

#include <Nazara/ChipmunkPhysics2D/ChipmunkHelper.inl>

#endif // NAZARA_CHIPMUNKPHYSICS2D_CHIPMUNKHELPER_HPP
