// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

inline void NzPrimitive::MakeBox(const NzVector3f& lengths, const NzVector3ui& subdivision, const NzMatrix4f& transformMatrix)
{
	matrix = transformMatrix;
	type = nzPrimitiveType_Box;
	box.lengths = lengths;
	box.subdivision = subdivision;
}

inline void NzPrimitive::MakeBox(const NzVector3f& lengths, const NzVector3ui& subdivision, const NzVector3f& position, const NzQuaternionf& rotation)
{
	MakeBox(lengths, subdivision, NzMatrix4f::Transform(position, rotation));
}

inline void NzPrimitive::MakeCubicSphere(float size, unsigned int subdivision, const NzMatrix4f& transformMatrix)
{
	matrix = transformMatrix;
	type = nzPrimitiveType_Sphere;
	sphere.size = size;
	sphere.type = nzSphereType_Cubic;
	sphere.cubic.subdivision = subdivision;
}

inline void NzPrimitive::MakeCubicSphere(float size, unsigned int subdivision, const NzVector3f& position, const NzQuaternionf& rotation)
{
	MakeCubicSphere(size, subdivision, NzMatrix4f::Transform(position, rotation));
}

inline void NzPrimitive::MakeIcoSphere(float size, unsigned int recursionLevel, const NzMatrix4f& transformMatrix)
{
	matrix = transformMatrix;
	type = nzPrimitiveType_Sphere;
	sphere.size = size;
	sphere.type = nzSphereType_Ico;
	sphere.ico.recursionLevel = recursionLevel;
}

inline void NzPrimitive::MakeIcoSphere(float size, unsigned int recursionLevel, const NzVector3f& position, const NzQuaternionf& rotation)
{
	MakeIcoSphere(size, recursionLevel, NzMatrix4f::Transform(position, rotation));
}

inline void NzPrimitive::MakePlane(const NzVector2f& size, const NzVector2ui& subdivision, const NzMatrix4f& transformMatrix)
{
	matrix = transformMatrix;
	type = nzPrimitiveType_Plane;
	plane.size = size;
	plane.subdivision = subdivision;
}

inline void NzPrimitive::MakePlane(const NzVector2f& size, const NzVector2ui& subdivision, const NzPlanef& planeInfo)
{
	MakePlane(size, subdivision, NzMatrix4f::Transform(planeInfo.distance * planeInfo.normal, NzQuaternionf::RotationBetween(NzVector3f::Up(), planeInfo.normal)));
}

inline void NzPrimitive::MakePlane(const NzVector2f& size, const NzVector2ui& subdivision, const NzVector3f& position, const NzQuaternionf& rotation)
{
	MakePlane(size, subdivision, NzMatrix4f::Transform(position, rotation));
}

inline void NzPrimitive::MakeUVSphere(float size, unsigned int sliceCount, unsigned int stackCount, const NzMatrix4f& transformMatrix)
{
	matrix = transformMatrix;
	type = nzPrimitiveType_Sphere;
	sphere.size = size;
	sphere.type = nzSphereType_UV;
	sphere.uv.sliceCount = sliceCount;
	sphere.uv.stackCount = stackCount;
}

inline void NzPrimitive::MakeUVSphere(float size, unsigned int sliceCount, unsigned int stackCount, const NzVector3f& position, const NzQuaternionf& rotation)
{
	MakeUVSphere(size, sliceCount, stackCount, NzMatrix4f::Transform(position, rotation));
}

inline NzPrimitive NzPrimitive::Box(const NzVector3f& lengths, const NzVector3ui& subdivision, const NzMatrix4f& transformMatrix)
{
	NzPrimitive primitive;
	primitive.MakeBox(lengths, subdivision, transformMatrix);

	return primitive;
}

inline NzPrimitive NzPrimitive::Box(const NzVector3f& lengths, const NzVector3ui& subdivision, const NzVector3f& position, const NzQuaternionf& rotation)
{
	NzPrimitive primitive;
	primitive.MakeBox(lengths, subdivision, position, rotation);

	return primitive;
}

inline NzPrimitive NzPrimitive::CubicSphere(float size, unsigned int subdivision, const NzMatrix4f& transformMatrix)
{
	NzPrimitive primitive;
	primitive.MakeCubicSphere(size, subdivision, transformMatrix);

	return primitive;
}

inline NzPrimitive NzPrimitive::CubicSphere(float size, unsigned int subdivision, const NzVector3f& position, const NzQuaternionf& rotation)
{
	NzPrimitive primitive;
	primitive.MakeCubicSphere(size, subdivision, position, rotation);

	return primitive;
}

inline NzPrimitive NzPrimitive::IcoSphere(float size, unsigned int recursionLevel, const NzMatrix4f& transformMatrix)
{
	NzPrimitive primitive;
	primitive.MakeIcoSphere(size, recursionLevel, transformMatrix);

	return primitive;
}

inline NzPrimitive NzPrimitive::IcoSphere(float size, unsigned int recursionLevel, const NzVector3f& position, const NzQuaternionf& rotation)
{
	NzPrimitive primitive;
	primitive.MakeIcoSphere(size, recursionLevel, position, rotation);

	return primitive;
}

inline NzPrimitive NzPrimitive::Plane(const NzVector2f& size, const NzVector2ui& subdivision, const NzMatrix4f& transformMatrix)
{
	NzPrimitive primitive;
	primitive.MakePlane(size, subdivision, transformMatrix);

	return primitive;
}

inline NzPrimitive NzPrimitive::Plane(const NzVector2f& size, const NzVector2ui& subdivision, const NzPlanef& plane)
{
	NzPrimitive primitive;
	primitive.MakePlane(size, subdivision, plane);

	return primitive;
}

inline NzPrimitive NzPrimitive::Plane(const NzVector2f& size, const NzVector2ui& subdivision, const NzVector3f& position, const NzQuaternionf& rotation)
{
	NzPrimitive primitive;
	primitive.MakePlane(size, subdivision, position, rotation);

	return primitive;
}

inline NzPrimitive NzPrimitive::UVSphere(float size, unsigned int sliceCount, unsigned int stackCount, const NzMatrix4f& transformMatrix)
{
	NzPrimitive primitive;
	primitive.MakeUVSphere(size, sliceCount, stackCount, transformMatrix);

	return primitive;
}

inline NzPrimitive NzPrimitive::UVSphere(float size, unsigned int sliceCount, unsigned int stackCount, const NzVector3f& position, const NzQuaternionf& rotation)
{
	NzPrimitive primitive;
	primitive.MakeUVSphere(size, sliceCount, stackCount, position, rotation);

	return primitive;
}

#include <Nazara/Core/DebugOff.hpp>
