// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

inline void NzPrimitive::MakeBox(const NzVector3f& lengths, const NzVector3ui& subdivision, const NzMatrix4f& transformMatrix, const NzRectf& uvCoords)
{
	matrix = transformMatrix;
	textureCoords = uvCoords;
	type = nzPrimitiveType_Box;
	box.lengths = lengths;
	box.subdivision = subdivision;
}

inline void NzPrimitive::MakeBox(const NzVector3f& lengths, const NzVector3ui& subdivision, const NzVector3f& position, const NzQuaternionf& rotation, const NzRectf& uvCoords)
{
	MakeBox(lengths, subdivision, NzMatrix4f::Transform(position, rotation), uvCoords);
}

inline void NzPrimitive::MakeCone(float length, float radius, unsigned int subdivision, const NzMatrix4f& transformMatrix, const NzRectf& uvCoords)
{
	matrix = transformMatrix;
	textureCoords = uvCoords;
	type = nzPrimitiveType_Cone;
	cone.length = length;
	cone.radius = radius;
	cone.subdivision = subdivision;
}

inline void NzPrimitive::MakeCone(float length, float radius, unsigned int subdivision, const NzVector3f& position, const NzQuaternionf& rotation, const NzRectf& uvCoords)
{
	MakeCone(length, radius, subdivision, NzMatrix4f::Transform(position, rotation), uvCoords);
}

inline void NzPrimitive::MakeCubicSphere(float size, unsigned int subdivision, const NzMatrix4f& transformMatrix, const NzRectf& uvCoords)
{
	matrix = transformMatrix;
	textureCoords = uvCoords;
	type = nzPrimitiveType_Sphere;
	sphere.size = size;
	sphere.type = nzSphereType_Cubic;
	sphere.cubic.subdivision = subdivision;
}

inline void NzPrimitive::MakeCubicSphere(float size, unsigned int subdivision, const NzVector3f& position, const NzQuaternionf& rotation, const NzRectf& uvCoords)
{
	MakeCubicSphere(size, subdivision, NzMatrix4f::Transform(position, rotation), uvCoords);
}

inline void NzPrimitive::MakeIcoSphere(float size, unsigned int recursionLevel, const NzMatrix4f& transformMatrix, const NzRectf& uvCoords)
{
	matrix = transformMatrix;
	textureCoords = uvCoords;
	type = nzPrimitiveType_Sphere;
	sphere.size = size;
	sphere.type = nzSphereType_Ico;
	sphere.ico.recursionLevel = recursionLevel;
}

inline void NzPrimitive::MakeIcoSphere(float size, unsigned int recursionLevel, const NzVector3f& position, const NzQuaternionf& rotation, const NzRectf& uvCoords)
{
	MakeIcoSphere(size, recursionLevel, NzMatrix4f::Transform(position, rotation), uvCoords);
}

inline void NzPrimitive::MakePlane(const NzVector2f& size, const NzVector2ui& subdivision, const NzMatrix4f& transformMatrix, const NzRectf& uvCoords)
{
	matrix = transformMatrix;
	textureCoords = uvCoords;
	type = nzPrimitiveType_Plane;
	plane.size = size;
	plane.subdivision = subdivision;
}

inline void NzPrimitive::MakePlane(const NzVector2f& size, const NzVector2ui& subdivision, const NzPlanef& planeInfo, const NzRectf& uvCoords)
{
	MakePlane(size, subdivision, NzMatrix4f::Transform(planeInfo.distance * planeInfo.normal, NzQuaternionf::RotationBetween(NzVector3f::Up(), planeInfo.normal)), uvCoords);
}

inline void NzPrimitive::MakePlane(const NzVector2f& size, const NzVector2ui& subdivision, const NzVector3f& position, const NzQuaternionf& rotation, const NzRectf& uvCoords)
{
	MakePlane(size, subdivision, NzMatrix4f::Transform(position, rotation), uvCoords);
}

inline void NzPrimitive::MakeUVSphere(float size, unsigned int sliceCount, unsigned int stackCount, const NzMatrix4f& transformMatrix, const NzRectf& uvCoords)
{
	matrix = transformMatrix;
	textureCoords = uvCoords;
	type = nzPrimitiveType_Sphere;
	sphere.size = size;
	sphere.type = nzSphereType_UV;
	sphere.uv.sliceCount = sliceCount;
	sphere.uv.stackCount = stackCount;
}

inline void NzPrimitive::MakeUVSphere(float size, unsigned int sliceCount, unsigned int stackCount, const NzVector3f& position, const NzQuaternionf& rotation, const NzRectf& uvCoords)
{
	MakeUVSphere(size, sliceCount, stackCount, NzMatrix4f::Transform(position, rotation), uvCoords);
}

