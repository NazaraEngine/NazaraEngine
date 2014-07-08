// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Debug.hpp>

template<typename T>
NzBoxf NzComputeVerticesAABB(const T* vertices, unsigned int vertexCount)
{
	NzBoxf aabb;
	if (vertexCount > 0)
	{
		aabb.Set(vertices->position.x, vertices->position.y, vertices->position.z, 0.f, 0.f, 0.f);
		vertices++;

		for (unsigned int i = 1; i < vertexCount; ++i)
		{
			aabb.ExtendTo(vertices->position);
			vertices++;
		}
	}
	else
		aabb.MakeZero();

	return aabb;
}

template<typename T>
void NzTransformVertices(T* vertices, unsigned int vertexCount, const NzMatrix4f& matrix)
{
	NzVector3f scale = matrix.GetScale();

	for (unsigned int i = 0; i < vertexCount; ++i)
	{
		vertices->normal = matrix.Transform(vertices->normal, 0.f) / scale;
		vertices->position = matrix.Transform(vertices->position);
		vertices->tangent = matrix.Transform(vertices->tangent, 0.f) / scale;
		vertices++;
	}
}

#include <Nazara/Utility/DebugOff.hpp>
