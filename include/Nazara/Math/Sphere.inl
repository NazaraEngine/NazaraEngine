// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Math/Box.hpp>
#include <NazaraUtils/MathUtils.hpp>
#include <algorithm>
#include <cstring>
#include <sstream>

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
	constexpr Sphere<T>::Sphere(T X, T Y, T Z, T Radius) :
	x(X),
	y(Y),
	z(Z),
	radius(Radius)
	{
	}

	/*!
	* \brief Constructs a Sphere object from its position and radius
	*
	* \param center Center of the sphere
	* \param Radius Half of the diameter
	*/
	template<typename T>
	constexpr Sphere<T>::Sphere(const Vector3<T>& center, T Radius) :
	x(center.x),
	y(center.y),
	z(center.z),
	radius(Radius)
	{
	}

	/*!
	* \brief Constructs a Sphere object from an array of four elements
	*
	* \param sphere[4] sphere[0] is X component, sphere[1] is Y component, sphere[2] is Z component and sphere[3] is radius
	*/
	template<typename T>
	constexpr Sphere<T>::Sphere(const T sphere[4]) :
	x(sphere[0]),
	y(sphere[1]),
	z(sphere[2]),
	radius(sphere[3])
	{
	}

	/*!
	* \brief Constructs a Sphere object from another type of Sphere
	*
	* \param sphere Sphere of type U to convert to type T
	*/
	template<typename T>
	template<typename U>
	constexpr Sphere<T>::Sphere(const Sphere<U>& sphere) :
	x(static_cast<T>(sphere.x)),
	y(static_cast<T>(sphere.y)),
	z(static_cast<T>(sphere.z)),
	radius(static_cast<T>(sphere.radius))
	{
	}

	template<typename T>
	constexpr bool Sphere<T>::ApproxEqual(const Sphere& sphere, T maxDifference) const
	{
		return NumberEquals(x, sphere.x, maxDifference) && NumberEquals(y, sphere.y, maxDifference) && NumberEquals(z, sphere.z, maxDifference) && NumberEquals(radius, sphere.radius, maxDifference);
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
	constexpr bool Sphere<T>::Contains(T X, T Y, T Z, T epsilon) const
	{
		return Contains(Vector3<T>(X, Y, Z), epsilon);
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
	constexpr bool Sphere<T>::Contains(const Box<T>& box, T epsilon) const
	{
		if (Contains(box.GetMinimum(), epsilon) && Contains(box.GetMaximum(), epsilon))
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
	constexpr bool Sphere<T>::Contains(const Vector3<T>& point, T epsilon) const
	{
		return (GetPosition().SquaredDistance(point) - radius * radius) <= epsilon;
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
	constexpr Vector3<T> Sphere<T>::GetNegativeVertex(const Vector3<T>& normal) const
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
	constexpr Vector3<T> Sphere<T>::GetPosition() const
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
	constexpr Vector3<T> Sphere<T>::GetPositiveVertex(const Vector3<T>& normal) const
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
	constexpr bool Sphere<T>::Intersect(const Box<T>& box) const
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
	constexpr bool Sphere<T>::Intersect(const Sphere& sphere) const
	{
		return GetPosition().SquaredDistance(Vector3<T>(sphere.x, sphere.y, sphere.z)) <= IntegralPow(radius + sphere.radius, 2);
	}

	/*!
	* \brief Checks whether this sphere is valid
	* \return true if the sphere has a strictly positive radius
	*/
	template<typename T>
	constexpr bool Sphere<T>::IsValid() const
	{
		return radius > T(0.0);
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object: "Sphere(x, y, z; radius)"
	*/

	template<typename T>
	std::string Sphere<T>::ToString() const
	{
		std::ostringstream ss;
		ss << *this;

		return ss.str();
	}

	/*!
	* \brief Returns the ith element of the sphere
	* \return A reference to the ith element of the sphere
	*
	* \remark Access to index greater than 4 is undefined behavior
	*/
	template<typename T>
	constexpr T& Sphere<T>::operator[](std::size_t i)
	{
		NazaraAssert(i < 4, "Index out of range");

		return *(&x+i);
	}

	/*!
	* \brief Returns the ith element of the sphere
	* \return A value to the ith element of the sphere
	*
	* \remark Access to index greater than 4 is undefined behavior
	*/
	template<typename T>
	constexpr T Sphere<T>::operator[](std::size_t i) const
	{
		NazaraAssert(i < 4, "Index out of range");

		return *(&x+i);
	}

	/*!
	* \brief Multiplies the radius of the sphere with a scalar
	* \return A sphere where the center is the same and radius is the product of this radius and the scalar
	*
	* \param scalar The scalar to multiply radius with
	*/
	template<typename T>
	constexpr Sphere<T> Sphere<T>::operator*(T scalar) const
	{
		return Sphere(x, y, z, radius * scalar);
	}

	/*!
	* \brief Multiplies the radius of other sphere with a scalar
	* \return A reference to this sphere where the center is the same and radius is the product of this radius and the scalar
	*
	* \param scalar The scalar to multiply radius with
	*/
	template<typename T>
	constexpr Sphere<T>& Sphere<T>::operator*=(T scalar)
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
	constexpr bool Sphere<T>::operator==(const Sphere& sphere) const
	{
		return x == sphere.x && y == sphere.y && z == sphere.z && radius == sphere.radius;
	}

	/*!
	* \brief Compares the sphere to other one
	* \return false if the spheres are the same
	*
	* \param sphere Other sphere to compare with
	*/
	template<typename T>
	constexpr bool Sphere<T>::operator!=(const Sphere& sphere) const
	{
		return !operator==(sphere);
	}

	template<typename T>
	constexpr bool Sphere<T>::operator<(const Sphere& sphere) const
	{
		if (x != sphere.x)
			return x < sphere.x;

		if (y != sphere.y)
			return y < sphere.y;

		if (z != sphere.z)
			return z < sphere.z;

		return radius < sphere.radius;
	}

	template<typename T>
	constexpr bool Sphere<T>::operator<=(const Sphere& sphere) const
	{
		if (x != sphere.x)
			return x < sphere.x;

		if (y != sphere.y)
			return y < sphere.y;

		if (z != sphere.z)
			return z < sphere.z;

		return radius <= sphere.radius;
	}

	template<typename T>
	constexpr bool Sphere<T>::operator>(const Sphere& sphere) const
	{
		if (x != sphere.x)
			return x > sphere.x;

		if (y != sphere.y)
			return y > sphere.y;

		if (z != sphere.z)
			return z > sphere.z;

		return radius > sphere.radius;
	}

	template<typename T>
	constexpr bool Sphere<T>::operator>=(const Sphere& sphere) const
	{
		if (x != sphere.x)
			return x > sphere.x;

		if (y != sphere.y)
			return y > sphere.y;

		if (z != sphere.z)
			return z > sphere.z;

		return radius >= sphere.radius;
	}

	/*!
	* \brief Shorthand for the sphere (0, 0, 0, 1)
	* \return A sphere with center (0, 0, 0) and radius 1
	*/
	template<typename T>
	constexpr Sphere<T> Sphere<T>::Unit()
	{
		return Sphere(0, 0, 0, 1);
	}

	template<typename T>
	constexpr bool Sphere<T>::ApproxEqual(const Sphere& lhs, const Sphere& rhs, T maxDifference)
	{
		return lhs.ApproxEqual(rhs, maxDifference);
	}

	/*!
	* \brief Interpolates the sphere to other one with a factor of interpolation
	* \return A new sphere which is the interpolation of two spheres
	*
	* \param from Initial sphere
	* \param to Target sphere
	* \param interpolation Factor of interpolation
	*
	* \see Lerp
	*/
	template<typename T>
	constexpr Sphere<T> Sphere<T>::Lerp(const Sphere& from, const Sphere& to, T interpolation)
	{
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
	*/
	template<typename T>
	constexpr Sphere<T> Sphere<T>::Zero()
	{
		return Sphere(0, 0, 0, 0);
	}

	/*!
	* \brief Serializes a Sphere
	* \return true if successfully serialized
	*
	* \param context Serialization context
	* \param sphere Input Sphere
	*/
	template<typename T>
	bool Serialize(SerializationContext& context, const Sphere<T>& sphere, TypeTag<Sphere<T>>)
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
	bool Unserialize(SerializationContext& context, Sphere<T>* sphere, TypeTag<Sphere<T>>)
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
		return out << "Sphere(" << sphere.x << ", " << sphere.y << ", " << sphere.z << "; " << sphere.radius << ')';
	}
}

