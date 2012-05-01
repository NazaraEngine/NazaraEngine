// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Basic.hpp>
#include <cmath>
#include <cstdlib>
#include <stdexcept>
#include <Nazara/Core/Debug.hpp>

template<typename T>
NzVector4<T>::NzVector4()
{
}

template<typename T>
NzVector4<T>::NzVector4(T X, T Y, T Z, T W) :
x(X),
y(Y),
z(Z),
w(W)
{
}

template<typename T>
NzVector4<T>::NzVector4(T scale) :
x(scale),
y(scale),
z(scale),
w(scale)
{
}

template<typename T>
NzVector4<T>::NzVector4(T vec[4]) :
x(vec[0]),
y(vec[1]),
z(vec[2]),
w(vec[3])
{
}

template<typename T>
template<typename U>
NzVector4<T>::NzVector4(const NzVector4<U>& vec) :
x(static_cast<T>(vec.x)),
y(static_cast<T>(vec.y)),
z(static_cast<T>(vec.z)),
w(static_cast<T>(vec.w))
{
}

template<typename T>
T NzVector4<T>::AbsDotProduct(const NzVector4& vec) const
{
	return std::fabs(x * vec.x) + std::fabs(y * vec.y) + std::fabs(z * vec.z) + std::fabs(w * vec.w);
}

template<> inline int NzVector4<int>::AbsDotProduct(const NzVector4<int>& vec) const
{
	return std::labs(x * vec.x) + std::labs(y * vec.y) + std::labs(z * vec.z) + std::labs(w * vec.w);
}

template<typename T>
T NzVector4<T>::DotProduct(const NzVector4& vec) const
{
	return x * vec.x + y * vec.y + z * vec.z + w * vec.w;
}

template<typename T>
void NzVector4<T>::MakeCeil(const NzVector4& vec)
{
	if (vec.x > x)
		x = vec.x;

	if (vec.y > y)
		y = vec.y;

    if (vec.z > z)
        z = vec.z;

    if (vec.w > w)
        w = vec.w;
}

template<typename T>
void NzVector4<T>::MakeFloor(const NzVector4& vec)
{
	if (vec.x < x)
		x = vec.x;

	if (vec.y < y)
		y = vec.y;

    if (vec.z < z)
        z = vec.z;

    if (vec.w < w)
        w = vec.w;
}

template<typename T>
void NzVector4<T>::Normalize()
{
	if (w != 0.f)
	{
		x /= w;
		y /= w;
		z /= w;
	}
}

template<typename T>
NzString NzVector4<T>::ToString() const
{
	NzStringStream ss;

	return ss << "Vector4(" << x << ", " << y << ", " << z <<')';
}

template<typename T>
NzVector4<T>::operator NzString() const
{
	return ToString();
}

template<typename T>
T& NzVector4<T>::operator[](unsigned int i)
{
	if (i >= 4)
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Index out of range (" << i << " >= 4)";

		throw std::domain_error(ss.ToString());
	}

	return *(&x+i);
}

template<typename T>
T NzVector4<T>::operator[](unsigned int i) const
{
	if (i >= 4)
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Index out of range (" << i << " >= 4)";

		throw std::domain_error(ss.ToString());
	}

	return *(&x+i);
}

template<typename T>
const NzVector4<T>& NzVector4<T>::operator+() const
{
	return *this;
}

template<typename T>
NzVector4<T> NzVector4<T>::operator-() const
{
	return NzVector4(-x, -y, -z, -w);
}

template<typename T>
NzVector4<T> NzVector4<T>::operator+(const NzVector4& vec) const
{
	return NzVector4(x + vec.x, y + vec.y, z + vec.z, w + vec.w);
}

template<typename T>
NzVector4<T> NzVector4<T>::operator-(const NzVector4& vec) const
{
	return NzVector4(x - vec.x, y - vec.y, z - vec.z);
}

template<typename T>
NzVector4<T> NzVector4<T>::operator*(const NzVector4& vec) const
{
	return NzVector4(x * vec.x, y * vec.y, z * vec.z, w * vec.w);
}

