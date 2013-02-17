// Copyright (C) 2012 Rémi Bèges - Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Basic.hpp>
#include <cstring>
#include <stdexcept>
#include <Nazara/Core/Debug.hpp>

///FIXME: Les calculs effectués ici sont probablements tous faux, la composante W étant spéciale dans le monde de la 3D

#define F(a) static_cast<T>(a)

template<typename T>
NzVector4<T>::NzVector4(T X, T Y, T Z, T W)
{
	Set(X, Y, Z, W);
}

template<typename T>
NzVector4<T>::NzVector4(T scale)
{
	Set(scale);
}

template<typename T>
NzVector4<T>::NzVector4(const T vec[4])
{
	Set(vec);
}

template<typename T>
NzVector4<T>::NzVector4(const NzVector3<T>& vec, T W)
{
	Set(vec, W);
}

template<typename T>
template<typename U>
NzVector4<T>::NzVector4(const NzVector4<U>& vec)
{
	Set(vec);
}

template<typename T>
T NzVector4<T>::AbsDotProduct(const NzVector4& vec) const
{
	return std::fabs(x * vec.x) + std::fabs(y * vec.y) + std::fabs(z * vec.z) + std::fabs(w * vec.w);
}

template<>
inline int NzVector4<int>::AbsDotProduct(const NzVector4<int>& vec) const
{
	return std::labs(x * vec.x) + std::labs(y * vec.y) + std::labs(z * vec.z) + std::labs(w * vec.w);
}

template<>
inline unsigned int NzVector4<unsigned int>::AbsDotProduct(const NzVector4<unsigned int>& vec) const
{
	return std::labs(x * vec.x) + std::labs(y * vec.y) + std::labs(z * vec.z) + std::labs(w * vec.w);
}

template<typename T>
T NzVector4<T>::DotProduct(const NzVector4& vec) const
{
	return x*vec.x + y*vec.y + z*vec.z + w*vec.w;
}

template<typename T>
NzVector4<T>& NzVector4<T>::MakeUnitX()
{
	return Set(F(1.0), F(0.0), F(0.0), F(1.0));
}

template<typename T>
NzVector4<T>& NzVector4<T>::MakeUnitY()
{
	return Set(F(0.0), F(1.0), F(0.0), F(1.0));
}

template<typename T>
NzVector4<T>& NzVector4<T>::MakeUnitZ()
{
	return Set(F(0.0), F(0.0), F(1.0), F(1.0));
}

template<typename T>
NzVector4<T>& NzVector4<T>::MakeZero()
{
	return Set(F(0.0), F(0.0), F(0.0), F(0.0));
}

template<typename T>
NzVector4<T>& NzVector4<T>::Maximize(const NzVector4& vec)
{
	if (vec.x > x)
		x = vec.x;

	if (vec.y > y)
		y = vec.y;

    if (vec.z > z)
        z = vec.z;

    if (vec.w > w)
        w = vec.w;

	return *this;
}

template<typename T>
NzVector4<T>& NzVector4<T>::Minimize(const NzVector4& vec)
{
	if (vec.x < x)
		x = vec.x;

	if (vec.y < y)
		y = vec.y;

    if (vec.z < z)
        z = vec.z;

    if (vec.w < w)
        w = vec.w;

	return *this;
}

template<typename T>
NzVector4<T>& NzVector4<T>::Normalize(T* length)
{
	x /= w;
	y /= w;
	z /= w;

	w = F(1.0);

	if (length)
		*length = w;

	return *this;
}

template<typename T>
NzVector4<T>& NzVector4<T>::Set(T X, T Y, T Z, T W)
{
	w = W;
	x = X;
	y = Y;
	z = Z;

	return *this;
}

template<typename T>
NzVector4<T>& NzVector4<T>::Set(T scale)
{
	w = scale;
	x = scale;
	y = scale;
	z = scale;

	return *this;
}

template<typename T>
NzVector4<T>& NzVector4<T>::Set(const T vec[4])
{
	std::memcpy(&x, vec, 4*sizeof(T));

	return *this;
}

template<typename T>
NzVector4<T>& NzVector4<T>::Set(const NzVector3<T>& vec, T W)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
	w = W;

	return *this;
}

template<typename T>
NzVector4<T>& NzVector4<T>::Set(const NzVector4& vec)
{
	std::memcpy(this, &vec, sizeof(NzVector4));

	return *this;
}

