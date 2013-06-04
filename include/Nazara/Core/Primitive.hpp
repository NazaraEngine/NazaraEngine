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
	nzPrimitiveType type;

	union
	{
		struct
		{
			NzMatrix4f matrix;
			NzVector3f lengths;
			NzVector3ui subdivision;
		}
		box;

		struct
		{
			NzVector2f size;
			NzVector2ui subdivision;
			NzVector3f normal;
			NzVector3f position;
		}
		plane;

		struct
		{
			nzSphereType type;
			NzMatrix4f matrix;
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
