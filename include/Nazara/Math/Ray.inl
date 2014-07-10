// Copyright (C) 2014 Gawaboumga (https://github.com/Gawaboumga) - Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <limits>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)

template<typename T>
NzRay<T>::NzRay(T X, T Y, T Z, T DirectionX, T DirectionY, T DirectionZ)
{
	Set(X, Y, Z, DirectionX, DirectionY, DirectionZ);
}

template<typename T>
NzRay<T>::NzRay(const T Origin[3], const T Direction[3])
{
	Set(Origin, Direction);
}

template<typename T>
NzRay<T>::NzRay(const NzPlane<T>& planeOne, const NzPlane<T>& planeTwo)
{
	Set(planeOne, planeTwo);
}

template<typename T>
NzRay<T>::NzRay(const NzVector3<T>& Origin, const NzVector3<T>& Direction)
{
	Set(Origin, Direction);
}

template<typename T>
template<typename U>
NzRay<T>::NzRay(const NzRay<U>& ray)
{
	Set(ray);
}

template<typename T>
template<typename U>
NzRay<T>::NzRay(const NzVector3<U>& Origin, const NzVector3<U>& Direction)
{
	Set(Origin, Direction);
}

template<typename T>
T NzRay<T>::ClosestPoint(const NzVector3<T>& point) const
{
	NzVector3<T> delta = point - origin;
	T vsq = direction.GetSquaredLength();
	T proj = delta.DotProduct(direction);

	return proj/vsq;
}

