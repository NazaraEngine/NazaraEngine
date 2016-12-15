// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <Nazara/Math/Box.hpp>
#include <algorithm>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)

namespace Nz
{
	/*!
	* \ingroup math
	* \class Nz::Sphere
	* \brief Math class that represents a sphere "S2" in a three dimensional euclidean space
	*/

	/*!
	* \brief Constructs a Sphere object from its center position and radius
	*
	* \param X X position
	* \param Y Y position
	* \param Z Z position
	* \param Radius half of the diameter
	*/

	template<typename T>
	Sphere<T>::Sphere(T X, T Y, T Z, T Radius)
	{
		Set(X, Y, Z, Radius);
	}
	/*
	template<typename T>
	Sphere<T>::Sphere(const Circle<T>& circle)
	{
		Set(rect);
	}
	*/

	/*!
	* \brief Constructs a Sphere object from its position and radius
	*
	* \param center Center of the sphere
	* \param Radius Half of the diameter
	*/

	template<typename T>
	Sphere<T>::Sphere(const Vector3<T>& center, T Radius)
	{
		Set(center, Radius);
	}

	/*!
	* \brief Constructs a Sphere object from an array of four elements
	*
	* \param sphere[4] sphere[0] is X component, sphere[1] is Y component, sphere[2] is Z component and sphere[3] is radius
	*/

	template<typename T>
	Sphere<T>::Sphere(const T sphere[4])
	{
		Set(sphere);
	}

	/*!
	* \brief Constructs a Sphere object from another type of Sphere
	*
	* \param sphere Sphere of type U to convert to type T
	*/

	template<typename T>
	template<typename U>
	Sphere<T>::Sphere(const Sphere<U>& sphere)
	{
		Set(sphere);
	}

	/*!
	* \brief Tests whether the sphere contains the provided point inclusive of the edge of the sphere
	* \return true if inclusive
	*
	* \param X X position of the point
	* \param Y Y position of the point
	* \param Z Z position of the point
	*
	* \see Contains
	*/

	template<typename T>
	bool Sphere<T>::Contains(T X, T Y, T Z) const
	{
		return Contains(Vector3<T>(X, Y, Z));
	}

	/*!
	* \brief Tests whether the sphere contains the provided box inclusive of the edge of the sphere
	* \return true if all inclusive
	*
	* \param box Three dimensional box
	*
	* \see Contains
	*/

	template<typename T>
	bool Sphere<T>::Contains(const Box<T>& box) const
	{
		if (Contains(box.GetMinimum()) && Contains(box.GetMaximum()))
			return true;

		return false;
	}

	/*!
	* \brief Tests whether the sphere contains the provided point inclusive of the edge of the sphere
	* \return true if inclusive
	*
	* \param point Position of the point
	*/

	template<typename T>
	bool Sphere<T>::Contains(const Vector3<T>& point) const
	{
		return GetPosition().SquaredDistance(point) <= radius * radius;
	}

	/*!
	* \brief Returns the distance from the sphere to the point (is negative when the point is inside the sphere)
	* \return Distance to the point
	*
	* \param X X position of the point
	* \param Y Y position of the point
	* \param Z Z position of the point
	*/

	template<typename T>
	T Sphere<T>::Distance(T X, T Y, T Z) const
	{
		return Distance({X, Y, Z});
	}

	/*!
	* \brief Returns the distance from the sphere to the point (is negative when the point is inside the sphere)
	* \return Distance to the point
	*
	* \param point Position of the point
	*/

	template<typename T>
	T Sphere<T>::Distance(const Vector3<T>& point) const
	{
		return Vector3f::Distance(point, GetPosition()) - radius;
	}

	/*!
	* \brief Extends the sphere to contain the point in the boundary
	* \return A reference to this sphere extended
	*
	* \param X X position of the point
	* \param Y Y position of the point
	* \param Z Z position of the point
	*
	* \see ExtendTo
	*/

	template<typename T>
	Sphere<T>& Sphere<T>::ExtendTo(T X, T Y, T Z)
	{
		radius = std::max(radius, radius + Distance(X, Y, Z));

		return *this;
	}

	/*!
	* \brief Extends the sphere to contain the point in the boundary
	* \return A reference to this sphere extended
	*
	* \param point Position of the point
	*
	* \see ExtendTo
	*/

