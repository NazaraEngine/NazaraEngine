// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	inline void Primitive::MakeBox(const Vector3f& lengths, const Vector3ui& subdivision, const Matrix4f& transformMatrix, const Rectf& uvCoords)
	{
		matrix = transformMatrix;
		textureCoords = uvCoords;
		type = PrimitiveType_Box;
		box.lengths = lengths;
		box.subdivision = subdivision;
	}

	inline void Primitive::MakeBox(const Vector3f& lengths, const Vector3ui& subdivision, const Vector3f& position, const Quaternionf& rotation, const Rectf& uvCoords)
	{
		MakeBox(lengths, subdivision, Matrix4f::Transform(position, rotation), uvCoords);
	}

	inline void Primitive::MakeCone(float length, float radius, unsigned int subdivision, const Matrix4f& transformMatrix, const Rectf& uvCoords)
	{
		matrix = transformMatrix;
		textureCoords = uvCoords;
		type = PrimitiveType_Cone;
		cone.length = length;
		cone.radius = radius;
		cone.subdivision = subdivision;
	}

	inline void Primitive::MakeCone(float length, float radius, unsigned int subdivision, const Vector3f& position, const Quaternionf& rotation, const Rectf& uvCoords)
	{
		MakeCone(length, radius, subdivision, Matrix4f::Transform(position, rotation), uvCoords);
	}

	inline void Primitive::MakeCubicSphere(float size, unsigned int subdivision, const Matrix4f& transformMatrix, const Rectf& uvCoords)
	{
		matrix = transformMatrix;
		textureCoords = uvCoords;
		type = PrimitiveType_Sphere;
		sphere.size = size;
		sphere.type = SphereType_Cubic;
		sphere.cubic.subdivision = subdivision;
	}

	inline void Primitive::MakeCubicSphere(float size, unsigned int subdivision, const Vector3f& position, const Quaternionf& rotation, const Rectf& uvCoords)
	{
		MakeCubicSphere(size, subdivision, Matrix4f::Transform(position, rotation), uvCoords);
	}

	inline void Primitive::MakeIcoSphere(float size, unsigned int recursionLevel, const Matrix4f& transformMatrix, const Rectf& uvCoords)
	{
		matrix = transformMatrix;
		textureCoords = uvCoords;
		type = PrimitiveType_Sphere;
		sphere.size = size;
		sphere.type = SphereType_Ico;
		sphere.ico.recursionLevel = recursionLevel;
	}

	inline void Primitive::MakeIcoSphere(float size, unsigned int recursionLevel, const Vector3f& position, const Quaternionf& rotation, const Rectf& uvCoords)
	{
		MakeIcoSphere(size, recursionLevel, Matrix4f::Transform(position, rotation), uvCoords);
	}

	inline void Primitive::MakePlane(const Vector2f& size, const Vector2ui& subdivision, const Matrix4f& transformMatrix, const Rectf& uvCoords)
	{
		matrix = transformMatrix;
		textureCoords = uvCoords;
		type = PrimitiveType_Plane;
		plane.size = size;
		plane.subdivision = subdivision;
	}

	inline void Primitive::MakePlane(const Vector2f& size, const Vector2ui& subdivision, const Planef& planeInfo, const Rectf& uvCoords)
	{
		MakePlane(size, subdivision, Matrix4f::Transform(planeInfo.distance * planeInfo.normal, Quaternionf::RotationBetween(Vector3f::Up(), planeInfo.normal)), uvCoords);
	}

	inline void Primitive::MakePlane(const Vector2f& size, const Vector2ui& subdivision, const Vector3f& position, const Quaternionf& rotation, const Rectf& uvCoords)
	{
		MakePlane(size, subdivision, Matrix4f::Transform(position, rotation), uvCoords);
	}

	inline void Primitive::MakeUVSphere(float size, unsigned int sliceCount, unsigned int stackCount, const Matrix4f& transformMatrix, const Rectf& uvCoords)
	{
		matrix = transformMatrix;
		textureCoords = uvCoords;
		type = PrimitiveType_Sphere;
		sphere.size = size;
		sphere.type = SphereType_UV;
		sphere.uv.sliceCount = sliceCount;
		sphere.uv.stackCount = stackCount;
	}

	inline void Primitive::MakeUVSphere(float size, unsigned int sliceCount, unsigned int stackCount, const Vector3f& position, const Quaternionf& rotation, const Rectf& uvCoords)
	{
		MakeUVSphere(size, sliceCount, stackCount, Matrix4f::Transform(position, rotation), uvCoords);
	}

	inline Primitive Primitive::Box(const Vector3f& lengths, const Vector3ui& subdivision, const Matrix4f& transformMatrix, const Rectf& uvCoords)
	{
		Primitive primitive;
		primitive.MakeBox(lengths, subdivision, transformMatrix, uvCoords);

		return primitive;
	}

	inline Primitive Primitive::Box(const Vector3f& lengths, const Vector3ui& subdivision, const Vector3f& position, const Quaternionf& rotation, const Rectf& uvCoords)
	{
		Primitive primitive;
		primitive.MakeBox(lengths, subdivision, position, rotation, uvCoords);

		return primitive;
	}

	inline Primitive Primitive::Cone(float length, float radius, unsigned int subdivision, const Matrix4f& transformMatrix, const Rectf& uvCoords)
	{
		Primitive primitive;
		primitive.MakeCone(length, radius, subdivision, transformMatrix, uvCoords);

		return primitive;
	}

	inline Primitive Primitive::Cone(float length, float radius, unsigned int subdivision, const Vector3f& position, const Quaternionf& rotation, const Rectf& uvCoords)
	{
		Primitive primitive;
		primitive.MakeCone(length, radius, subdivision, position, rotation, uvCoords);

		return primitive;
	}

	inline Primitive Primitive::CubicSphere(float size, unsigned int subdivision, const Matrix4f& transformMatrix, const Rectf& uvCoords)
	{
		Primitive primitive;
		primitive.MakeCubicSphere(size, subdivision, transformMatrix, uvCoords);

		return primitive;
	}

	inline Primitive Primitive::CubicSphere(float size, unsigned int subdivision, const Vector3f& position, const Quaternionf& rotation, const Rectf& uvCoords)
	{
		Primitive primitive;
		primitive.MakeCubicSphere(size, subdivision, position, rotation, uvCoords);

		return primitive;
	}

	inline Primitive Primitive::IcoSphere(float size, unsigned int recursionLevel, const Matrix4f& transformMatrix, const Rectf& uvCoords)
	{
		Primitive primitive;
		primitive.MakeIcoSphere(size, recursionLevel, transformMatrix, uvCoords);

		return primitive;
	}

	inline Primitive Primitive::IcoSphere(float size, unsigned int recursionLevel, const Vector3f& position, const Quaternionf& rotation, const Rectf& uvCoords)
	{
		Primitive primitive;
		primitive.MakeIcoSphere(size, recursionLevel, position, rotation, uvCoords);

		return primitive;
	}

	inline Primitive Primitive::Plane(const Vector2f& size, const Vector2ui& subdivision, const Matrix4f& transformMatrix, const Rectf& uvCoords)
	{
		Primitive primitive;
		primitive.MakePlane(size, subdivision, transformMatrix, uvCoords);

		return primitive;
	}

	inline Primitive Primitive::Plane(const Vector2f& size, const Vector2ui& subdivision, const Planef& plane, const Rectf& uvCoords)
	{
		Primitive primitive;
		primitive.MakePlane(size, subdivision, plane, uvCoords);

		return primitive;
	}

	inline Primitive Primitive::Plane(const Vector2f& size, const Vector2ui& subdivision, const Vector3f& position, const Quaternionf& rotation, const Rectf& uvCoords)
	{
		Primitive primitive;
		primitive.MakePlane(size, subdivision, position, rotation, uvCoords);

		return primitive;
	}

	inline Primitive Primitive::UVSphere(float size, unsigned int sliceCount, unsigned int stackCount, const Matrix4f& transformMatrix, const Rectf& uvCoords)
	{
		Primitive primitive;
		primitive.MakeUVSphere(size, sliceCount, stackCount, transformMatrix, uvCoords);

		return primitive;
	}

	inline Primitive Primitive::UVSphere(float size, unsigned int sliceCount, unsigned int stackCount, const Vector3f& position, const Quaternionf& rotation, const Rectf& uvCoords)
	{
		Primitive primitive;
		primitive.MakeUVSphere(size, sliceCount, stackCount, position, rotation, uvCoords);

		return primitive;
	}
}

#include <Nazara/Core/DebugOff.hpp>
