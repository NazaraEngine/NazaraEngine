// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Basic.hpp>
#include <algorithm>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)

template<typename T>
NzSphere<T>::NzSphere(T X, T Y, T Z, T Radius)
{
	Set(X, Y, Z, Radius);
}
/*
template<typename T>
NzSphere<T>::NzSphere(const NzCircle<T>& circle)
{
	Set(rect);
}
*/
template<typename T>
NzSphere<T>::NzSphere(const NzVector3<T>& center, T Radius)
{
	Set(center, Radius);
}

template<typename T>
NzSphere<T>::NzSphere(const T sphere[6])
{
	Set(sphere);
}

template<typename T>
template<typename U>
NzSphere<T>::NzSphere(const NzSphere<U>& sphere)
{
	Set(sphere);
}

template<typename T>
bool NzSphere<T>::Contains(T X, T Y, T Z) const
{
	return Distance(X, Y, Z) < F(0.0);
}
/*
template<typename T>
bool NzSphere<T>::Contains(const NzCube<T>& cube) const
{
}
*/
template<typename T>
bool NzSphere<T>::Contains(const NzVector3<T>& point) const
{
	return Contains(point.x, point.y, point.z);
}

template<typename T>
T NzSphere<T>::Distance(T X, T Y, T Z) const
{
	NzVector3<T> distance(X-x, Y-y, Z-z);
	return distance.GetLength() - radius;
}

template<typename T>
T NzSphere<T>::Distance(const NzVector3<T>& point) const
{
	return Distance(point.x, point.y, point.z);
}

template<typename T>
NzSphere<T>& NzSphere<T>::ExtendTo(T X, T Y, T Z)
{
	T distance = Distance(X, Y, Z);

	if (distance > F(0.0))
		radius += distance;

	return *this;
}

template<typename T>
NzSphere<T>& NzSphere<T>::ExtendTo(const NzVector3<T>& point)
{
	return ExtendTo(point);
}

template<typename T>
NzVector3<T> NzSphere<T>::GetNegativeVertex(const NzVector3<T>& normal) const
{
	NzVector3<T> neg(GetPosition());
	neg -= normal * radius;

	return neg;
}

template<typename T>
NzVector3<T> NzSphere<T>::GetPosition() const
{
	return NzVector3<T>(x, y, z);
}

template<typename T>
NzVector3<T> NzSphere<T>::GetPositiveVertex(const NzVector3<T>& normal) const
{
	NzVector3<T> pos(GetPosition());
	pos += normal * radius;

	return pos;
}
/*
template<typename T>
bool NzSphere<T>::Intersect(const NzCube<T>& cube) const
{

}
*/

template<typename T>
bool NzSphere<T>::Intersect(const NzSphere& sphere) const
{
	return Distance(sphere.x, sphere.y, sphere.z) <= sphere.radius;
}

template<typename T>
bool NzSphere<T>::IsValid() const
{
	return radius > F(0.0);
}

template<typename T>
NzSphere<T>& NzSphere<T>::MakeZero()
{
	x = F(0.0);
	y = F(0.0);
	z = F(0.0);
	radius = F(0.0);

	return *this;
}

template<typename T>
NzSphere<T>& NzSphere<T>::Set(T X, T Y, T Z, T Radius)
{
	x = X;
	y = Y;
	z = Z;
	radius = Radius;

	return *this;
}

template<typename T>
NzSphere<T>& NzSphere<T>::Set(const NzVector3<T>& center, T Radius)
{
	x = center.x;
	y = center.y;
	z = center.z;
	radius = Radius;

	return *this;
}
/*
template<typename T>
NzSphere<T>& NzCube<T>::Set(const NzCircle<T>& circle)
{
	x = circle.x;
	y = circle.y;
	z = F(0.0);
	radius = circle.radius;

	return *this;
}
*/
template<typename T>
NzSphere<T>& NzSphere<T>::Set(const NzSphere& sphere)
{
	std::memcpy(this, &sphere, sizeof(NzSphere));

	return *this;
}

template<typename T>
NzSphere<T>& NzSphere<T>::Set(const T sphere[4])
{
	x = sphere[0];
	y = sphere[1];
	z = sphere[2];
	radius = sphere[3];

	return *this;
}

template<typename T>
template<typename U>
NzSphere<T>& NzSphere<T>::Set(const NzSphere<U>& sphere)
{
	x = F(sphere.x);
	y = F(sphere.y);
	z = F(sphere.z);
	radius = F(sphere.radius);

	return *this;
}

template<typename T>
T NzSphere<T>::SquaredDistance(T X, T Y, T Z) const
{
	NzVector3<T> distance(X-x, Y-y, Z-z);
	return distance.SquaredLength() - radius*radius;
}

template<typename T>
T NzSphere<T>::SquaredDistance(const NzVector3<T>& point) const
{
	return SquaredDistance(point.x, point.y, point.z);
}

template<typename T>
NzString NzSphere<T>::ToString() const
{
	NzStringStream ss;

	return ss << "Sphere(" << x << ", " << y << ", " << z << "; " << radius << ')';
}

template<typename T>
T& NzSphere<T>::operator[](unsigned int i)
{
	#if NAZARA_MATH_SAFE
	if (i >= 4)
	{
		NzStringStream ss;
		ss << "Index out of range: (" << i << " >= 4)";

		NazaraError(ss);
		throw std::domain_error(ss.ToString());
	}
	#endif

	return *(&x+i);
}

template<typename T>
T NzSphere<T>::operator[](unsigned int i) const
{
	#if NAZARA_MATH_SAFE
	if (i >= 4)
	{
		NzStringStream ss;
		ss << "Index out of range: (" << i << " >= 4)";

		NazaraError(ss);
		throw std::domain_error(ss.ToString());
	}
	#endif

	return *(&x+i);
}

template<typename T>
NzSphere<T> NzSphere<T>::operator*(T scalar) const
{
	return NzSphere(x, y, z, radius*scalar);
}

template<typename T>
NzSphere<T>& NzSphere<T>::operator*=(T scalar)
{
	radius *= scalar;
}

template<typename T>
bool NzSphere<T>::operator==(const NzSphere& sphere) const
{
	return NzNumberEquals(x, sphere.x) && NzNumberEquals(y, sphere.y) && NzNumberEquals(z, sphere.z) &&
	       NzNumberEquals(radius, sphere.radius);
}

template<typename T>
bool NzSphere<T>::operator!=(const NzSphere& sphere) const
{
	return !operator==(sphere);
}

template<typename T>
NzSphere<T> NzSphere<T>::Lerp(const NzSphere& from, const NzSphere& to, T interpolation)
{
	#ifdef NAZARA_DEBUG
	if (interpolation < F(0.0) || interpolation > F(1.0))
	{
		NazaraError("Interpolation must be in range [0..1] (Got " + NzString::Number(interpolation) + ')');
		return Zero();
	}
	#endif

	NzSphere sphere;
	sphere.x = NzLerp(from.x, to.x, interpolation);
	sphere.y = NzLerp(from.y, to.y, interpolation);
	sphere.z = NzLerp(from.z, to.z, interpolation);
	sphere.radius = NzLerp(from.radius, to.radius, interpolation);

	return sphere;
}

template<typename T>
NzSphere<T> NzSphere<T>::Zero()
{
	NzSphere sphere;
	sphere.MakeZero();

	return sphere;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzSphere<T>& sphere)
{
	return out << sphere.ToString();
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
