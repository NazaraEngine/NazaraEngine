// Copyright (C) 2017 Gawaboumga (https://github.com/Gawaboumga) - Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <limits>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)

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
	Ray<T>::Ray(T X, T Y, T Z, T DirectionX, T DirectionY, T DirectionZ)
	{
		Set(X, Y, Z, DirectionX, DirectionY, DirectionZ);
	}

	/*!
	* \brief Constructs a Ray object from two Vector3
	*
	* \param Origin Vector which represents the origin of the ray
	* \param Direction Vector which represents the direction of the ray
	*/

	template<typename T>
	Ray<T>::Ray(const Vector3<T>& Origin, const Vector3<T>& Direction)
	{
		Set(Origin, Direction);
	}

	/*!
	* \brief Constructs a Ray object from two arrays of three elements
	*
	* \param Origin[3] Origin[0] is X position, Origin[1] is Y position and Origin[2] is Z position
	* \param Direction[3] Direction[0] is X direction, Direction[1] is Y direction and Direction[2] is Z direction
	*/

	template<typename T>
	Ray<T>::Ray(const T Origin[3], const T Direction[3])
	{
		Set(Origin, Direction);
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
		Set(planeOne, planeTwo);
	}

	/*!
	* \brief Constructs a Ray object from another type of Ray
	*
	* \param ray Ray of type U to convert to type T
	*/

	template<typename T>
	template<typename U>
	Ray<T>::Ray(const Ray<U>& ray)
	{
		Set(ray);
	}

	/*!
	* \brief Constructs a Ray object from two Vector3 from another type of Ray
	*
	* \param Origin Origin of type U to convert to type T
	* \param Direction Direction of type U to convert to type T
	*/

	template<typename T>
	template<typename U>
	Ray<T>::Ray(const Vector3<U>& Origin, const Vector3<U>& Direction)
	{
		Set(Origin, Direction);
	}

	/*!
	* \brief Finds the closest point of the ray from point
	* \return The parameter where the point along this ray that is closest to the point provided
	*
	* \param point The point to get the closest approach to
	*/

	template<typename T>
	T Ray<T>::ClosestPoint(const Vector3<T>& point) const
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
	Vector3<T> Ray<T>::GetPoint(T lambda) const
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
	* \remark If BoundingVolume is Extend_Infinite, then closestHit and furthestHit are equal to 0 et infinity
	* \remark If BoundingVolume is Extend_Null, then closestHit and furthestHit are unchanged
	* \remark If enumeration of BoundingVolume is not defined in Extend, a NazaraError is thrown and closestHit and furthestHit are unchanged
	*
	* \see Intersect
	*/

	template<typename T>
	bool Ray<T>::Intersect(const BoundingVolume<T>& volume, T* closestHit, T* furthestHit) const
	{
		switch (volume.extend)
		{
			case Extend_Finite:
			{
				if (Intersect(volume.aabb))
					return Intersect(volume.obb, closestHit, furthestHit);

				return false;
			}

			case Extend_Infinite:
			{
				if (closestHit)
					*closestHit = F(0.0);

				if (furthestHit)
					*furthestHit = std::numeric_limits<T>::infinity();

				return true;
			}

			case Extend_Null:
				return false;
		}

		NazaraError("Invalid extend type (0x" + String::Number(volume.extend, 16) + ')');
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
	bool Ray<T>::Intersect(const Box<T>& box, T* closestHit, T* furthestHit) const
	{
		// http://www.gamedev.net/topic/429443-obb-ray-and-obb-plane-intersection/
		T tfirst = F(0.0);
		T tlast = std::numeric_limits<T>::infinity();

		Vector3<T> boxMin = box.GetMinimum();
		Vector3<T> boxMax = box.GetMaximum();

		for (unsigned int i = 0; i < 3; ++i)
		{
			T dir = direction[i];
			T ori = origin[i];
			T max = boxMax[i];
			T min = boxMin[i];

			if (NumberEquals(dir, F(0.0)))
			{
				if (ori < max && ori > min)
					continue;

				return false;
			}

			T tmin = (min - ori) / dir;
			T tmax = (max - ori) / dir;
			if (tmin > tmax)
				std::swap(tmin, tmax);

			if (tmax < tfirst || tmin > tlast)
				return false;

			tfirst = std::max(tfirst, tmin);
			tlast = std::min(tlast, tmax);
		}

		if (closestHit)
			*closestHit = tfirst;

		if (furthestHit)
			*furthestHit = tlast;

		return true;
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
	bool Ray<T>::Intersect(const Box<T>& box, const Matrix4<T>& transform, T* closestHit, T* furthestHit) const
	{
		// http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-custom-ray-obb-function/
		// Intersection method from Real-Time Rendering and Essential Mathematics for Games
		T tMin = F(0.0);
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

			if (!NumberEquals(f, F(0.0)))
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
				if (-e + boxMin[i] > F(0.0) || -e + boxMax[i] < F(0.0))
					return false;
		}

		if (closestHit)
			*closestHit = tMin;

		if (furthestHit)
			*furthestHit = tMax;

		return true;
	}

	/*!
	* \brief Checks whether or not this ray intersects with the OrientedBox
	* \return true if it intersects
	*
	* \param orientedBox OrientedBox to check
	* \param closestHit Optional argument to get the closest parameter where the intersection is only if it happened
	* \param furthestHit Optional argument to get the furthest parameter where the intersection is only if it happened
	*
	* \see Intersect
	*/

	template<typename T>
	bool Ray<T>::Intersect(const OrientedBox<T>& orientedBox, T* closestHit, T* furthestHit) const
	{
		Vector3<T> corner = orientedBox.GetCorner(BoxCorner_FarLeftBottom);
		Vector3<T> oppositeCorner = orientedBox.GetCorner(BoxCorner_NearRightTop);

		Vector3<T> width = (orientedBox.GetCorner(BoxCorner_NearLeftBottom) - corner);
		Vector3<T> height = (orientedBox.GetCorner(BoxCorner_FarLeftTop) - corner);
		Vector3<T> depth = (orientedBox.GetCorner(BoxCorner_FarRightBottom) - corner);

		// Construction de la matrice de transformation de l'OBB
		Matrix4<T> matrix(width.x, height.x, depth.x, corner.x,
		                  width.y, height.y, depth.y, corner.y,
		                  width.z, height.z, depth.z, corner.z,
		                  F(0.0),  F(0.0),   F(0.0),  F(1.0));

		matrix.InverseAffine();

		corner = matrix.Transform(corner);
		oppositeCorner = matrix.Transform(oppositeCorner);

		Box<T> tmpBox(corner, oppositeCorner);
		Ray<T> tmpRay(matrix.Transform(origin), matrix.Transform(direction));

		return tmpRay.Intersect(tmpBox, closestHit, furthestHit);
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
	bool Ray<T>::Intersect(const Plane<T>& plane, T* hit) const
	{
		T divisor = plane.normal.DotProduct(direction);
		if (NumberEquals(divisor, F(0.0)))
			return false; // Perpendicular

		T lambda = -(plane.normal.DotProduct(origin) - plane.distance) / divisor; // The plane is ax + by + cz = d
		if (lambda < F(0.0))
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
	bool Ray<T>::Intersect(const Sphere<T>& sphere, T* closestHit, T* furthestHit) const
	{
		Vector3<T> sphereRay = sphere.GetPosition() - origin;
		T length = sphereRay.DotProduct(direction);

		if (length < F(0.0))
			return false; // ray is perpendicular to the vector origin - center

		T squaredDistance = sphereRay.GetSquaredLength() - length * length;
		T squaredRadius = sphere.radius * sphere.radius;

		if (squaredDistance > squaredRadius)
			return false; // if the ray is further than the radius

		// Calcul des points d'intersection si besoin
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
	bool Ray<T>::Intersect(const Vector3<T>& firstPoint, const Vector3<T>& secondPoint, const Vector3<T>& thirdPoint, T* hit) const
	{
		// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
		Vector3<T> firstEdge = secondPoint - firstPoint;
		Vector3<T> secondEdge = thirdPoint - firstPoint;

		Vector3<T> P = Vector3<T>::CrossProduct(direction, secondEdge);
		const T divisor = firstEdge.DotProduct(P);
		if (NumberEquals(divisor, F(0.0)))
			return false; // Ray lies in plane of triangle

		Vector3<T> directionToPoint = origin - firstPoint;
		T u = directionToPoint.DotProduct(P) / divisor;
		if (u < F(0.0) || u > F(1.0))
			return 0; // The intersection lies outside of the triangle

		Vector3<T> Q = Vector3<T>::CrossProduct(directionToPoint, firstEdge);
		T v = directionToPoint.DotProduct(Q) / divisor;
		if (v < F(0.0) || u + v > F(1.0))
			return 0; // The intersection lies outside of the triangle

		T t = secondEdge.DotProduct(Q) / divisor;
		if (t > F(0.0))
		{
			if (hit)
				*hit = t;
			return true;
		}

		return false;
	}

	/*!
	* \brief Makes the ray with position (0, 0, 0) and direction (1, 0, 0)
	* \return A reference to this ray with position (0, 0, 0) and direction (1, 0, 0)
	*
	* \see AxisX
	*/

	template<typename T>
	Ray<T>& Ray<T>::MakeAxisX()
	{
		return Set(Vector3<T>::Zero(), Vector3<T>::UnitX());
	}

	/*!
	* \brief Makes the ray with position (0, 0, 0) and direction (0, 1, 0)
	* \return A reference to this ray with position (0, 0, 0) and direction (0, 1, 0)
	*
	* \see AxisY
	*/

	template<typename T>
	Ray<T>& Ray<T>::MakeAxisY()
	{
		return Set(Vector3<T>::Zero(), Vector3<T>::UnitY());
	}

	/*!
	* \brief Makes the ray with position (0, 0, 0) and direction (0, 0, 1)
	* \return A reference to this ray with position (0, 0, 0) and direction (0, 0, 1)
	*
	* \see AxisZ
	*/

	template<typename T>
	Ray<T>& Ray<T>::MakeAxisZ()
	{
		return Set(Vector3<T>::Zero(), Vector3<T>::UnitZ());
	}

	/*!
	* \brief Sets the components of the ray with position and direction
	* \return A reference to this ray
	*
	* \param X X position
	* \param Y Y position
	* \param Z Z position
	* \param DirectionX X component of the vector direction
	* \param DirectionY Y component of the vector direction
	* \param DirectionY Y component of the vector direction
	*/

	template<typename T>
	Ray<T>& Ray<T>::Set(T X, T Y, T Z, T directionX, T directionY, T directionZ)
	{
		direction.Set(directionX, directionY, directionZ);
		origin.Set(X, Y, Z);

		return *this;
	}

	/*!
	* \brief Sets the components of the ray with position and direction
	* \return A reference to this ray
	*
	* \param Origin Vector which represents the origin of the ray
	* \param Direction Vector which represents the direction of the ray
	*/

	template<typename T>
	Ray<T>& Ray<T>::Set(const Vector3<T>& Origin, const Vector3<T>& Direction)
	{
		direction = Direction;
		origin = Origin;

		return *this;
	}

	/*!
	* \brief Sets the components of this ray from two arrays of three elements
	* \return A reference to this ray
	*
	* \param Origin[3] Origin[0] is X position, Origin[1] is Y position and Origin[2] is Z position
	* \param Direction[3] Direction[0] is X direction, Direction[1] is Y direction and Direction[2] is Z direction
	*/

	template<typename T>
	Ray<T>& Ray<T>::Set(const T Origin[3], const T Direction[3])
	{
		direction.Set(Direction);
		origin.Set(Origin);

		return *this;
	}

	/*!
	* \brief Sets the components of this ray from the intersection of two planes
	* \return A reference to this ray
	*
	* \param planeOne First plane
	* \param planeTwo Second secant plane
	*
	* \remark Produce a NazaraError if planes are parallel with NAZARA_MATH_SAFE defined
	* \throw std::domain_error if NAZARA_MATH_SAFE is defined and planes are parallel
	*/

	template<typename T>
	Ray<T>& Ray<T>::Set(const Plane<T>& planeOne, const Plane<T>& planeTwo)
	{
		T termOne = planeOne.normal.GetLength();
		T termTwo = planeOne.normal.DotProduct(planeTwo.normal);
		T termFour = planeTwo.normal.GetLength();
		T det = termOne * termFour - termTwo * termTwo;

		#if NAZARA_MATH_SAFE
		if (NumberEquals(det, F(0.0)))
		{
			String error("Planes are parallel");

			NazaraError(error);
			throw std::domain_error(error.ToStdString());
		}
		#endif

		T invdet = F(1.0) / det;
		T fc0 = (termFour * -planeOne.distance + termTwo * planeTwo.distance) * invdet;
		T fc1 = (termOne * -planeTwo.distance + termTwo * planeOne.distance) * invdet;

		direction = planeOne.normal.CrossProduct(planeTwo.normal);
		origin = planeOne.normal * fc0 + planeTwo.normal * fc1;

		return *this;
	}

	/*!
	* \brief Sets the components of the ray with components from another
	* \return A reference to this ray
	*
	* \param ray The other ray
	*/

	template<typename T>
	Ray<T>& Ray<T>::Set(const Ray& ray)
	{
		std::memcpy(this, &ray, sizeof(Ray));

		return *this;
	}

	/*!
	* \brief Sets the components of the ray from another type of Ray
	* \return A reference to this ray
	*
	* \param ray Ray of type U to convert its components
	*/

	template<typename T>
	template<typename U>
	Ray<T>& Ray<T>::Set(const Ray<U>& ray)
	{
		direction.Set(ray.direction);
		origin.Set(ray.origin);

		return *this;
	}

	/*!
	* \brief Sets the components of the ray from another type of Ray
	* \return A reference to this ray
	*
	* \param Origin Origin of type U to convert to type T
	* \param Direction Direction of type U to convert to type T
	*/

	template<typename T>
	template<typename U>
	Ray<T>& Ray<T>::Set(const Vector3<U>& Origin, const Vector3<U>& Direction)
	{
		direction.Set(Direction);
		origin.Set(Origin);

		return *this;
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object: "Ray(origin: Vector3(origin.x, origin.y, origin.z), direction: Vector3(direction.x, direction.y, direction.z))"
	*/

	template<typename T>
	String Ray<T>::ToString() const
	{
		StringStream ss;

		return ss << "Ray(origin: " << origin.ToString() << ", direction: " << direction.ToString() << ")";
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
	Vector3<T> Ray<T>::operator*(T lambda) const
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
	bool Ray<T>::operator==(const Ray& ray) const
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
	bool Ray<T>::operator!=(const Ray& ray) const
	{
		return !operator==(ray);
	}

	/*!
	* \brief Shorthand for the ray (0, 0, 0), (1, 0, 0)
	* \return A ray with position (0, 0, 0) and direction (1, 0, 0)
	*
	* \see MakeAxisX
	*/

	template<typename T>
	Ray<T> Ray<T>::AxisX()
	{
		Ray axis;
		axis.MakeAxisX();

		return axis;
	}

	/*!
	* \brief Shorthand for the ray (0, 0, 0), (0, 1, 0)
	* \return A ray with position (0, 0, 0) and direction (0, 1, 0)
	*
	* \see MakeAxisY
	*/

	template<typename T>
	Ray<T> Ray<T>::AxisY()
	{
		Ray axis;
		axis.MakeAxisY();

		return axis;
	}

	/*!
	* \brief Shorthand for the ray (0, 0, 0), (0, 0, 1)
	* \return A ray with position (0, 0, 0) and direction (0, 0, 1)
	*
	* \see MakeAxisZ
	*/

	template<typename T>
	Ray<T> Ray<T>::AxisZ()
	{
		Ray axis;
		axis.MakeAxisZ();

		return axis;
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
	Ray<T> Ray<T>::Lerp(const Ray& from, const Ray& to, T interpolation)
	{
		return Ray<T>(Nz::Vector3<T>::Lerp(from.origin, to.origin, interpolation), Nz::Vector3<T>::Lerp(from.direction, to.direction, interpolation));
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
	return out << ray.ToString();
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