template<typename T>
NzVector4<T> NzVector4<T>::operator*(T scale) const
{
	return NzVector4(x * scale, y * scale, z * scale, w * scale);
}

template<typename T>
NzVector4<T> NzVector4<T>::operator/(const NzVector4& vec) const
{
	if (vec.x == 0.f || vec.y == 0.f || vec.z == 0.f || vec.w == 0.f)
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Division by zero";

		throw std::domain_error(ss.ToString());
	}

	return NzVector4(x / vec.x, y / vec.y, z / vec.z, w / vec.w);
}

template<typename T>
NzVector4<T> NzVector4<T>::operator/(T scale) const
{
	if (scale == 0.f)
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Division by zero";

		throw std::domain_error(ss.ToString());
	}

	return NzVector4(x / scale, y / scale, z / scale, w / scale);
}

template<typename T>
NzVector4<T>& NzVector4<T>::operator+=(const NzVector4& vec)
{
	x += vec.x;
	y += vec.y;
	z += vec.z;
	w += vec.w;

	return *this;
}

template<typename T>
NzVector4<T>& NzVector4<T>::operator-=(const NzVector4& vec)
{
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;
	w -= vec.w;

	return *this;
}

template<typename T>
NzVector4<T>& NzVector4<T>::operator*=(const NzVector4& vec)
{
	x *= vec.x;
	y *= vec.y;
	z *= vec.z;
	w *= vec.w;

	return *this;
}

template<typename T>
NzVector4<T>& NzVector4<T>::operator*=(T scale)
{
	x *= scale;
	y *= scale;
	z *= scale;
	w *= scale;

	return *this;
}

template<typename T>
NzVector4<T>& NzVector4<T>::operator/=(const NzVector4& vec)
{
	if (vec.x == 0.f || vec.y == 0.f || vec.z == 0.f || vec.w == 0.f)
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Division by zero";

		throw std::domain_error(ss.ToString());
	}

	x /= vec.x;
	y /= vec.y;
	z /= vec.z;
	w /= vec.w;

	return *this;
}

template<typename T>
NzVector4<T>& NzVector4<T>::operator/=(T scale)
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
	w /= scale;

	return *this;
}

template<typename T>
bool NzVector4<T>::operator==(const NzVector4& vec) const
{
	return NzNumberEquals(x, vec.x) &&
		   NzNumberEquals(y, vec.y) &&
		   NzNumberEquals(z, vec.z) &&
		   NzNumberEquals(w, vec.w);
}

template<typename T>
bool NzVector4<T>::operator!=(const NzVector4& vec) const
{
	return !operator==(vec);
}

template<typename T>
bool NzVector4<T>::operator<(const NzVector4& vec) const
{
	return x < vec.x && y < vec.y && z < vec.z && w < vec.w;
}

template<typename T>
bool NzVector4<T>::operator<=(const NzVector4& vec) const
{
	return operator<(vec) || operator==(vec);
}

template<typename T>
bool NzVector4<T>::operator>(const NzVector4& vec) const
{
	return !operator<=(vec);
}

template<typename T>
bool NzVector4<T>::operator>=(const NzVector4& vec) const
{
	return !operator<(vec);
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzVector4<T>& vec)
{
	return out << vec.ToString();
}

template<typename T>
NzVector4<T> operator*(T scale, const NzVector4<T>& vec)
{
	return NzVector4<T>(scale * vec.x, scale * vec.y, scale * vec.z, scale * vec.w);
}

template<typename T>
NzVector4<T> operator/(T scale, const NzVector4<T>& vec)
{
	if (vec.x == 0.f || vec.y == 0.f || vec.z == 0.f || vec.w == 0.f)
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Division by zero";

		throw std::domain_error(ss.ToString());
	}

	return NzVector3<T>(scale / vec.x, scale / vec.y, scale / vec.z, scale / vec.w);
}

#include <Nazara/Core/DebugOff.hpp>