	template<typename T>
	Sphere<T>& Sphere<T>::ExtendTo(const Vector3<T>& point)
	{
		return ExtendTo(point.x, point.y, point.z);
	}

	/*!
	* \brief Computes the negative vertex of one direction
	* \return The position of the vertex on the sphere in the opposite way of the normal while considering the center
	*
	* \param normal Vector normalized indicating a direction
	*
	* \see GetPositiveVertex
	*/

	template<typename T>
	Vector3<T> Sphere<T>::GetNegativeVertex(const Vector3<T>& normal) const
	{
		Vector3<T> neg(GetPosition());
		neg -= normal * radius;

		return neg;
	}

	/*!
	* \brief Gets a Vector3 of the position
	* \return The position of the center of the sphere
	*/

	template<typename T>
	Vector3<T> Sphere<T>::GetPosition() const
	{
		return Vector3<T>(x, y, z);
	}

	/*!
	* \brief Computes the positive vertex of one direction
	* \return The position of the vertex on the sphere in the same way of the normal while considering the center
	*
	* \param normal Vector normalized indicating a direction
	*
	* \see GetNegativeVertex
	*/

	template<typename T>
	Vector3<T> Sphere<T>::GetPositiveVertex(const Vector3<T>& normal) const
	{
		Vector3<T> pos(GetPosition());
		pos += normal * radius;

		return pos;
	}

	/*!
	* \brief Checks whether or not this sphere intersects a box
	* \return true if the box intersects
	*
	* \param box Box to check
	*/

	template<typename T>
	bool Sphere<T>::Intersect(const Box<T>& box) const
	{
		// Arvo's algorithm.
		T squaredDistance = T(0.0);
		if (x < box.x)
		{
			T diff = x - box.x;
			squaredDistance += diff * diff;
		}
		else if (x > box.x + box.width)
		{
			T diff = x - (box.x + box.width);
			squaredDistance += diff * diff;
		}

		if (y < box.y)
		{
			T diff = y - box.y;
			squaredDistance += diff * diff;
		}
		else if (y > box.y + box.height)
		{
			T diff = y - (box.y + box.height);
			squaredDistance += diff * diff;
		}

		if (z < box.z)
		{
			T diff = z - box.z;
			squaredDistance += diff * diff;
		}
		else if (z > box.z + box.depth)
		{
			T diff = z - (box.z + box.depth);
			squaredDistance += diff * diff;
		}

		return squaredDistance <= radius * radius;
	}

	/*!
	* \brief Checks whether or not this sphere intersects another sphere
	* \return true if the spheres intersect or if one is in the other
	*
	* \param sphere Sphere to check
	*/

	template<typename T>
	bool Sphere<T>::Intersect(const Sphere& sphere) const
	{
		return GetPosition().SquaredDistance(Vector3<T>(sphere.x, sphere.y, sphere.z)) <= IntegralPow(radius + sphere.radius, 2);
	}

	/*!
	* \brief Checks whether this sphere is valid
	* \return true if the sphere has a strictly positive radius
	*/

	template<typename T>
	bool Sphere<T>::IsValid() const
	{
		return radius > F(0.0);
	}

	/*!
	* \brief Makes the sphere position (0, 0, 0) and radius 1
	* \return A reference to this vector with position (0, 0, 0) and radius 1
	*
	* \see Unit
	*/

	template<typename T>
	Sphere<T>& Sphere<T>::MakeUnit()
	{
		x = F(0.0);
		y = F(0.0);
		z = F(0.0);
		radius = F(1.0);

		return *this;
	}

	/*!
	* \brief Makes the sphere position (0, 0, 0) and radius 0
	* \return A reference to this vector with position (0, 0, 0) and radius 0
	*
	* \see Zero
	*/

	template<typename T>
	Sphere<T>& Sphere<T>::MakeZero()
	{
		x = F(0.0);
		y = F(0.0);
		z = F(0.0);
		radius = F(0.0);

		return *this;
	}

	/*!
	* \brief Sets the components of the sphere with center and radius
	* \return A reference to this sphere
	*
	* \param X X position
	* \param Y Y position
	* \param Z Z position
	* \param Radius half of the diameter
	*/

