// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Nz::PrimitiveList
	* \brief Core class that represents a geometric primitive
	*/

	/*!
	* \brief Makes a box centered
	*
	* \param lengths (Width, Height, Depht)
	* \param subdivision Number of subdivision for the axis
	* \param transformMatrix Matrix to apply
	* \param uvCoords Coordinates for texture
	*/
	inline void Primitive::MakeBox(const Vector3f& lengths, const Vector3ui& subdivision, const Matrix4f& transformMatrix, const Rectf& uvCoords)
	{
		matrix = transformMatrix;
		textureCoords = uvCoords;
		type = PrimitiveType_Box;
		box.lengths = lengths;
		box.subdivision = subdivision;
	}

	/*!
	* \brief Makes a box centered
	*
	* \param lengths (Width, Height, Depht)
	* \param subdivision Number of subdivision for the axis
	* \param position Position of the box
	* \param rotation Rotation of the box
	* \param uvCoords Coordinates for texture
	*/
	inline void Primitive::MakeBox(const Vector3f& lengths, const Vector3ui& subdivision, const Vector3f& position, const Quaternionf& rotation, const Rectf& uvCoords)
	{
		MakeBox(lengths, subdivision, Matrix4f::Transform(position, rotation), uvCoords);
	}

	/*!
	* \brief Makes a cone, centered in (0, 0, 0) and circle in (0, -length, 0)
	*
	* \param length Height of the cone
	* \param radius Width of the radius
	* \param subdivision Number of sides for the circle
	* \param transformMatrix Matrix to apply
	* \param uvCoords Coordinates for texture
	*/
	inline void Primitive::MakeCone(float length, float radius, unsigned int subdivision, const Matrix4f& transformMatrix, const Rectf& uvCoords)
	{
		matrix = transformMatrix;
		textureCoords = uvCoords;
		type = PrimitiveType_Cone;
		cone.length = length;
		cone.radius = radius;
		cone.subdivision = subdivision;
	}

	/*!
	* \brief Makes a cone, centered in (0, 0, 0) and circle in (0, -length, 0)
	*
	* \param length Height of the cone
	* \param radius Width of the radius
	* \param subdivision Number of sides for the circle
	* \param position Position of the cone
	* \param rotation Rotation of the cone
	* \param uvCoords Coordinates for texture
	*/
	inline void Primitive::MakeCone(float length, float radius, unsigned int subdivision, const Vector3f& position, const Quaternionf& rotation, const Rectf& uvCoords)
	{
		MakeCone(length, radius, subdivision, Matrix4f::Transform(position, rotation), uvCoords);
	}

	/*!
	* \brief Makes a cubic sphere, centered in (0, 0, 0)
	*
	* \param size Radius of the cubic sphere
	* \param subdivision Number of subdivision for the box
	* \param transformMatrix Matrix to apply
	* \param uvCoords Coordinates for texture
	*/
	inline void Primitive::MakeCubicSphere(float size, unsigned int subdivision, const Matrix4f& transformMatrix, const Rectf& uvCoords)
	{
		matrix = transformMatrix;
		textureCoords = uvCoords;
		type = PrimitiveType_Sphere;
		sphere.size = size;
		sphere.type = SphereType_Cubic;
		sphere.cubic.subdivision = subdivision;
	}

	/*!
	* \brief Adds a cubic sphere, centered in (0, 0, 0)
	*
	* \param size Radius of the cubic sphere
	* \param subdivision Number of subdivision for the box
	* \param position Position of the cubic sphere
	* \param rotation Rotation of the cubic sphere
	* \param uvCoords Coordinates for texture
	*/
	inline void Primitive::MakeCubicSphere(float size, unsigned int subdivision, const Vector3f& position, const Quaternionf& rotation, const Rectf& uvCoords)
	{
		MakeCubicSphere(size, subdivision, Matrix4f::Transform(position, rotation), uvCoords);
	}

	/*!
	* \brief Makes a icosphere, centered in (0, 0, 0)
	*
	* \param size Radius of the icosphere
	* \param recursionLevel Number of recursion for the icosphere
	* \param transformMatrix Matrix to apply
	* \param uvCoords Coordinates for texture
	*/
	inline void Primitive::MakeIcoSphere(float size, unsigned int recursionLevel, const Matrix4f& transformMatrix, const Rectf& uvCoords)
	{
		matrix = transformMatrix;
		textureCoords = uvCoords;
		type = PrimitiveType_Sphere;
		sphere.size = size;
		sphere.type = SphereType_Ico;
		sphere.ico.recursionLevel = recursionLevel;
	}

	/*!
	* \brief Makes a icosphere, centered in (0, 0, 0)
	*
	* \param size Radius of the sphere
	* \param recursionLevel Number of recursion for the icosphere
	* \param position Position of the icosphere
	* \param rotation Rotation of the icosphere
	* \param uvCoords Coordinates for texture
	*/
	inline void Primitive::MakeIcoSphere(float size, unsigned int recursionLevel, const Vector3f& position, const Quaternionf& rotation, const Rectf& uvCoords)
	{
		MakeIcoSphere(size, recursionLevel, Matrix4f::Transform(position, rotation), uvCoords);
	}

	/*!
	* \brief Makes a plane, centered in (0, 0, 0)
	*
	* \param size (Width, Depth)
	* \param subdivision Number of subdivision for the axis
	* \param transformMatrix Matrix to apply
	* \param uvCoords Coordinates for texture
	*/
	inline void Primitive::MakePlane(const Vector2f& size, const Vector2ui& subdivision, const Matrix4f& transformMatrix, const Rectf& uvCoords)
	{
		matrix = transformMatrix;
		textureCoords = uvCoords;
		type = PrimitiveType_Plane;
		plane.size = size;
		plane.subdivision = subdivision;
	}

	/*!
	* \brief Makes a plane, centered in (0, 0, 0)
	*
	* \param size (Width, Depth)
	* \param subdivision Number of subdivision for the axis
	* \param planeInfo Information for the plane
	* \param uvCoords Coordinates for texture
	*/
	inline void Primitive::MakePlane(const Vector2f& size, const Vector2ui& subdivision, const Planef& planeInfo, const Rectf& uvCoords)
	{
		MakePlane(size, subdivision, Matrix4f::Transform(planeInfo.distance * planeInfo.normal, Quaternionf::RotationBetween(Vector3f::Up(), planeInfo.normal)), uvCoords);
	}

	/*!
	* \brief Makes a plane, centered in (0, 0, 0)
	*
	* \param size (Width, Depth)
	* \param subdivision Number of subdivision for the axis
	* \param position Position of the plane
	* \param rotation Rotation of the plane
	* \param uvCoords Coordinates for texture
	*/
	inline void Primitive::MakePlane(const Vector2f& size, const Vector2ui& subdivision, const Vector3f& position, const Quaternionf& rotation, const Rectf& uvCoords)
	{
		MakePlane(size, subdivision, Matrix4f::Transform(position, rotation), uvCoords);
	}

	/*!
	* \brief Makes a UV sphere, centered in (0, 0, 0)
	*
	* \param size Radius of the sphere
	* \param sliceCount Number of slices
	* \param stackCount Number of stacks
	* \param transformMatrix Matrix to apply
	* \param uvCoords Coordinates for texture
	*/
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

	/*!
	* \brief Makes a UV sphere, centered in (0, 0, 0)
	*
	* \param size Radius of the sphere
	* \param sliceCount Number of slices
	* \param stackCount Number of stacks
	* \param position Position of the box
	* \param rotation Rotation of the box
	* \param uvCoords Coordinates for texture
	*/
	inline void Primitive::MakeUVSphere(float size, unsigned int sliceCount, unsigned int stackCount, const Vector3f& position, const Quaternionf& rotation, const Rectf& uvCoords)
	{
		MakeUVSphere(size, sliceCount, stackCount, Matrix4f::Transform(position, rotation), uvCoords);
	}

	/*!
	* \brief Creates a box centered
	* \return Primitive which is a box
	*
	* \param lengths (Width, Height, Depht)
	* \param subdivision Number of subdivision for the axis
	* \param transformMatrix Matrix to apply
	* \param uvCoords Coordinates for texture
	*/
	inline Primitive Primitive::Box(const Vector3f& lengths, const Vector3ui& subdivision, const Matrix4f& transformMatrix, const Rectf& uvCoords)
	{
		Primitive primitive;
		primitive.MakeBox(lengths, subdivision, transformMatrix, uvCoords);

		return primitive;
	}

	/*!
	* \brief Creates a box centered
	* \return Primitive which is a box
	*
	* \param lengths (Width, Height, Depht)
	* \param subdivision Number of subdivision for the axis
	* \param position Position of the box
	* \param rotation Rotation of the box
	* \param uvCoords Coordinates for texture
	*/
	inline Primitive Primitive::Box(const Vector3f& lengths, const Vector3ui& subdivision, const Vector3f& position, const Quaternionf& rotation, const Rectf& uvCoords)
	{
		Primitive primitive;
		primitive.MakeBox(lengths, subdivision, position, rotation, uvCoords);

		return primitive;
	}

	/*!
	* \brief Creates a cone, centered in (0, 0, 0) and circle in (0, -length, 0)
	* \return Primitive which is a cone
	*
	* \param length Height of the cone
	* \param radius Width of the radius
	* \param subdivision Number of sides for the circle
	* \param transformMatrix Matrix to apply
	* \param uvCoords Coordinates for texture
	*/
	inline Primitive Primitive::Cone(float length, float radius, unsigned int subdivision, const Matrix4f& transformMatrix, const Rectf& uvCoords)
	{
		Primitive primitive;
		primitive.MakeCone(length, radius, subdivision, transformMatrix, uvCoords);

		return primitive;
	}

	/*!
	* \brief Creates a cone, centered in (0, 0, 0) and circle in (0, -length, 0)
	* \return Primitive which is a cone
	*
	* \param length Height of the cone
	* \param radius Width of the radius
	* \param subdivision Number of sides for the circle
	* \param position Position of the cone
	* \param rotation Rotation of the cone
	* \param uvCoords Coordinates for texture
	*/
	inline Primitive Primitive::Cone(float length, float radius, unsigned int subdivision, const Vector3f& position, const Quaternionf& rotation, const Rectf& uvCoords)
	{
		Primitive primitive;
		primitive.MakeCone(length, radius, subdivision, position, rotation, uvCoords);

		return primitive;
	}

	/*!
	* \brief Creates a cubic sphere, centered in (0, 0, 0)
	* \return Primitive which is a cubic sphere
	*
	* \param size Radius of the cubic sphere
	* \param subdivision Number of subdivision for the box
	* \param transformMatrix Matrix to apply
	* \param uvCoords Coordinates for texture
	*/
	inline Primitive Primitive::CubicSphere(float size, unsigned int subdivision, const Matrix4f& transformMatrix, const Rectf& uvCoords)
	{
		Primitive primitive;
		primitive.MakeCubicSphere(size, subdivision, transformMatrix, uvCoords);

		return primitive;
	}

	/*!
	* \brief Creates a cubic sphere, centered in (0, 0, 0)
	* \return Primitive which is a cubic sphere
	*
	* \param size Radius of the cubic sphere
	* \param subdivision Number of subdivision for the box
	* \param position Position of the cubic sphere
	* \param rotation Rotation of the cubic sphere
	* \param uvCoords Coordinates for texture
	*/
	inline Primitive Primitive::CubicSphere(float size, unsigned int subdivision, const Vector3f& position, const Quaternionf& rotation, const Rectf& uvCoords)
	{
		Primitive primitive;
		primitive.MakeCubicSphere(size, subdivision, position, rotation, uvCoords);

		return primitive;
	}

	/*!
	* \brief Creates a icosphere, centered in (0, 0, 0)
	* \return Primitive which is a icosphere
	*
	* \param size Radius of the icosphere
	* \param recursionLevel Number of recursion for the icosphere
	* \param transformMatrix Matrix to apply
	* \param uvCoords Coordinates for texture
	*/
	inline Primitive Primitive::IcoSphere(float size, unsigned int recursionLevel, const Matrix4f& transformMatrix, const Rectf& uvCoords)
	{
		Primitive primitive;
		primitive.MakeIcoSphere(size, recursionLevel, transformMatrix, uvCoords);

		return primitive;
	}

	/*!
	* \brief Creates a icosphere, centered in (0, 0, 0)
	* \return Primitive which is a icosphere
	*
	* \param size Radius of the sphere
	* \param recursionLevel Number of recursion for the icosphere
	* \param position Position of the icosphere
	* \param rotation Rotation of the icosphere
	* \param uvCoords Coordinates for texture
	*/
	inline Primitive Primitive::IcoSphere(float size, unsigned int recursionLevel, const Vector3f& position, const Quaternionf& rotation, const Rectf& uvCoords)
	{
		Primitive primitive;
		primitive.MakeIcoSphere(size, recursionLevel, position, rotation, uvCoords);

		return primitive;
	}

	/*!
	* \brief Creates a plane, centered in (0, 0, 0)
	* \return Primitive which is a plane
	*
	* \param size (Width, Depth)
	* \param subdivision Number of subdivision for the axis
	* \param transformMatrix Matrix to apply
	* \param uvCoords Coordinates for texture
	*/
	inline Primitive Primitive::Plane(const Vector2f& size, const Vector2ui& subdivision, const Matrix4f& transformMatrix, const Rectf& uvCoords)
	{
		Primitive primitive;
		primitive.MakePlane(size, subdivision, transformMatrix, uvCoords);

		return primitive;
	}

	/*!
	* \brief Creates a plane, centered in (0, 0, 0)
	* \return Primitive which is a plane
	*
	* \param size (Width, Depth)
	* \param subdivision Number of subdivision for the axis
	* \param plane Information for the plane
	* \param uvCoords Coordinates for texture
	*/
	inline Primitive Primitive::Plane(const Vector2f& size, const Vector2ui& subdivision, const Planef& plane, const Rectf& uvCoords)
	{
		Primitive primitive;
		primitive.MakePlane(size, subdivision, plane, uvCoords);

		return primitive;
	}

	/*!
	* \brief Creates a plane, centered in (0, 0, 0)
	* \return Primitive which is a plane
	*
	* \param size (Width, Depth)
	* \param subdivision Number of subdivision for the axis
	* \param position Position of the plane
	* \param rotation Rotation of the plane
	* \param uvCoords Coordinates for texture
	*/
	inline Primitive Primitive::Plane(const Vector2f& size, const Vector2ui& subdivision, const Vector3f& position, const Quaternionf& rotation, const Rectf& uvCoords)
	{
		Primitive primitive;
		primitive.MakePlane(size, subdivision, position, rotation, uvCoords);

		return primitive;
	}

	/*!
	* \brief Creates a UV sphere, centered in (0, 0, 0)
	* \return Primitive which is a uv sphere
	*
	* \param size Radius of the sphere
	* \param sliceCount Number of slices
	* \param stackCount Number of stacks
	* \param transformMatrix Matrix to apply
	* \param uvCoords Coordinates for texture
	*/
	inline Primitive Primitive::UVSphere(float size, unsigned int sliceCount, unsigned int stackCount, const Matrix4f& transformMatrix, const Rectf& uvCoords)
	{
		Primitive primitive;
		primitive.MakeUVSphere(size, sliceCount, stackCount, transformMatrix, uvCoords);

		return primitive;
	}

	/*!
	* \brief Creates a UV sphere, centered in (0, 0, 0)
	* \return Primitive which is a uv sphere
	*
	* \param size Radius of the sphere
	* \param sliceCount Number of slices
	* \param stackCount Number of stacks
	* \param position Position of the box
	* \param rotation Rotation of the box
	* \param uvCoords Coordinates for texture
	*/
	inline Primitive Primitive::UVSphere(float size, unsigned int sliceCount, unsigned int stackCount, const Vector3f& position, const Quaternionf& rotation, const Rectf& uvCoords)
	{
		Primitive primitive;
		primitive.MakeUVSphere(size, sliceCount, stackCount, position, rotation, uvCoords);

		return primitive;
	}
}

#include <Nazara/Core/DebugOff.hpp>
