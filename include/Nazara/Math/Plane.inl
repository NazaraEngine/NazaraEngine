// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/StringExt.hpp>
#include <cstring>
#include <sstream>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup math
	* \class Nz::Plane
	* \brief Math class that represents a plane in 3D
	*
	* \remark The convention used in this class is: If you ask for plane with normal (0, 1, 0) and distance 1, you will get 0 * X + 1 * Y + 0 * Z - 1 = 0 or Y = 1. Notice the sign minus before the distance on the left side of the equation
	*/

	/*!
	* \brief Constructs a Plane object from its components
	*
	* \param normalX X component of the normal
	* \param normalY Y component of the normal
	* \param normalZ Z component of the normal
	* \param D Distance to origin
	*/
	template<typename T>
	constexpr Plane<T>::Plane(T normalX, T normalY, T normalZ, T D) :
	normal(normalX, normalY, normalZ),
	distance(D)
	{
	}

	/*!
	* \brief Constructs a Plane object from an array of four elements
	*
	* \param plane[4] plane[0] is X component, plane[1] is Y component, plane[2] is Z component and plane[3] is D
	*/
	template<typename T>
	constexpr Plane<T>::Plane(const T plane[4]) :
	normal(plane[0], plane[1], plane[2]),
	distance(plane[3])
	{
	}

	/*!
	* \brief Constructs a Plane object from a normal and a distance
	*
	* \param Normal normal of the vector
	* \param D Distance to origin
	*/
	template<typename T>
	constexpr Plane<T>::Plane(const Vector3<T>& Normal, T D) :
	normal(Normal),
	distance(D)
	{
	}

	/*!
	* \brief Constructs a Plane object from a normal and a point
	*
	* \param Normal Normal of the plane
	* \param point Point which verifies the equation of the plane
	*/
	template<typename T>
	constexpr Plane<T>::Plane(const Vector3<T>& Normal, const Vector3<T>& point) :
	normal(Normal),
	distance(-Normal.DotProduct(point))
	{
	}

	/*!
	* \brief Constructs a Plane object from three points
	*
	* \param point1 First point
	* \param point2 Second point
	* \param point3 Third point
	*
	* \remark They are expected not to be colinear
	*/
	template<typename T>
	Plane<T>::Plane(const Vector3<T>& point1, const Vector3<T>& point2, const Vector3<T>& point3)
	{
		Vector3<T> edge1 = point2 - point1;
		Vector3<T> edge2 = point3 - point1;
		normal = edge1.CrossProduct(edge2);
		normal.Normalize();

		distance = -normal.DotProduct(point3);
	}

	/*!
	* \brief Constructs a Plane object from another type of Plane
	*
	* \param plane Plane of type U to convert to type T
	*/
	template<typename T>
	template<typename U>
	constexpr Plane<T>::Plane(const Plane<U>& plane) :
	normal(Vector3<T>(plane.normal)),
	distance(T(plane.distance))
	{
	}

	template<typename T>
	constexpr bool Plane<T>::ApproxEqual(const Plane& plane, T maxDifference) const
	{
		if (!normal.ApproxEqual(plane.normal, maxDifference))
			return false;

		return NumberEquals(distance, plane.distance, maxDifference);
	}

	template<typename T>
	Plane<T>& Plane<T>::Normalize(T* length)
	{
		T normalLength = normal.GetLength();
		normal /= normalLength;
		distance /= normalLength;

		if (length)
			*length = normalLength;

		return *this;
	}

	/*!
	* \brief Returns the distance from the plane to the point
	* \return Distance to the point
	*
	* \param point Position of the point
	*
	* \remark If T is negative, it means that the point is in the opposite direction of the normal
	*
	* \see Distance
	*/
	template<typename T>
	constexpr T Plane<T>::SignedDistance(const Vector3<T>& point) const
	{
		return normal.DotProduct(point) + distance; // ax + by + cz + d = 0
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object: "Plane(Normal: Vector3(x, y, z); Distance: w)"
	*/
	template<typename T>
	std::string Plane<T>::ToString() const
	{
		std::ostringstream ss;
		ss << *this;

		return ss.str();
	}

	/*!
	* \brief Compares the plane to other one
	* \return true if the planes are the same
	*
	* \param plane Other vector to compare with
	*
	* \remark Plane with normal N and distance D is the same than with normal -N et distance -D
	*/

	template<typename T>
	constexpr bool Plane<T>::operator==(const Plane& plane) const
	{
		return normal == plane.normal && distance == plane.distance;
	}

	/*!
	* \brief Compares the plane to other one
	* \return false if the planes are the same
	*
	* \param plane Other plane to compare with
	*
	* \remark Plane with normal N and distance D is the same than with normal -N et distance -D
	*/

	template<typename T>
	constexpr bool Plane<T>::operator!=(const Plane& plane) const
	{
		return !operator==(plane);
	}

	template<typename T>
	constexpr bool Plane<T>::ApproxEqual(const Plane& lhs, const Plane& rhs, T maxDifference)
	{
		return lhs.ApproxEqual(rhs, maxDifference);
	}

	/*!
	* \brief Intersects three planes to retrieve a single intersection point
	* \return The intersection point
	*
	* \param p0 First plane
	* \param p1 Second plane
	* \param p2 Third plane
	*
	* \remark All three planes must have differents normals otherwise result is undefined
	*/
	template<typename T>
	constexpr Vector3<T> Plane<T>::Intersect(const Plane& p0, const Plane& p1, const Plane& p2)
	{
		// From https://donw.io/post/frustum-point-extraction/
		Vector3f bxc = Vector3f::CrossProduct(p1.normal, p2.normal);
		Vector3f cxa = Vector3f::CrossProduct(p2.normal, p0.normal);
		Vector3f axb = Vector3f::CrossProduct(p0.normal, p1.normal);
		Vector3f r = -p0.distance * bxc - p1.distance * cxa - p2.distance * axb;

		return r * (T(1.0) / Vector3f::DotProduct(p0.normal, bxc));
	}

	/*!
	* \brief Interpolates the plane to other one with a factor of interpolation
	* \return A new plane which is the interpolation of two planes
	*
	* \param from Initial plane
	* \param to Target plane
	* \param interpolation Factor of interpolation
	*
	* \see Lerp
	*/
	template<typename T>
	constexpr Plane<T> Plane<T>::Lerp(const Plane& from, const Plane& to, T interpolation)
	{
		Plane plane;
		plane.distance = Nz::Lerp(from.distance, to.distance, interpolation);
		plane.normal = Vector3<T>::Lerp(from.normal, to.normal, interpolation);
		plane.normal.Normalize();

		return plane;
	}

	template<typename T>
	Plane<T> Plane<T>::Normalize(const Plane& plane, T* length)
	{
		Plane normalizedPlane(plane);
		normalizedPlane.Normalize(length);

		return normalizedPlane;
	}

	/*!
	* \brief Shorthand for the plane (0, 0, 1, 0)
	* \return A plane with components (0, 0, 1, 0)
	*/
	template<typename T>
	constexpr Plane<T> Plane<T>::XY()
	{
		return Plane(Vector3<T>::UnitZ(), 0);
	}

	/*!
	* \brief Shorthand for the plane (0, 1, 0, 0)
	* \return A plane with components (0, 1, 0, 0)
	*/
	template<typename T>
	constexpr Plane<T> Plane<T>::XZ()
	{
		return Plane(Vector3<T>::UnitY(), 0);
	}

	/*!
	* \brief Shorthand for the plane (1, 0, 0, 0)
	* \return A plane with components (1, 0, 0, 0)
	*/
	template<typename T>
	constexpr Plane<T> Plane<T>::YZ()
	{
		return Plane(Vector3<T>::UnitX(), 0);
	}

	/*!
	* \brief Serializes a Vector2
	* \return true if successfully serialized
	*
	* \param context Serialization context
	* \param plane Input Vector2
	*/
	template<typename T>
	bool Serialize(SerializationContext& context, const Plane<T>& plane, TypeTag<Plane<T>>)
	{
		if (!Serialize(context, plane.normal))
			return false;

		if (!Serialize(context, plane.distance))
			return false;

		return true;
	}

	/*!
	* \brief Unserializes a Plane
	* \return true if successfully unserialized
	*
	* \param context Serialization context
	* \param plane Output Plane
	*/
	template<typename T>
	bool Unserialize(SerializationContext& context, Plane<T>* plane, TypeTag<Plane<T>>)
	{
		if (!Unserialize(context, &plane->normal))
			return false;

		if (!Unserialize(context, &plane->distance))
			return false;

		return true;
	}

	/*!
	* \brief Output operator
	* \return The stream
	*
	* \param out The stream
	* \param plane The plane to output
	*/
	template<typename T>
	std::ostream& operator<<(std::ostream& out, const Nz::Plane<T>& plane)
	{
		return out << "Plane(Normal: " << plane.normal << "; Distance: " << plane.distance << ')';
	}
}

#include <Nazara/Core/DebugOff.hpp>
