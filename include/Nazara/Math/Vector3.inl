// Copyright (C) 2012 Rémi Bèges
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Basic.hpp>
#include <cmath>
#include <cstdlib>
#include <stdexcept>
#include <Nazara/Core/Debug.hpp>

template<typename T>
NzVector3<T>::NzVector3()
{
}

template<typename T>
NzVector3<T>::NzVector3(T X, T Y, T Z) :
x(X),
y(Y),
z(Z)
{
}

template<typename T>
NzVector3<T>::NzVector3(T scale) :
x(scale),
y(scale),
z(scale)
{
}

template<typename T>
NzVector3<T>::NzVector3(T vec[3]) :
x(vec[0]),
y(vec[1]),
z(vec[2])
{
}

template<typename T>
template<typename U>
NzVector3<T>::NzVector3(const NzVector3<U>& vec) :
x(static_cast<T>(vec.x)),
y(static_cast<T>(vec.y)),
z(static_cast<T>(vec.z))
{
}

template<typename T>
T NzVector3<T>::AbsDotProduct(const NzVector3& vec) const
{
	return std::fabs(x * vec.x) + std::fabs(y * vec.y) + std::fabs(z * vec.z);
}

template<> inline int NzVector3<int>::AbsDotProduct(const NzVector3<int>& vec) const
{
	return std::labs(x * vec.x) + std::labs(y * vec.y) + std::labs(z * vec.z);
}

template<typename T>
NzVector3<T> NzVector3<T>::CrossProduct(const NzVector3& vec) const
{
	return NzVector3(y * vec.z - z * vec.y, z * vec.x - x * vec.y, x * vec.y - y * vec.x);
}

template<typename T>
double NzVector3<T>::Distance(const NzVector3& vec) const
{
	return std::sqrt(SquaredDistance(vec));
}

template<typename T>
T NzVector3<T>::DotProduct(const NzVector3& vec) const
{
	return x * vec.x + y * vec.y + z * vec.z;
}

template<typename T>
NzVector3<T> NzVector3<T>::GetNormal() const
{
	NzVector3 vec(*this);
	vec.Normalize();

	return vec;
}

template<typename T>
void NzVector3<T>::MakeCeil(const NzVector3& vec)
{
	if (vec.x > x)
		x = vec.x;

	if (vec.y > y)
		y = vec.y;

    if (vec.z > z)
        z = vec.z;
}

template<typename T>
void NzVector3<T>::MakeFloor(const NzVector3& vec)
{
	if (vec.x < x)
		x = vec.x;

	if (vec.y < y)
		y = vec.y;

    if (vec.z < z)
        z = vec.z;
}

template<typename T>
double NzVector3<T>::Length() const
{
	return std::sqrt(SquaredLength());
}

template<typename T>
double NzVector3<T>::Normalize()
{
	double length = Length();

	if (length != 0.f)
	{
		x /= length;
		y /= length;
		z /= length;
	}

	return length;
}

template<typename T>
T NzVector3<T>::SquaredDistance(const NzVector3& vec) const
{
	return operator-(vec).SquaredLength();
}

template<typename T>
T NzVector3<T>::SquaredLength() const
{
	return x * x + y * y + z * z;
}

template<typename T>
NzString NzVector3<T>::ToString() const
{
	NzStringStream ss;

	return ss << "Vector3(" << x << ", " << y << ", " << z <<')';
}

template<typename T>
NzVector3<T>::operator NzString() const
{
	return ToString();
}

template<typename T>
T& NzVector3<T>::operator[](unsigned int i)
{
	if (i >= 3)
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Index out of range (" << i << " >= 3)";

		throw std::domain_error(ss.ToString());
	}

	return *(&x+i);
}

template<typename T>
T NzVector3<T>::operator[](unsigned int i) const
{
	if (i >= 3)
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Index out of range (" << i << " >= 3)";

		throw std::domain_error(ss.ToString());
	}

	return *(&x+i);
}

template<typename T>
const NzVector3<T>& NzVector3<T>::operator+() const
{
	return *this;
}

template<typename T>
NzVector3<T> NzVector3<T>::operator-() const
{
	return NzVector3(-x, -y, -z);
}

