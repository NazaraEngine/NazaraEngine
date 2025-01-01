// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Physics2D module"
// For conditions of distribution and use, see copyright notice in Export.hpp


namespace Nz
{
	inline Vector2f FromChipmunk(const cpVect& vect)
	{
		return { float(vect.x), float(vect.y) };
	}

	inline cpVect ToChipmunk(const Vector2f& vec)
	{
		return cpv(cpFloat(vec.x), cpFloat(vec.y));
	}
}