inline NzPrimitive NzPrimitive::Box(const NzVector3f& lengths, const NzVector3ui& subdivision, const NzMatrix4f& transformMatrix, const NzRectf& uvCoords)
{
	NzPrimitive primitive;
	primitive.MakeBox(lengths, subdivision, transformMatrix, uvCoords);

	return primitive;
}

inline NzPrimitive NzPrimitive::Box(const NzVector3f& lengths, const NzVector3ui& subdivision, const NzVector3f& position, const NzQuaternionf& rotation, const NzRectf& uvCoords)
{
	NzPrimitive primitive;
	primitive.MakeBox(lengths, subdivision, position, rotation, uvCoords);

	return primitive;
}

inline NzPrimitive NzPrimitive::Cone(float length, float radius, unsigned int subdivision, const NzMatrix4f& transformMatrix, const NzRectf& uvCoords)
{
	NzPrimitive primitive;
	primitive.MakeCone(length, radius, subdivision, transformMatrix, uvCoords);

	return primitive;
}

inline NzPrimitive NzPrimitive::Cone(float length, float radius, unsigned int subdivision, const NzVector3f& position, const NzQuaternionf& rotation, const NzRectf& uvCoords)
{
	NzPrimitive primitive;
	primitive.MakeCone(length, radius, subdivision, position, rotation, uvCoords);

	return primitive;
}

inline NzPrimitive NzPrimitive::CubicSphere(float size, unsigned int subdivision, const NzMatrix4f& transformMatrix, const NzRectf& uvCoords)
{
	NzPrimitive primitive;
	primitive.MakeCubicSphere(size, subdivision, transformMatrix, uvCoords);

	return primitive;
}

inline NzPrimitive NzPrimitive::CubicSphere(float size, unsigned int subdivision, const NzVector3f& position, const NzQuaternionf& rotation, const NzRectf& uvCoords)
{
	NzPrimitive primitive;
	primitive.MakeCubicSphere(size, subdivision, position, rotation, uvCoords);

	return primitive;
}

inline NzPrimitive NzPrimitive::IcoSphere(float size, unsigned int recursionLevel, const NzMatrix4f& transformMatrix, const NzRectf& uvCoords)
{
	NzPrimitive primitive;
	primitive.MakeIcoSphere(size, recursionLevel, transformMatrix, uvCoords);

	return primitive;
}

inline NzPrimitive NzPrimitive::IcoSphere(float size, unsigned int recursionLevel, const NzVector3f& position, const NzQuaternionf& rotation, const NzRectf& uvCoords)
{
	NzPrimitive primitive;
	primitive.MakeIcoSphere(size, recursionLevel, position, rotation, uvCoords);

	return primitive;
}

inline NzPrimitive NzPrimitive::Plane(const NzVector2f& size, const NzVector2ui& subdivision, const NzMatrix4f& transformMatrix, const NzRectf& uvCoords)
{
	NzPrimitive primitive;
	primitive.MakePlane(size, subdivision, transformMatrix, uvCoords);

	return primitive;
}

inline NzPrimitive NzPrimitive::Plane(const NzVector2f& size, const NzVector2ui& subdivision, const NzPlanef& plane, const NzRectf& uvCoords)
{
	NzPrimitive primitive;
	primitive.MakePlane(size, subdivision, plane, uvCoords);

	return primitive;
}

inline NzPrimitive NzPrimitive::Plane(const NzVector2f& size, const NzVector2ui& subdivision, const NzVector3f& position, const NzQuaternionf& rotation, const NzRectf& uvCoords)
{
	NzPrimitive primitive;
	primitive.MakePlane(size, subdivision, position, rotation, uvCoords);

	return primitive;
}

inline NzPrimitive NzPrimitive::UVSphere(float size, unsigned int sliceCount, unsigned int stackCount, const NzMatrix4f& transformMatrix, const NzRectf& uvCoords)
{
	NzPrimitive primitive;
	primitive.MakeUVSphere(size, sliceCount, stackCount, transformMatrix, uvCoords);

	return primitive;
}

inline NzPrimitive NzPrimitive::UVSphere(float size, unsigned int sliceCount, unsigned int stackCount, const NzVector3f& position, const NzQuaternionf& rotation, const NzRectf& uvCoords)
{
	NzPrimitive primitive;
	primitive.MakeUVSphere(size, sliceCount, stackCount, position, rotation, uvCoords);

	return primitive;
}

#include <Nazara/Core/DebugOff.hpp>