	template<typename T>
	Sphere<T>& Sphere<T>::Set(T X, T Y, T Z, T Radius)
	{
		x = X;
		y = Y;
		z = Z;
		radius = Radius;

		return *this;
	}

	/*!
	* \brief Sets the components of the sphere with center and radius
	* \return A reference to this sphere
	*
	* \param center Center of the sphere
	* \param Radius Half of the diameter
	*/

	template<typename T>
	Sphere<T>& Sphere<T>::Set(const Vector3<T>& center, T Radius)
	{
		x = center.x;
		y = center.y;
		z = center.z;
		radius = Radius;

		return *this;
	}
	/*
	template<typename T>
	Sphere<T>& Sphere<T>::Set(const Circle<T>& circle)
	{
		x = circle.x;
		y = circle.y;
		z = F(0.0);
		radius = circle.radius;

		return *this;
	}
	*/

	/*!
	* \brief Sets the components of the sphere with center and radius from another
	* \return A reference to this sphere
	*
	* \param sphere The other sphere
	*/

	template<typename T>
	Sphere<T>& Sphere<T>::Set(const Sphere& sphere)
	{
		std::memcpy(this, &sphere, sizeof(Sphere));

		return *this;
	}

	/*!
	* \brief Sets the components of the sphere from an array of four elements
	* \return A reference to this sphere
	*
	* \param sphere[4] sphere[0] is X position, sphere[1] is Y position, sphere[2] is Z position and sphere[3] is radius
	*/

	template<typename T>
	Sphere<T>& Sphere<T>::Set(const T sphere[4])
	{
		x = sphere[0];
		y = sphere[1];
		z = sphere[2];
		radius = sphere[3];

		return *this;
	}

	/*!
	* \brief Sets the components of the sphere from another type of Sphere
	* \return A reference to this sphere
	*
	* \param sphere Sphere of type U to convert its components
	*/

