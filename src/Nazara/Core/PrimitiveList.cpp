// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/PrimitiveList.hpp>
#include <Nazara/Core/Config.hpp>
#include <Nazara/Core/Debug.hpp>

void NzPrimitiveList::AddCube(const NzCubef& cube, const NzVector3ui& subdivision, const NzMatrix4f& matrix)
{
	NzPrimitive primitive;
	primitive.type = nzPrimitiveType_Cube;
	primitive.cube.cube = cube; // cube.cube = cube, parce que je le vaux bien
	primitive.cube.matrix = matrix;
	primitive.cube.subdivision = subdivision;

	m_primitives.push_back(primitive);
}

void NzPrimitiveList::AddCube(const NzCubef& cube, const NzVector3ui& subdivision, const NzVector3f& position, const NzQuaternionf& rotation)
{
	AddCube(cube, subdivision, NzMatrix4f::Transform(position, rotation));
}

void NzPrimitiveList::AddCubicSphere(float size, unsigned int subdivision, const NzMatrix4f& matrix)
{
	NzPrimitive primitive;
	primitive.type = nzPrimitiveType_Sphere;
	primitive.sphere.matrix = matrix;
	primitive.sphere.size = size;
	primitive.sphere.type = nzSphereType_Cubic;
	primitive.sphere.cubic.subdivision = subdivision;

	m_primitives.push_back(primitive);
}

void NzPrimitiveList::AddCubicSphere(float size, unsigned int subdivision, const NzVector3f& position, const NzQuaternionf& rotation)
{
	AddCubicSphere(size, subdivision, NzMatrix4f::Transform(position, rotation));
}

void NzPrimitiveList::AddIcoSphere(float size, unsigned int recursionLevel, const NzMatrix4f& matrix)
{
	NzPrimitive primitive;
	primitive.type = nzPrimitiveType_Sphere;
	primitive.sphere.matrix = matrix;
	primitive.sphere.size = size;
	primitive.sphere.type = nzSphereType_Ico;
	primitive.sphere.ico.recursionLevel = recursionLevel;

	m_primitives.push_back(primitive);
}

void NzPrimitiveList::AddIcoSphere(float size, unsigned int recursionLevel, const NzVector3f& position, const NzQuaternionf& rotation)
{
	AddIcoSphere(size, recursionLevel, NzMatrix4f::Transform(position, rotation));
}

void NzPrimitiveList::AddPlane(const NzPlanef& plane, const NzVector2f& size, const NzVector2ui& subdivision)
{
	NzPrimitive primitive;
	primitive.type = nzPrimitiveType_Plane;
	primitive.plane.normal = plane.normal;
	primitive.plane.position = plane.distance * plane.normal;
	primitive.plane.size = size;
	primitive.plane.subdivision = subdivision;

	m_primitives.push_back(primitive);
}

void NzPrimitiveList::AddPlane(const NzVector3f& position, const NzVector3f& normal, const NzVector2f& size, const NzVector2ui& subdivision)
{
	NzPrimitive primitive;
	primitive.type = nzPrimitiveType_Plane;
	primitive.plane.normal = normal;
	primitive.plane.position = position;
	primitive.plane.size = size;
	primitive.plane.subdivision = subdivision;

	m_primitives.push_back(primitive);
}

void NzPrimitiveList::AddUVSphere(float size, unsigned int slices, unsigned int stacks, const NzMatrix4f& matrix)
{
	NzPrimitive primitive;
	primitive.type = nzPrimitiveType_Sphere;
	primitive.sphere.matrix = matrix;
	primitive.sphere.size = size;
	primitive.sphere.type = nzSphereType_UV;
	primitive.sphere.uv.slices = slices;
	primitive.sphere.uv.stacks = stacks;

	m_primitives.push_back(primitive);
}

void NzPrimitiveList::AddUVSphere(float size, unsigned int slices, unsigned int stacks, const NzVector3f& position, const NzQuaternionf& rotation)
{
	AddUVSphere(size, slices, stacks, NzMatrix4f::Transform(position, rotation));
}

NzPrimitive& NzPrimitiveList::GetPrimitive(unsigned int i)
{
	#if NAZARA_CORE_SAFE
	if (i >= m_primitives.size())
	{
		NazaraError("Primitive index out of range (" + NzString::Number(i) + " >= " + NzString::Number(m_primitives.size()) + ')');

		static NzPrimitive dummy;
		return dummy;
	}
	#endif

	return m_primitives[i];
}

const NzPrimitive& NzPrimitiveList::GetPrimitive(unsigned int i) const
{
	#if NAZARA_CORE_SAFE
	if (i >= m_primitives.size())
	{
		NazaraError("Primitive index out of range (" + NzString::Number(i) + " >= " + NzString::Number(m_primitives.size()) + ')');

		static NzPrimitive dummy;
		return dummy;
	}
	#endif

	return m_primitives[i];
}

unsigned int NzPrimitiveList::GetSize() const
{
	return m_primitives.size();
}

NzPrimitive& NzPrimitiveList::operator()(unsigned int i)
{
	return GetPrimitive(i);
}

const NzPrimitive& NzPrimitiveList::operator()(unsigned int i) const
{
	return GetPrimitive(i);
}