template<typename T>
NzVector3<T> NzRay<T>::GetPoint(T lambda) const
{
	return origin + lambda*direction;
}
/*
template<typename T>
bool NzRay<T>::Intersect(const NzBoundingVolume<T>& volume, T* closestHit, T* farthestHit) const
{
	switch (volume.extend)
	{
		case nzExtend_Finite:
		{
			if (Intersect(volume.aabb))
				return Intersect(volume.obb, closestHit, farthestHit);

			return false;
		}

		case nzExtend_Infinite:
		{
			if (closestHit)
				*closestHit = F(0.0);

			if (farthestHit)
				*farthestHit = std::numeric_limits<T>::infinity();

			return true;
		}

		case nzExtend_Null:
			return false;
	}

	NazaraError("Invalid extend type (0x" + NzString::Number(volume.extend, 16) + ')');
	return false;
}
*/
template<typename T>
bool NzRay<T>::Intersect(const NzBox<T>& box, T* closestHit, T* farthestHit) const
{
	// http://www.gamedev.net/topic/429443-obb-ray-and-obb-plane-intersection/
	T tfirst = F(0.0);
	T tlast = std::numeric_limits<T>::infinity();

	NzVector3<T> boxMin = box.GetMinimum();
	NzVector3<T> boxMax = box.GetMaximum();

	for (unsigned int i = 0; i < 3; ++i)
	{
		T dir = direction[i];
		T ori = origin[i];
		T max = boxMax[i];
		T min = boxMin[i];

		if (NzNumberEquals(dir, F(0.0)))
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

	if (farthestHit)
		*farthestHit = tlast;

	return true;
}

template<typename T>
bool NzRay<T>::Intersect(const NzBox<T>& box, const NzMatrix4<T>& transform, T* closestHit, T* farthestHit) const
{
	// http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-custom-ray-obb-function/
	// Intersection method from Real-Time Rendering and Essential Mathematics for Games
	T tMin = F(0.0);
	T tMax = std::numeric_limits<T>::infinity();

	NzVector3<T> boxMin = box.GetMinimum();
	NzVector3<T> boxMax = box.GetMaximum();
	NzVector3<T> delta = transform.GetTranslation() - origin;

	// Test intersection with the 2 planes perpendicular to the OBB's X axis
	for (unsigned int i = 0; i < 3; ++i)
	{
		NzVector3<T> axis(transform(0, i), transform(1, i), transform(2, i));
		T e = axis.DotProduct(delta);
		T f = direction.DotProduct(axis);

		if (!NzNumberEquals(f, F(0.0)))
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

	if (farthestHit)
		*farthestHit = tMax;

	return true;
}

///FIXME: Le test ci-dessous est beaucoup trop approximatif pour être vraiment utile
/// Mais le vrai problème vient certainement des OrientedBox en elles-mêmes, peut-être faut-il envisager de les refaire ?
/*
template<typename T>
bool NzRay<T>::Intersect(const NzOrientedBox<T>& orientedBox, T* closestHit, T* farthestHit) const
{
    NzVector3<T> width = (orientedBox.GetCorner(nzCorner_NearLeftBottom) - orientedBox.GetCorner(nzCorner_FarLeftBottom)).Normalize();
    NzVector3<T> height = (orientedBox.GetCorner(nzCorner_FarLeftTop) - orientedBox.GetCorner(nzCorner_FarLeftBottom)).Normalize();
    NzVector3<T> depth = (orientedBox.GetCorner(nzCorner_FarRightBottom) - orientedBox.GetCorner(nzCorner_FarLeftBottom)).Normalize();

	// Construction de la matrice de transformation de l'OBB
	NzMatrix4<T> matrix(width.x, height.x, depth.x, F(0.0),
	                    width.y, height.y, depth.y, F(0.0),
	                    width.z, height.z, depth.z, F(0.0),
	                    F(0.0),  F(0.0),   F(0.0),  F(1.0));

	// Test en tant qu'AABB avec une matrice de rotation
	return Intersect(orientedBox.localBox, matrix, closestHit, farthestHit);
}
*/
template<typename T>
bool NzRay<T>::Intersect(const NzPlane<T>& plane, T* hit) const
{
	T divisor = plane.normal.DotProduct(direction);
	if (NzNumberEquals(divisor, F(0.0)))
		return false; // perpendicular

	T lambda = -(plane.normal.DotProduct(origin) - plane.distance) / divisor; // The plane is ax+by+cz=d
	if (lambda < F(0.0))
		return false; // Le plan est derrière le rayon

	if (hit)
		*hit = lambda;

	return true;
}

template<typename T>
bool NzRay<T>::Intersect(const NzSphere<T>& sphere, T* closestHit, T* farthestHit) const
{
	NzVector3<T> sphereRay = sphere.GetPosition() - origin;
	T length = sphereRay.DotProduct(direction);

	if (length < F(0.0))
		return false; // ray is perpendicular to the vector origin - center

	T squaredDistance = sphereRay.GetSquaredLength() - length*length;
	T squaredRadius = sphere.radius*sphere.radius;

	if (squaredDistance > squaredRadius)
		return false; // if the ray is further than the radius

	// Calcul des points d'intersection si besoin
	if (closestHit || farthestHit)
	{
		T deltaLambda = std::sqrt(squaredRadius - squaredDistance);

		if (closestHit)
			*closestHit = length - deltaLambda;

		if (farthestHit)
			*farthestHit = length + deltaLambda;
	}

	return true;
}

template<typename T>
NzRay<T>& NzRay<T>::MakeAxisX()
{
	return Set(NzVector3<T>::Zero(), NzVector3<T>::UnitX());
}

template<typename T>
NzRay<T>& NzRay<T>::MakeAxisY()
{
	return Set(NzVector3<T>::Zero(), NzVector3<T>::UnitY());
}

template<typename T>
NzRay<T>& NzRay<T>::MakeAxisZ()
{
	return Set(NzVector3<T>::Zero(), NzVector3<T>::UnitZ());
}

template<typename T>
NzRay<T>& NzRay<T>::Set(T X, T Y, T Z, T directionX, T directionY, T directionZ)
{
	direction.Set(directionX, directionY, directionZ);
	origin.Set(X, Y, Z);

	return *this;
}

template<typename T>
NzRay<T>& NzRay<T>::Set(const T Origin[3], const T Direction[3])
{
	direction.Set(Direction);
	origin.Set(Origin);

	return *this;
}

template<typename T>
NzRay<T>& NzRay<T>::Set(const NzPlane<T>& planeOne, const NzPlane<T>& planeTwo)
{
	T termOne = planeOne.normal.GetLength();
	T termTwo = planeOne.normal.DotProduct(planeTwo.normal);
	T termFour = planeTwo.normal.GetLength();
	T det = termOne * termFour - termTwo * termTwo;

	#if NAZARA_MATH_SAFE
	if (NzNumberEquals(det, F(0.0)))
	{
		NzString error("Planes are parallel.");

		NazaraError(error);
		throw std::domain_error(error);
	}
	#endif

	T invdet = F(1.0) / det;
	T fc0 = (termFour * -planeOne.distance + termTwo * planeTwo.distance) * invdet;
	T fc1 = (termOne * -planeTwo.distance + termTwo * planeOne.distance) * invdet;

	direction = planeOne.normal.CrossProduct(planeTwo.normal);
	origin = planeOne.normal * fc0 + planeTwo.normal * fc1;

	return *this;
}

template<typename T>
NzRay<T>& NzRay<T>::Set(const NzRay& ray)
{
	std::memcpy(this, &ray, sizeof(NzRay));

	return *this;
}

template<typename T>
NzRay<T>& NzRay<T>::Set(const NzVector3<T>& Origin, const NzVector3<T>& Direction)
{
	direction = Direction;
	origin = Origin;

	return *this;
}

template<typename T>
template<typename U>
NzRay<T>& NzRay<T>::Set(const NzRay<U>& ray)
{
	direction.Set(ray.direction);
	origin.Set(ray.origin);

	return *this;
}

template<typename T>
template<typename U>
NzRay<T>& NzRay<T>::Set(const NzVector3<U>& Origin, const NzVector3<U>& Direction)
{
	direction.Set(Direction);
	origin.Set(Origin);

	return *this;
}

template<typename T>
NzString NzRay<T>::ToString() const
{
	NzStringStream ss;

	return ss << "Ray(origin: " << origin.ToString() << ", direction: " << direction.ToString() << ")";
}

template<typename T>
NzVector3<T> NzRay<T>::operator*(T lambda) const
{
	return GetPoint(lambda);
}

template<typename T>
NzRay<T> NzRay<T>::AxisX()
{
	NzRay axis;
	axis.MakeAxisX();

	return axis;
}

template<typename T>
NzRay<T> NzRay<T>::AxisY()
{
	NzRay axis;
	axis.MakeAxisY();

	return axis;
}

template<typename T>
NzRay<T> NzRay<T>::AxisZ()
{
	NzRay axis;
	axis.MakeAxisZ();

	return axis;
}

template<typename T>
NzRay<T> NzRay<T>::Lerp(const NzRay& from, const NzRay& to, T interpolation)
{
	return NzRay<T>(from.origin.Lerp(to.origin, interpolation), from.direction.Lerp(to.direction, interpolation));
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzRay<T>& ray)
{
	return out << ray.ToString();
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