template<typename T>
template<typename U>
NzVector4<T>& NzVector4<T>::Set(const NzVector4<U>& vec)
{
	w = F(vec.w);
	x = F(vec.x);
	y = F(vec.y);
	z = F(vec.z);

	return *this;
}

template<typename T>
NzString NzVector4<T>::ToString() const
{
	NzStringStream ss;

	return ss << "Vector4(" << x << ", " << y << ", " << z << ", " << w << ')';
}

template<typename T>
NzVector4<T>::operator T*()
{
	return &x;
}

template<typename T>
NzVector4<T>::operator const T*() const
{
	return &x;
}

template<typename T>
T& NzVector4<T>::operator[](unsigned int i)
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
T NzVector4<T>::operator[](unsigned int i) const
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
	return NzVector4(x - vec.x, y - vec.y, z - vec.z, w - vec.w);
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
	#if NAZARA_MATH_SAFE
	if (NzNumberEquals(vec.x, F(0.0)) || NzNumberEquals(vec.y, F(0.0)) || NzNumberEquals(vec.z, F(0.0)) || NzNumberEquals(vec.w, F(0.0)))
	{
		NzString error("Division by zero");

		NazaraError(error);
		throw std::domain_error(error);
	}
	#endif

	return NzVector4(x / vec.x, y / vec.y, z / vec.z, w / vec.w);
}

template<typename T>
NzVector4<T> NzVector4<T>::operator/(T scale) const
{
	#if NAZARA_MATH_SAFE
	if (NzNumberEquals(scale, F(0.0)))
	{
		NzString error("Division by zero");

		NazaraError(error);
		throw std::domain_error(error);
	}
	#endif

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
	#if NAZARA_MATH_SAFE
	if (NzNumberEquals(vec.x, F(0.0)) || NzNumberEquals(vec.y, F(0.0)) || NzNumberEquals(vec.z, F(0.0)) || NzNumberEquals(vec.w, F(0.0)))
	{
		NzString error("Division by zero");

		NazaraError(error);
		throw std::domain_error(error);
	}
	#endif

	x /= vec.x;
	y /= vec.y;
	z /= vec.z;
	w /= vec.w;

	return *this;
}

template<typename T>
NzVector4<T>& NzVector4<T>::operator/=(T scale)
{
	#if NAZARA_MATH_SAFE
	if (NzNumberEquals(scale, F(0.0)))
	{
		NzString error("Division by zero");

		NazaraError(error);
		throw std::domain_error(error);
	}
	#endif

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
	if (x == vec.x)
	{
		if (y == vec.y)
		{
			if (z == vec.z)
				return w < vec.w;
			else
				return z < vec.z;
		}
		else
			return y < vec.y;
	}
	else
		return x < vec.x;
}

template<typename T>
bool NzVector4<T>::operator<=(const NzVector4& vec) const
{
	if (x == vec.x)
	{
		if (y == vec.y)
		{
			if (z == vec.z)
				return w <= vec.w;
			else
				return z < vec.z;
		}
		else
			return y < vec.y;
	}
	else
		return x < vec.x;
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
NzVector4<T> NzVector4<T>::UnitX()
{
	NzVector4 vector;
	vector.MakeUnitX();

	return vector;
}

template<typename T>
NzVector4<T> NzVector4<T>::UnitY()
{
	NzVector4 vector;
	vector.MakeUnitY();

	return vector;
}

template<typename T>
NzVector4<T> NzVector4<T>::UnitZ()
{
	NzVector4 vector;
	vector.MakeUnitZ();

	return vector;
}

template<typename T>
NzVector4<T> NzVector4<T>::Zero()
{
	NzVector4 vector;
	vector.MakeZero();

	return vector;
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
	#if NAZARA_MATH_SAFE
	if (NzNumberEquals(vec.x, F(0.0)) || NzNumberEquals(vec.y, F(0.0)) || NzNumberEquals(vec.z, F(0.0)) || NzNumberEquals(vec.w, F(0.0)))
	{
		NzString error("Division by zero");

		NazaraError(error);
		throw std::domain_error(error);
	}
	#endif

	return NzVector4<T>(scale / vec.x, scale / vec.y, scale / vec.z, scale / vec.w);
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
