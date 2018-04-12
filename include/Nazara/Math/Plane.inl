// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)

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
	Plane<T>::Plane(T normalX, T normalY, T normalZ, T D)
	{
		Set(normalX, normalY, normalZ, D);
	}

	/*!
	* \brief Constructs a Plane object from an array of four elements
	*
	* \param plane[4] plane[0] is X component, plane[1] is Y component, plane[2] is Z component and plane[3] is D
	*/

	template<typename T>
	Plane<T>::Plane(const T plane[4])
	{
		Set(plane);
	}

	/*!
	* \brief Constructs a Plane object from a normal and a distance
	*
	* \param Normal normal of the vector
	* \param D Distance to origin
	*/

	template<typename T>
	Plane<T>::Plane(const Vector3<T>& Normal, T D)
	{
		Set(Normal, D);
	}

	/*!
	* \brief Constructs a Plane object from a normal and a point
	*
	* \param Normal Normal of the plane
	* \param point Point which verifies the equation of the plane
	*/

	template<typename T>
	Plane<T>::Plane(const Vector3<T>& Normal, const Vector3<T>& point)
	{
		Set(Normal, point);
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
		Set(point1, point2, point3);
	}

	/*!
	* \brief Constructs a Plane object from another type of Plane
	*
	* \param plane Plane of type U to convert to type T
	*/

	template<typename T>
	template<typename U>
	Plane<T>::Plane(const Plane<U>& plane)
	{
		Set(plane);
	}

	/*!
	* \brief Returns the distance from the plane to the point
	* \return Distance to the point
	*
	* \param x X position of the point
	* \param y Y position of the point
	* \param z Z position of the point
	*
	* \remark If T is negative, it means that the point is in the opposite direction of the normal
	*
	* \see Distance
	*/

	template<typename T>
	T Plane<T>::Distance(T x, T y, T z) const
	{
		return Distance(Vector3<T>(x, y, z));
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
	T Plane<T>::Distance(const Vector3<T>& point) const
	{
		return normal.DotProduct(point) - distance; // ax + by + cd - d = 0.
	}

	/*!
	* \brief Makes the plane (0, 0, 1, 0)
	* \return A reference to this plane with components (0, 0, 1, 0)
	*
	* \see XY
	*/

	template<typename T>
	Plane<T>& Plane<T>::MakeXY()
	{
		return Set(F(0.0), F(0.0), F(1.0), F(0.0));
	}

	/*!
	* \brief Makes the plane (0, 1, 0, 0)
	* \return A reference to this plane with components (0, 1, 0, 0)
	*
	* \see XZ
	*/

	template<typename T>
	Plane<T>& Plane<T>::MakeXZ()
	{
		return Set(F(0.0), F(1.0), F(0.0), F(0.0));
	}

	/*!
	* \brief Makes the plane (1, 0, 0, 0)
	* \return A reference to this plane with components (1, 0, 0, 0)
	*
	* \see YZ
	*/

	template<typename T>
	Plane<T>& Plane<T>::MakeYZ()
	{
		return Set(F(1.0), F(0.0), F(0.0), F(0.0));
	}

	/*!
	* \brief Sets the components of the plane
	* \return A reference to this plane
	*
	* \param normalX X component of the normal
	* \param normalY Y component of the normal
	* \param normalZ Z component of the normal
	* \param D Distance to origin
	*/

	template<typename T>
	Plane<T>& Plane<T>::Set(T normalX, T normalY, T normalZ, T D)
	{
		distance = D;
		normal.Set(normalX, normalY, normalZ);

		return *this;
	}

	/*!
	* \brief Sets the components of the plane from an array of four elements
	* \return A reference to this plane
	*
	* \param plane[4] plane[0] is X component, plane[1] is Y component, plane[2] is Z component and plane[3] is D
	*/

	template<typename T>
	Plane<T>& Plane<T>::Set(const T plane[4])
	{
		normal.Set(plane[0], plane[1], plane[2]);
		distance = plane[3];

		return *this;
	}

	/*!
	* \brief Sets the components of the plane from another plane
	* \return A reference to this plane
	*
	* \param plane The other plane
	*/

	template<typename T>
	Plane<T>& Plane<T>::Set(const Plane& plane)
	{
		std::memcpy(this, &plane, sizeof(Plane));

		return *this;
	}

	/*!
	* \brief Sets the components of the plane from a normal and a distance
	* \return A reference to this plane
	*
	* \param Normal Normal of the vector
	* \param D Distance to origin
	*/

	template<typename T>
	Plane<T>& Plane<T>::Set(const Vector3<T>& Normal, T D)
	{
		distance = D;
		normal = Normal;

		return *this;
	}

	/*!
	* \brief Sets the components of the plane from a normal and a point
	* \return A reference to this plane
	*
	* \param Normal Normal of the plane
	* \param point Point which verifies the equation of the plane
	*/

	template<typename T>
	Plane<T>& Plane<T>::Set(const Vector3<T>& Normal, const Vector3<T>& point)
	{
		normal = Normal;
		distance = -normal.DotProduct(point);

		return *this;
	}

	/*!
	* \brief Sets the components of the plane from three points
	* \return A reference to this plane
	*
	* \param point1 First point
	* \param point2 Second point
	* \param point3 Third point
	*
	* \remark They are expected not to be colinear
	*/

	template<typename T>
	Plane<T>& Plane<T>::Set(const Vector3<T>& point1, const Vector3<T>& point2, const Vector3<T>& point3)
	{
		Vector3<T> edge1 = point2 - point1;
		Vector3<T> edge2 = point3 - point1;
		normal = edge1.CrossProduct(edge2);
		normal.Normalize();

		distance = normal.DotProduct(point3);

		return *this;
	}

	/*!
	* \brief Sets the components of the plane from another type of Plane
	* \return A reference to this plane
	*
	* \param plane Plane of type U to convert its components
	*/

	template<typename T>
	template<typename U>
	Plane<T>& Plane<T>::Set(const Plane<U>& plane)
	{
		normal.Set(plane.normal);
		distance = F(plane.distance);

		return *this;
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object: "Plane(Normal: Vector3(x, y, z); Distance: w)"
	*/

	template<typename T>
	String Plane<T>::ToString() const
	{
		StringStream ss;

		return ss << "Plane(Normal: " << normal.ToString() << "; Distance: " << distance << ')';
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
	bool Plane<T>::operator==(const Plane& plane) const
	{
		return (normal == plane.normal && NumberEquals(distance, plane.distance)) || (normal == -plane.normal && NumberEquals(distance, -plane.distance));
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
	bool Plane<T>::operator!=(const Plane& plane) const
	{
		return !operator==(plane);
	}

	/*!
	* \brief Interpolates the plane to other one with a factor of interpolation
	* \return A new plane which is the interpolation of two planes
	*
	* \param from Initial plane
	* \param to Target plane
	* \param interpolation Factor of interpolation
	*
	* \remark interpolation is meant to be between 0 and 1, other values are potentially undefined behavior
	* \remark With NAZARA_DEBUG, a NazaraError is thrown and Plane() is returned
	*
	* \see Lerp
	*/

	template<typename T>
	Plane<T> Plane<T>::Lerp(const Plane& from, const Plane& to, T interpolation)
	{
		#ifdef NAZARA_DEBUG
		if (interpolation < F(0.0) || interpolation > F(1.0))
		{
			NazaraError("Interpolation must be in range [0..1] (Got " + String::Number(interpolation) + ')');
			return Plane();
		}
		#endif

		Plane plane;
		plane.distance = Nz::Lerp(from.distance, to.distance, interpolation);
		plane.normal = Vector3<T>::Lerp(from.normal, to.normal, interpolation);
		plane.normal.Normalize();

		return plane;
	}

	/*!
	* \brief Shorthand for the plane (0, 0, 1, 0)
	* \return A plane with components (0, 0, 1, 0)
	*
	* \see MakeXY
	*/

	template<typename T>
	Plane<T> Plane<T>::XY()
	{
		Plane plane;
		plane.MakeXY();

		return plane;
	}

	/*!
	* \brief Shorthand for the plane (0, 1, 0, 0)
	* \return A plane with components (0, 1, 0, 0)
	*
	* \see MakeXZ
	*/

	template<typename T>
	Plane<T> Plane<T>::XZ()
	{
		Plane plane;
		plane.MakeXZ();

		return plane;
	}

	/*!
	* \brief Shorthand for the plane (1, 0, 0, 0)
	* \return A plane with components (1, 0, 0, 0)
	*
	* \see MakeYZ
	*/

	template<typename T>
	Plane<T> Plane<T>::YZ()
	{
		Plane plane;
		plane.MakeYZ();

		return plane;
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
	return out << plane.ToString();
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