template<typename T>
NzVector3<T> NzVector3<T>::operator+(const NzVector3& vec) const
{
	return NzVector3(x + vec.x, y + vec.y, z + vec.z);
}

template<typename T>
NzVector3<T> NzVector3<T>::operator-(const NzVector3& vec) const
{
	return NzVector3(x - vec.x, y - vec.y, z - vec.z);
}

template<typename T>
NzVector3<T> NzVector3<T>::operator*(const NzVector3& vec) const
{
	return NzVector3(x * vec.x, y * vec.y, z * vec.z);
}

template<typename T>
NzVector3<T> NzVector3<T>::operator*(T scale) const
{
	return NzVector3(x * scale, y * scale, z * scale);
}

template<typename T>
NzVector3<T> NzVector3<T>::operator/(const NzVector3& vec) const
{
	if (vec.x == 0.f || vec.y == 0.f || vec.z == 0.f)
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Division by zero";

		throw std::domain_error(ss.ToString());
	}

	return NzVector3(x / vec.x, y / vec.y, z / vec.z);
}

template<typename T>
NzVector3<T> NzVector3<T>::operator/(T scale) const
{
	if (scale == 0.f)
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Division by zero";

		throw std::domain_error(ss.ToString());
	}

	return NzVector3(x / scale, y / scale, z / scale);
}

template<typename T>
NzVector3<T>& NzVector3<T>::operator+=(const NzVector3& vec)
{
	x += vec.x;
	y += vec.y;
	z += vec.z;

	return *this;
}

template<typename T>
NzVector3<T>& NzVector3<T>::operator-=(const NzVector3& vec)
{
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;

	return *this;
}

template<typename T>
NzVector3<T>& NzVector3<T>::operator*=(const NzVector3& vec)
{
	x *= vec.x;
	y *= vec.y;
	z *= vec.z;

	return *this;
}

template<typename T>
NzVector3<T>& NzVector3<T>::operator*=(T scale)
{
	x *= scale;
	y *= scale;
	z *= scale;

	return *this;
}

template<typename T>
NzVector3<T>& NzVector3<T>::operator/=(const NzVector3& vec)
{
	if (vec.x == 0.f || vec.y == 0.f || vec.z == 0.f)
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Division by zero";

		throw std::domain_error(ss.ToString());
	}

	x /= vec.x;
	y /= vec.y;
	z /= vec.z;

	return *this;
}

template<typename T>
NzVector3<T>& NzVector3<T>::operator/=(T scale)
{
	if (scale == 0.f)
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Division by zero";

		throw std::domain_error(ss.ToString());
	}

	x /= scale;
	y /= scale;
	z /= scale;

	return *this;
}

template<typename T>
bool NzVector3<T>::operator==(const NzVector3& vec) const
{
	return NzNumberEquals(x, vec.x) &&
		   NzNumberEquals(y, vec.y) &&
		   NzNumberEquals(z, vec.z);
}

template<typename T>
bool NzVector3<T>::operator!=(const NzVector3& vec) const
{
	return !operator==(vec);
}

template<typename T>
bool NzVector3<T>::operator<(const NzVector3& vec) const
{
	return x < vec.x && y < vec.y && z < vec.z;
}

template<typename T>
bool NzVector3<T>::operator<=(const NzVector3& vec) const
{
	return operator<(vec) || operator==(vec);
}

template<typename T>
bool NzVector3<T>::operator>(const NzVector3& vec) const
{
	return !operator<=(vec);
}

template<typename T>
bool NzVector3<T>::operator>=(const NzVector3& vec) const
{
	return !operator<(vec);
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzVector3<T>& vec)
{
	return out << vec.ToString();
}

template<typename T>
NzVector3<T> operator*(T scale, const NzVector3<T>& vec)
{
	return NzVector3<T>(scale * vec.x, scale * vec.y, scale * vec.z);
}

template<typename T>
NzVector3<T> operator/(T scale, const NzVector3<T>& vec)
{
	if (vec.x == 0.f || vec.y == 0.f || vec.z == 0.f)
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Division by zero";

		throw std::domain_error(ss.ToString());
	}

	return NzVector3<T>(scale / vec.x, scale / vec.y, scale / vec.z);
}

#include <Nazara/Core/DebugOff.hpp>
