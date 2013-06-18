// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PRIMITIVE_HPP
#define NAZARA_PRIMITIVE_HPP

#include <Nazara/Core/Enums.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Plane.hpp>
#include <Nazara/Math/Vector3.hpp>

struct NzPrimitive
{
	NzMatrix4f matrix;
	nzPrimitiveType type;

	union
	{
		struct
		{
			NzVector3f lengths;
			NzVector3ui subdivision;
		}
		box;

		struct
		{
			NzVector2f size;
			NzVector2ui subdivision;
		}
		plane;

		struct
		{
			nzSphereType type;
			float size;

			union
			{
				struct
				{
					unsigned int subdivision;
				}
				cubic;

				struct
				{
					unsigned int recursionLevel;
				}
				ico;

				struct
				{
					unsigned int slices;
					unsigned int stacks;
				}
				uv;
			};
		}
		sphere;
	};
};

#endif // NAZARA_PRIMITIVE_HPP
