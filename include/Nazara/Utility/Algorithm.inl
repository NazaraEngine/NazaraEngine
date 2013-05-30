// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Debug.hpp>

void NzTransformVertex(NzMeshVertex* vertex, const NzMatrix4f& matrix)
{
	vertex->normal = matrix.Transform(vertex->normal, 0.f);
	vertex->position = matrix.Transform(vertex->position);
	vertex->tangent = matrix.Transform(vertex->tangent, 0.f);
	vertex++;
}

void NzTransformVertices(NzMeshVertex* vertices, unsigned int vertexCount, const NzMatrix4f& matrix)
{
	if (matrix.IsIdentity())
		return;

	for (unsigned int i = 0; i < vertexCount; ++i)
		NzTransformVertex(vertices++, matrix);
}

#include <Nazara/Utility/DebugOff.hpp>
