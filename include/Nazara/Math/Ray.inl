// Copyright (C) 2024 Gawaboumga (https://github.com/Gawaboumga) - Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <NazaraUtils/MathUtils.hpp>
#include <limits>
#include <sstream>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup math
	* \class Nz::Ray
	* \brief Math class that represents a ray or a straight line in 3D space
	*
	* This ray is meant to be understood like origin + lambda * direction, where lambda is a real positive parameter
	*/

	/*!
	* \brief Constructs a Ray object from its position and direction
	*
	* \param X X position
	* \param Y Y position
	* \param Z Z position
	* \param DirectionX X component of the vector direction
	* \param DirectionY Y component of the vector direction
	* \param DirectionY Y component of the vector direction
	*/
	template<typename T>
	constexpr Ray<T>::Ray(T X, T Y, T Z, T DirectionX, T DirectionY, T DirectionZ) :
	direction(DirectionX, DirectionY, DirectionZ),
	origin(X, Y, Z)
	{
	}

	/*!
	* \brief Constructs a Ray object from two Vector3
	*
	* \param Origin Vector which represents the origin of the ray
	* \param Direction Vector which represents the direction of the ray
	*/
	template<typename T>
	constexpr Ray<T>::Ray(const Vector3<T>& Origin, const Vector3<T>& Direction) :
	direction(Direction),
	origin(Origin)
	{
	}

	/*!
	* \brief Constructs a Ray object from two arrays of three elements
	*
	* \param Origin[3] Origin[0] is X position, Origin[1] is Y position and Origin[2] is Z position
	* \param Direction[3] Direction[0] is X direction, Direction[1] is Y direction and Direction[2] is Z direction
	*/
	template<typename T>
	constexpr Ray<T>::Ray(const T Origin[3], const T Direction[3]) :
	direction(Direction),
	origin(Origin)
	{
	}

	/*!
	* \brief Constructs a Ray object from the intersection of two planes
	*
	* \param planeOne First plane
	* \param planeTwo Second secant plane
	*
	* \remark Produce a NazaraError if planes are parallel with NAZARA_MATH_SAFE defined
	* \throw std::domain_error if NAZARA_MATH_SAFE is defined and planes are parallel
	*/
	template<typename T>
	Ray<T>::Ray(const Plane<T>& planeOne, const Plane<T>& planeTwo)
	{
		T termOne = planeOne.normal.GetLength();
		T termTwo = planeOne.normal.DotProduct(planeTwo.normal);
		T termFour = planeTwo.normal.GetLength();
		T det = termOne * termFour - termTwo * termTwo;

		#if NAZARA_MATH_SAFE
		if (NumberEquals(det, T(0.0)))
		{
			std::string error("Planes are parallel");

			NazaraError(error);
			throw std::domain_error(error);
		}
		#endif

		T invdet = T(1.0) / det;
		T fc0 = (termFour * -planeOne.distance + termTwo * planeTwo.distance) * invdet;
		T fc1 = (termOne * -planeTwo.distance + termTwo * planeOne.distance) * invdet;

		direction = planeOne.normal.CrossProduct(planeTwo.normal);
		origin = planeOne.normal * fc0 + planeTwo.normal * fc1;
	}

	/*!
	* \brief Constructs a Ray object from another type of Ray
	*
	* \param ray Ray of type U to convert to type T
	*/

	template<typename T>
	template<typename U>
	constexpr Ray<T>::Ray(const Ray<U>& ray) :
	direction(ray.direction),
	origin(ray.origin)
	{
	}

	/*!
	* \brief Constructs a Ray object from two Vector3 from another type of Ray
	*
	* \param Origin Origin of type U to convert to type T
	* \param Direction Direction of type U to convert to type T
	*/

	template<typename T>
	template<typename U>
	constexpr Ray<T>::Ray(const Vector3<U>& Origin, const Vector3<U>& Direction) :
	direction(Direction),
	origin(Origin)
	{
	}

	template<typename T>
	constexpr bool Ray<T>::ApproxEqual(const Ray& ray, T maxDifference) const
	{
		return direction.ApproxEqual(ray.direction, maxDifference) && origin.ApproxEqual(ray.origin, maxDifference);
	}

	/*!
	* \brief Finds the closest point of the ray from point
	* \return The parameter where the point along this ray that is closest to the point provided
	*
	* \param point The point to get the closest approach to
	*/
	template<typename T>
	constexpr T Ray<T>::ClosestPoint(const Vector3<T>& point) const
	{
		Vector3<T> delta = point - origin;
		T vsq = direction.GetSquaredLength();
		T proj = delta.DotProduct(direction);

		return proj / vsq;
	}

	/*!
	* \brief Gets the point along the ray for this parameter
	* \return The point on the ray
	*
	* \param lambda Parameter to obtain a particular point on the ray
	*/
	template<typename T>
	constexpr Vector3<T> Ray<T>::GetPoint(T lambda) const
	{
		return origin + lambda * direction;
	}

	/*!
	* \brief Checks whether or not this ray intersects with the BoundingVolume
	* \return true if it intersects
	*
	* \param volume BoundingVolume to check
	* \param closestHit Optional argument to get the closest parameter where the intersection is only if it happened
	* \param furthestHit Optional argument to get the furthest parameter where the intersection is only if it happened
	*
	* \remark If BoundingVolume is Extent::Infinite, then closestHit and furthestHit are equal to 0 et infinity
	* \remark If BoundingVolume is Extent::Null, then closestHit and furthestHit are unchanged
	* \remark If enumeration of BoundingVolume is not defined in Extent, a NazaraError is thrown and closestHit and furthestHit are unchanged
	*
	* \see Intersect
	*/
	template<typename T>
	constexpr bool Ray<T>::Intersect(const BoundingVolume<T>& volume, T* closestHit, T* furthestHit) const
	{
		switch (volume.extent)
		{
			case Extent::Finite:
			{
				if (Intersect(volume.aabb))
					return true; // TODO: Should test OBB but there's currently no way of doing so (OrientedBox don't store enough info?)

				return false;
			}

			case Extent::Infinite:
			{
				if (closestHit)
					*closestHit = T(0.0);

				if (furthestHit)
					*furthestHit = std::numeric_limits<T>::infinity();

				return true;
			}

			case Extent::Null:
				return false;
		}

		NazaraErrorFmt("invalid extent type ({0:#x})", UnderlyingCast(volume.extent));
		return false;
	}

	/*!
	* \brief Checks whether or not this ray intersects with the Box
	* \return true if it intersects
	*
	* \param box Box to check
	* \param closestHit Optional argument to get the closest parameter where the intersection is only if it happened
	* \param furthestHit Optional argument to get the furthest parameter where the intersection is only if it happened
	*
	* \see Intersect
	*/
	template<typename T>
	constexpr bool Ray<T>::Intersect(const Box<T>& box, T* closestHit, T* furthestHit) const
	{
		// https://tavianator.com/2015/ray_box_nan.html
		Vector3<T> boxMin = box.GetMinimum();
		Vector3<T> boxMax = box.GetMaximum();

		T t1 = (boxMin[0] - origin[0]) / direction[0];
		T t2 = (boxMax[0] - origin[0]) / direction[0];

		T tmin = std::min(t1, t2);
		T tmax = std::max(t1, t2);

		for (unsigned int i = 1; i < 3; ++i)
		{
			t1 = (boxMin[i] - origin[i]) / direction[i];
			t2 = (boxMax[i] - origin[i]) / direction[i];

			tmin = std::max(tmin, std::min(t1, t2));
			tmax = std::min(tmax, std::max(t1, t2));
		}

		tmin = std::max(tmin, T(0.0));

		if (closestHit)
			*closestHit = tmin;

		if (furthestHit)
			*furthestHit = tmax;

		return tmax > tmin;
	}

	/*!
	* \brief Checks whether or not this ray intersects with the transform Matrix4 applied to the Box
	* \return true if it intersects
	*
	* \param box Box to check
	* \param transform Matrix4 which represents the transformation of the box
	* \param closestHit Optional argument to get the closest parameter where the intersection is only if it happened
	* \param furthestHit Optional argument to get the furthest parameter where the intersection is only if it happened
	*
	* \see Intersect
	*/
	template<typename T>
	constexpr bool Ray<T>::Intersect(const Box<T>& box, const Matrix4<T>& transform, T* closestHit, T* furthestHit) const
	{
		// http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-custom-ray-obb-function/
		// Intersection method from Real-Time Rendering and Essential Mathematics for Games
		T tMin = T(0.0);
		T tMax = std::numeric_limits<T>::infinity();

		Vector3<T> boxMin = box.GetMinimum();
		Vector3<T> boxMax = box.GetMaximum();
		Vector3<T> delta = transform.GetTranslation() - origin;

		// Test intersection with the 2 planes perpendicular to the OBB's X axis
		for (unsigned int i = 0; i < 3; ++i)
		{
			Vector3<T> axis(transform(0, i), transform(1, i), transform(2, i));
			T e = axis.DotProduct(delta);
			T f = direction.DotProduct(axis);

			if (!NumberEquals(f, T(0.0)))
			{
				T t1 = (e + boxMin[i]) / f; // Intersection with the "left" plane
				T t2 = (e + boxMax[i]) / f; // Intersection with the "right" plane
				// t1 and t2 now contain distances betwen ray origin and ray-plane intersections

				// We want t1 to represent the nearest intersection,
				// so if it's not the case, invert t1 and t2
				if (t1 > t2)
					std::swap(t1, t2);

				// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
				if (t2 < tMax)
					tMax = t2;

				// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
				if (t1 > tMin)
					tMin = t1;

				// And here's the trick :
				// If "far" is closer than "near", then there is NO intersection.
				if (tMax < tMin)
					return false;
			}
			else
				// Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
				if (-e + boxMin[i] > T(0.0) || -e + boxMax[i] < T(0.0))
					return false;
		}

		if (closestHit)
			*closestHit = tMin;

		if (furthestHit)
			*furthestHit = tMax;

		return true;
	}

	/*!
	* \brief Checks whether or not this ray intersects with the plane
	* \return true if it intersects
	*
	* \param plane Plane to check
	* \param hit Optional argument to get the parameter where the intersection is only if it happened
	*
	* \see Intersect
	*/
	template<typename T>
	constexpr bool Ray<T>::Intersect(const Plane<T>& plane, T* hit) const
	{
		T divisor = plane.normal.DotProduct(direction);
		if (NumberEquals(divisor, T(0.0)))
			return false; // Perpendicular

		T lambda = -(plane.normal.DotProduct(origin) - plane.distance) / divisor; // The plane is ax + by + cz = d
		if (lambda < T(0.0))
			return false; // The plane is 'behind' the ray.

		if (hit)
			*hit = lambda;

		return true;
	}

	/*!
	* \brief Checks whether or not this ray intersects with the sphere
	* \return true if it intersects
	*
	* \param sphere Sphere to check
	* \param closestHit Optional argument to get the closest parameter where the intersection is only if it happened
	* \param furthestHit Optional argument to get the furthest parameter where the intersection is only if it happened
	*
	* \see Intersect
	*/
	template<typename T>
	constexpr bool Ray<T>::Intersect(const Sphere<T>& sphere, T* closestHit, T* furthestHit) const
	{
		Vector3<T> sphereRay = sphere.GetPosition() - origin;
		T length = sphereRay.DotProduct(direction);

		if (length < T(0.0))
			return false; // ray is perpendicular to the vector origin - center

		T squaredDistance = sphereRay.GetSquaredLength() - length * length;
		T squaredRadius = sphere.radius * sphere.radius;

		if (squaredDistance > squaredRadius)
			return false; // if the ray is further than the radius

		// Compute intersections points if required
		if (closestHit || furthestHit)
		{
			T deltaLambda = std::sqrt(squaredRadius - squaredDistance);

			if (closestHit)
				*closestHit = length - deltaLambda;

			if (furthestHit)
				*furthestHit = length + deltaLambda;
		}

		return true;
	}

	/*!
	* \brief Checks whether or not this ray intersects with the triangle
	* \return true if it intersects
	*
	* \param firstPoint First vertex of the triangle
	* \param secondPoint Second vertex of the triangle
	* \param thirdPoint Third vertex of the triangle
	* \param hit Optional argument to get the parameter where the intersection is only if it happened
	*
	* \see Intersect
	*/
	template<typename T>
	constexpr bool Ray<T>::Intersect(const Vector3<T>& firstPoint, const Vector3<T>& secondPoint, const Vector3<T>& thirdPoint, T* hit) const
	{
		// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
		Vector3<T> firstEdge = secondPoint - firstPoint;
		Vector3<T> secondEdge = thirdPoint - firstPoint;

		Vector3<T> P = Vector3<T>::CrossProduct(direction, secondEdge);
		const T divisor = firstEdge.DotProduct(P);
		if (NumberEquals(divisor, T(0.0)))
			return false; // Ray lies in plane of triangle

		Vector3<T> directionToPoint = origin - firstPoint;
		T u = directionToPoint.DotProduct(P) / divisor;
		if (u < T(0.0) || u > T(1.0))
			return false; // The intersection lies outside of the triangle

		Vector3<T> Q = Vector3<T>::CrossProduct(directionToPoint, firstEdge);
		T v = directionToPoint.DotProduct(Q) / divisor;
		if (v < T(0.0) || u + v > T(1.0))
			return false; // The intersection lies outside of the triangle

		T t = secondEdge.DotProduct(Q) / divisor;
		if (t > T(0.0))
		{
			if (hit)
				*hit = t;
			return true;
		}

		return false;
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object: "Ray(origin: Vector3(origin.x, origin.y, origin.z), direction: Vector3(direction.x, direction.y, direction.z))"
	*/
	template<typename T>
	std::string Ray<T>::ToString() const
	{
		std::ostringstream ss;
		ss << *this;

		return ss.str();
	}

	/*!
	* \brief Multiplies the direction ray with the lambda to get the point along the ray for this parameter
	* \return The point on the ray
	*
	* \param lambda Parameter to obtain a particular point on the ray
	*
	* \see GetPoint
	*/
	template<typename T>
	constexpr Vector3<T> Ray<T>::operator*(T lambda) const
	{
		return GetPoint(lambda);
	}

	/*!
	* \brief Compares the ray to other one
	* \return true if the ray are the same
	*
	* \param rec Other ray to compare with
	*/
	template<typename T>
	constexpr bool Ray<T>::operator==(const Ray& ray) const
	{
		return direction == ray.direction && origin == ray.origin;
	}

	/*!
	* \brief Compares the ray to other one
	* \return false if the ray are the same
	*
	* \param rec Other ray to compare with
	*/
	template<typename T>
	constexpr bool Ray<T>::operator!=(const Ray& ray) const
	{
		return !operator==(ray);
	}

	template<typename T>
	constexpr bool Ray<T>::operator<(const Ray& ray) const
	{
		if (origin != ray.origin)
			return origin < ray.origin;

		return direction < ray.direction;
	}

	template<typename T>
	constexpr bool Ray<T>::operator<=(const Ray& ray) const
	{
		if (origin != ray.origin)
			return origin < ray.origin;

		return direction <= ray.direction;
	}

	template<typename T>
	constexpr bool Ray<T>::operator>(const Ray& ray) const
	{
		if (origin != ray.origin)
			return origin > ray.origin;

		return direction > ray.direction;
	}

	template<typename T>
	constexpr bool Ray<T>::operator>=(const Ray& ray) const
	{
		if (origin != ray.origin)
			return origin > ray.origin;

		return direction >= ray.direction;
	}

	template<typename T>
	constexpr bool Ray<T>::ApproxEqual(const Ray& lhs, const Ray& rhs, T maxDifference)
	{
		return lhs.ApproxEqual(rhs, maxDifference);
	}

	/*!
	* \brief Shorthand for the ray (0, 0, 0), (1, 0, 0)
	* \return A ray with position (0, 0, 0) and direction (1, 0, 0)
	*/
	template<typename T>
	constexpr Ray<T> Ray<T>::AxisX()
	{
		return Ray(Vector3<T>::Zero(), Vector3<T>::UnitX());
	}

	/*!
	* \brief Shorthand for the ray (0, 0, 0), (0, 1, 0)
	* \return A ray with position (0, 0, 0) and direction (0, 1, 0)
	*/

	template<typename T>
	constexpr Ray<T> Ray<T>::AxisY()
	{
		return Ray(Vector3<T>::Zero(), Vector3<T>::UnitY());
	}

	/*!
	* \brief Shorthand for the ray (0, 0, 0), (0, 0, 1)
	* \return A ray with position (0, 0, 0) and direction (0, 0, 1)
	*/

	template<typename T>
	constexpr Ray<T> Ray<T>::AxisZ()
	{
		return Ray(Vector3<T>::Zero(), Vector3<T>::UnitZ());
	}

	/*!
	* \brief Interpolates the ray to other one with a factor of interpolation
	* \return A new ray which is the interpolation of two rectangles
	*
	* \param from Initial ray
	* \param to Target ray
	* \param interpolation Factor of interpolation
	*
	* \remark interpolation is meant to be between 0 and 1, other values are potentially undefined behavior
	*
	* \see Lerp
	*/
	template<typename T>
	constexpr Ray<T> Ray<T>::Lerp(const Ray& from, const Ray& to, T interpolation)
	{
		return Ray<T>(Vector3<T>::Lerp(from.origin, to.origin, interpolation), Vector3<T>::Lerp(from.direction, to.direction, interpolation));
	}

	/*!
	* \brief Serializes a Ray
	* \return true if successfully serialized
	*
	* \param context Serialization context
	* \param ray Input Ray
	*/
	template<typename T>
	bool Serialize(SerializationContext& context, const Ray<T>& ray, TypeTag<Ray<T>>)
	{
		if (!Serialize(context, ray.origin))
			return false;

		if (!Serialize(context, ray.direction))
			return false;

		return true;
	}

	/*!
	* \brief Unserializes a Ray
	* \return true if successfully unserialized
	*
	* \param context Serialization context
	* \param ray Output Ray
	*/
	template<typename T>
	bool Unserialize(SerializationContext& context, Ray<T>* ray, TypeTag<Ray<T>>)
	{
		if (!Unserialize(context, &ray->origin))
			return false;

		if (!Unserialize(context, &ray->direction))
			return false;

		return true;
	}

	/*!
	* \brief Output operator
	* \return The stream
	*
	* \param out The stream
	* \param ray The ray to output
	*/

	template<typename T>
	std::ostream& operator<<(std::ostream& out, const Nz::Ray<T>& ray)
	{
		return out << "Ray(origin: " << ray.origin << ", direction: " << ray.direction << ")";
	}
}

#include <Nazara/Core/DebugOff.hpp>