	template<typename T>
	template<typename U>
	Sphere<T>& Sphere<T>::Set(const Sphere<U>& sphere)
	{
		x = F(sphere.x);
		y = F(sphere.y);
		z = F(sphere.z);
		radius = F(sphere.radius);

		return *this;
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object: "Sphere(x, y, z; radius)"
	*/

	template<typename T>
	String Sphere<T>::ToString() const
	{
		StringStream ss;

		return ss << "Sphere(" << x << ", " << y << ", " << z << "; " << radius << ')';
	}

	/*!
	* \brief Returns the ith element of the sphere
	* \return A reference to the ith element of the sphere
	*
	* \remark Access to index greather than 4 is undefined behavior
	* \remark Produce a NazaraError if you try to acces to index greather than 4 with NAZARA_MATH_SAFE defined
	* \throw std::domain_error if NAZARA_MATH_SAFE is defined and one of you try to acces to index greather than 4
	*/

	template<typename T>
	T& Sphere<T>::operator[](unsigned int i)
	{
		#if NAZARA_MATH_SAFE
		if (i >= 4)
		{
			StringStream ss;
			ss << "Index out of range: (" << i << " >= 4)";

			NazaraError(ss);
			throw std::domain_error(ss.ToString());
		}
		#endif

		return *(&x+i);
	}

	/*!
	* \brief Returns the ith element of the sphere
	* \return A value to the ith element of the sphere
	*
	* \remark Access to index greather than 4 is undefined behavior
	* \remark Produce a NazaraError if you try to acces to index greather than 4 with NAZARA_MATH_SAFE defined
	* \throw std::domain_error if NAZARA_MATH_SAFE is defined and one of you try to acces to index greather than 4
	*/

	template<typename T>
	T Sphere<T>::operator[](unsigned int i) const
	{
		#if NAZARA_MATH_SAFE
		if (i >= 4)
		{
			StringStream ss;
			ss << "Index out of range: (" << i << " >= 4)";

			NazaraError(ss);
			throw std::domain_error(ss.ToString());
		}
		#endif

		return *(&x+i);
	}

	/*!
	* \brief Multiplies the radius of the sphere with a scalar
	* \return A sphere where the center is the same and radius is the product of this radius and the scalar
	*
	* \param scale The scalar to multiply radius with
	*/

	template<typename T>
	Sphere<T> Sphere<T>::operator*(T scalar) const
	{
		return Sphere(x, y, z, radius * scalar);
	}

	/*!
	* \brief Multiplies the radius of other sphere with a scalar
	* \return A reference to this sphere where the center is the same and radius is the product of this radius and the scalar
	*
	* \param scale The scalar to multiply radius with
	*/

	template<typename T>
	Sphere<T>& Sphere<T>::operator*=(T scalar)
	{
		radius *= scalar;
	}

	/*!
	* \brief Compares the sphere to other one
	* \return true if the spheres are the same
	*
	* \param sphere Other sphere to compare with
	*/

	template<typename T>
	bool Sphere<T>::operator==(const Sphere& sphere) const
	{
		return NumberEquals(x, sphere.x) && NumberEquals(y, sphere.y) && NumberEquals(z, sphere.z) &&
		       NumberEquals(radius, sphere.radius);
	}

	/*!
	* \brief Compares the sphere to other one
	* \return false if the spheres are the same
	*
	* \param sphere Other sphere to compare with
	*/

	template<typename T>
	bool Sphere<T>::operator!=(const Sphere& sphere) const
	{
		return !operator==(sphere);
	}

	/*!
	* \brief Shorthand for the sphere (0, 0, 0, 1)
	* \return A sphere with center (0, 0, 0) and radius 1
	*
	* \see MakeUnit
	*/

	template<typename T>
	Sphere<T> Sphere<T>::Unit()
	{
		Sphere sphere;
		sphere.MakeUnit();

		return sphere;
	}

	/*!
	* \brief Interpolates the sphere to other one with a factor of interpolation
	* \return A new sphere which is the interpolation of two spheres
	*
	* \param from Initial sphere
	* \param to Target sphere
	* \param interpolation Factor of interpolation
	*
	* \remark interpolation is meant to be between 0 and 1, other values are potentially undefined behavior
	* \remark With NAZARA_DEBUG, a NazaraError is thrown and Zero() is returned
	*
	* \see Lerp
	*/

	template<typename T>
	Sphere<T> Sphere<T>::Lerp(const Sphere& from, const Sphere& to, T interpolation)
	{
		#ifdef NAZARA_DEBUG
		if (interpolation < F(0.0) || interpolation > F(1.0))
		{
			NazaraError("Interpolation must be in range [0..1] (Got " + String::Number(interpolation) + ')');
			return Zero();
		}
		#endif

		Sphere sphere;
		sphere.x = Nz::Lerp(from.x, to.x, interpolation);
		sphere.y = Nz::Lerp(from.y, to.y, interpolation);
		sphere.z = Nz::Lerp(from.z, to.z, interpolation);
		sphere.radius = Nz::Lerp(from.radius, to.radius, interpolation);

		return sphere;
	}

	/*!
	* \brief Shorthand for the sphere (0, 0, 0, 0)
	* \return A sphere with center (0, 0, 0) and radius 0
	*
	* \see MakeZero
	*/

	template<typename T>
	Sphere<T> Sphere<T>::Zero()
	{
		Sphere sphere;
		sphere.MakeZero();

		return sphere;
	}

	/*!
	* \brief Serializes a Sphere
	* \return true if successfully serialized
	*
	* \param context Serialization context
	* \param sphere Input Sphere
	*/
	template<typename T>
	bool Serialize(SerializationContext& context, const Sphere<T>& sphere)
	{
		if (!Serialize(context, sphere.x))
			return false;

		if (!Serialize(context, sphere.y))
			return false;

		if (!Serialize(context, sphere.z))
			return false;

		if (!Serialize(context, sphere.radius))
			return false;

		return true;
	}

	/*!
	* \brief Unserializes a Sphere
	* \return true if successfully unserialized
	*
	* \param context Serialization context
	* \param sphere Output Sphere
	*/
	template<typename T>
	bool Unserialize(SerializationContext& context, Sphere<T>* sphere)
	{
		if (!Unserialize(context, &sphere->x))
			return false;

		if (!Unserialize(context, &sphere->y))
			return false;

		if (!Unserialize(context, &sphere->z))
			return false;

		if (!Unserialize(context, &sphere->radius))
			return false;

		return true;
	}
}

/*!
* \brief Output operator
* \return The stream
*
* \param out The stream
* \param sphere The sphere to output
*/

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nz::Sphere<T>& sphere)
{
	return out << sphere.ToString();
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
