// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Basic.hpp>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)

template<typename T>
NzPlane<T>::NzPlane(T normalX, T normalY, T normalZ, T D)
{
	Set(normalX, normalY, normalZ, D);
}

template<typename T>
NzPlane<T>::NzPlane(const T plane[4])
{
	Set(plane);
}

template<typename T>
NzPlane<T>::NzPlane(const NzVector3<T>& Normal, T D)
{
	Set(Normal, D);
}

template<typename T>
NzPlane<T>::NzPlane(const NzVector3<T>& Normal, const NzVector3<T>& point)
{
	Set(Normal, point);
}

template<typename T>
NzPlane<T>::NzPlane(const NzVector3<T>& point1, const NzVector3<T>& point2, const NzVector3<T>& point3)
{
	Set(point1, point2, point3);
}

template<typename T>
template<typename U>
NzPlane<T>::NzPlane(const NzPlane<U>& plane)
{
	Set(plane);
}

template<typename T>
T NzPlane<T>::Distance(const NzVector3<T>& point) const
{
	return normal.DotProduct(point) + distance;
}

template<typename T>
T NzPlane<T>::Distance(T x, T y, T z) const
{
	return Distance(NzVector3<T>(x, y, z));
}

template<typename T>
NzVector3<T> NzPlane<T>::GetNormal() const
{
    return normal;
}

template<typename T>
T NzPlane<T>::GetDistance() const
{
    return distance;
}

template<typename T>
NzPlane<T>& NzPlane<T>::Set(T normalX, T normalY, T normalZ, T D)
{
	distance = D;
	normal.Set(normalX, normalY, normalZ);

	return *this;
}

template<typename T>
NzPlane<T>& NzPlane<T>::Set(const T plane[4])
{
	normal.Set(plane[0], plane[1], plane[2]);
	distance = plane[3];

	return *this;
}

template<typename T>
NzPlane<T>& NzPlane<T>::Set(const NzPlane& plane)
{
	std::memcpy(this, &plane, sizeof(NzPlane));

	return *this;
}

template<typename T>
NzPlane<T>& NzPlane<T>::Set(const NzVector3<T>& Normal, T D)
{
	distance = D;
	normal = Normal;

	return *this;
}

template<typename T>
NzPlane<T>& NzPlane<T>::Set(const NzVector3<T>& Normal, const NzVector3<T>& point)
{
	normal = Normal;
	distance = -normal.DotProduct(point);

	return *this;
}

template<typename T>
NzPlane<T>& NzPlane<T>::Set(const NzVector3<T>& point1, const NzVector3<T>& point2, const NzVector3<T>& point3)
{
	NzVector3<T> edge1 = point2 - point1;
	NzVector3<T> edge2 = point3 - point1;
	normal = edge1.CrossProduct(edge2);
	normal.Normalize();

	distance = -normal.DotProduct(point3);

	return *this;
}

template<typename T>
template<typename U>
NzPlane<T>& NzPlane<T>::Set(const NzPlane<U>& plane)
{
	normal.Set(plane.normal);
	distance = F(plane.distance);

	return *this;
}

template<typename T>
NzString NzPlane<T>::ToString() const
{
	NzStringStream ss;

	return ss << "Plane(Normal: " << normal.ToString() << "; Distance: " << distance << ')';
}

template<typename T>
NzPlane<T> NzPlane<T>::Lerp(const NzPlane& from, const NzPlane& to, T interpolation)
{
	#ifdef NAZARA_DEBUG
	if (interpolation < F(0.0) || interpolation > F(1.0))
	{
		NazaraError("Interpolation must be in range [0..1] (Got " + NzString::Number(interpolation) + ')');
		return NzPlane();
	}
	#endif

	NzPlane plane;
	plane.distance = NzLerp(from.distance, to.distance, interpolation);
	plane.normal = NzVector3<T>::Lerp(from.normal, to.normal, interpolation);
	plane.normal.Normalize();

	return plane;
}

template<typename T>
NzPlane<T> NzPlane<T>::XY()
{
    return NzPlane<T>(F(0.0), F(0.0), F(1.0), F(0.0));
}

template<typename T>
NzPlane<T> NzPlane<T>::XZ()
{
    return NzPlane<T>(F(0.0), F(1.0), F(0.0), F(0.0));
}

template<typename T>
NzPlane<T> NzPlane<T>::YZ()
{
    return NzPlane<T>(F(1.0), F(0.0), F(0.0), F(0.0));
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzPlane<T>& plane)
{
	return out << plane.ToString();
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
