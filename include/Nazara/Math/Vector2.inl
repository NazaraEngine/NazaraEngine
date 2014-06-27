// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Basic.hpp>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)

template<typename T>
NzVector2<T>::NzVector2(T X, T Y)
{
	Set(X, Y);
}

template<typename T>
NzVector2<T>::NzVector2(T scale)
{
	Set(scale);
}

template<typename T>
NzVector2<T>::NzVector2(const T vec[2])
{
	Set(vec);
}

template<typename T>
template<typename U>
NzVector2<T>::NzVector2(const NzVector2<U>& vec)
{
	Set(vec);
}

template<typename T>
T NzVector2<T>::AbsDotProduct(const NzVector2& vec) const
{
	return std::fabs(x * vec.x) + std::fabs(y * vec.y);
}

template<>
inline int NzVector2<int>::AbsDotProduct(const NzVector2<int>& vec) const
{
	return std::labs(x * vec.x) + std::labs(y * vec.y);
}

template<>
inline unsigned int NzVector2<unsigned int>::AbsDotProduct(const NzVector2<unsigned int>& vec) const
{
	return std::labs(x * vec.x) + std::labs(y * vec.y);
}

template<typename T>
T NzVector2<T>::Distance(const NzVector2& vec) const
{
	return std::sqrt(SquaredDistance(vec));
}

template<typename T>
float NzVector2<T>::Distancef(const NzVector2& vec) const
{
	return std::sqrt(static_cast<float>(SquaredDistance(vec)));
}

template<typename T>
T NzVector2<T>::DotProduct(const NzVector2& vec) const
{
	return x*vec.x + y*vec.y;
}

template<typename T>
T NzVector2<T>::GetLength() const
{
	return std::sqrt(GetSquaredLength());
}

template<typename T>
float NzVector2<T>::GetLengthf() const
{
	return std::sqrt(static_cast<float>(GetSquaredLength()));
}

template<typename T>
NzVector2<T> NzVector2<T>::GetNormal(T* length) const
{
	NzVector2 vec(*this);
	vec.Normalize(length);

	return vec;
}

template<typename T>
T NzVector2<T>::GetSquaredLength() const
{
	return x*x + y*y;
}

template<typename T>
NzVector2<T>& NzVector2<T>::MakeUnitX()
{
	return Set(F(1.0), F(0.0));
}

template<typename T>
NzVector2<T>& NzVector2<T>::MakeUnitY()
{
	return Set(F(0.0), F(1.0));
}

template<typename T>
NzVector2<T>& NzVector2<T>::MakeZero()
{
	return Set(F(0.0), F(0.0));
}

template<typename T>
NzVector2<T>& NzVector2<T>::Maximize(const NzVector2& vec)
{
	if (vec.x > x)
		x = vec.x;

	if  (vec.y > y)
		y = vec.y;

	return *this;
}

template<typename T>
NzVector2<T>& NzVector2<T>::Minimize(const NzVector2& vec)
{
	if (vec.x < x)
		x = vec.x;

	if  (vec.y < y)
		y = vec.y;

	return *this;
}

template<typename T>
NzVector2<T>& NzVector2<T>::Normalize(T* length)
{
	T norm = std::sqrt(GetSquaredLength());
	T invNorm = F(1.0) / norm;

	x *= invNorm;
	y *= invNorm;

	if (length)
		*length = norm;

	return *this;
}

template<typename T>
NzVector2<T>& NzVector2<T>::Set(T X, T Y)
{
	x = X;
	y = Y;

	return *this;
}

template<typename T>
NzVector2<T>& NzVector2<T>::Set(T scale)
{
	x = scale;
	y = scale;

	return *this;
}

template<typename T>
NzVector2<T>& NzVector2<T>::Set(const T vec[2])
{
	std::memcpy(&x, vec, 2*sizeof(T));

	return *this;
}

template<typename T>
NzVector2<T>& NzVector2<T>::Set(const NzVector2& vec)
{
	std::memcpy(this, &vec, sizeof(NzVector2));

	return *this;
}

template<typename T>
template<typename U>
NzVector2<T>& NzVector2<T>::Set(const NzVector2<U>& vec)
{
	x = F(vec.x);
	y = F(vec.y);

	return *this;
}

template<typename T>
T NzVector2<T>::SquaredDistance(const NzVector2& vec) const
{
	return operator-(vec).GetSquaredLength();
}

template<typename T>
NzString NzVector2<T>::ToString() const
{
	NzStringStream ss;

	return ss << "Vector2(" << x << ", " << y << ')';
}

template<typename T>
NzVector2<T>::operator T*()
{
	return &x;
}

template<typename T>
NzVector2<T>::operator const T*() const
{
	return &x;
}

template<typename T>
const NzVector2<T>& NzVector2<T>::operator+() const
{
	return *this;
}

template<typename T>
NzVector2<T> NzVector2<T>::operator-() const
{
	return NzVector2(-x, -y);
}

template<typename T>
NzVector2<T> NzVector2<T>::operator+(const NzVector2& vec) const
{
	return NzVector2(x + vec.x, y + vec.y);
}

template<typename T>
NzVector2<T> NzVector2<T>::operator-(const NzVector2& vec) const
{
	return NzVector2(x - vec.x, y - vec.y);
}

template<typename T>
NzVector2<T> NzVector2<T>::operator*(const NzVector2& vec) const
{
	return NzVector2(x * vec.x, y * vec.y);
}

template<typename T>
NzVector2<T> NzVector2<T>::operator*(T scale) const
{
	return NzVector2(x * scale, y * scale);
}

template<typename T>
NzVector2<T> NzVector2<T>::operator/(const NzVector2& vec) const
{
	#if NAZARA_MATH_SAFE
	if (NzNumberEquals(vec.x, F(0.0)) || NzNumberEquals(vec.y, F(0.0)))
	{
		NzString error("Division by zero");

		NazaraError(error);
		throw std::domain_error(error);
	}
	#endif

	return NzVector2(x / vec.x, y / vec.y);
}

template<typename T>
NzVector2<T> NzVector2<T>::operator/(T scale) const
{
	#if NAZARA_MATH_SAFE
	if (NzNumberEquals(scale, F(0.0)))
	{
		NzString error("Division by zero");

		NazaraError(error);
		throw std::domain_error(error);
	}
	#endif

	return NzVector2(x / scale, y / scale);
}

template<typename T>
NzVector2<T>& NzVector2<T>::operator+=(const NzVector2& vec)
{
	x += vec.x;
	y += vec.y;

	return *this;
}

template<typename T>
NzVector2<T>& NzVector2<T>::operator-=(const NzVector2& vec)
{
	x -= vec.x;
	y -= vec.y;

	return *this;
}

template<typename T>
NzVector2<T>& NzVector2<T>::operator*=(const NzVector2& vec)
{
	x *= vec.x;
	y *= vec.y;

	return *this;
}

template<typename T>
NzVector2<T>& NzVector2<T>::operator*=(T scale)
{
	x *= scale;
	y *= scale;

	return *this;
}

template<typename T>
NzVector2<T>& NzVector2<T>::operator/=(const NzVector2& vec)
{
	#if NAZARA_MATH_SAFE
	if (NzNumberEquals(vec.x, F(0.0)) || NzNumberEquals(vec.y, F(0.0)))
	{
		NzString error("Division by zero");

		NazaraError(error);
		throw std::domain_error(error);
	}
	#endif

	x /= vec.x;
	y /= vec.y;

	return *this;
}

template<typename T>
NzVector2<T>& NzVector2<T>::operator/=(T scale)
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

	return *this;
}

template<typename T>
bool NzVector2<T>::operator==(const NzVector2& vec) const
{
	return NzNumberEquals(x, vec.x) &&
		   NzNumberEquals(y, vec.y);
}

template<typename T>
bool NzVector2<T>::operator!=(const NzVector2& vec) const
{
	return !operator==(vec);
}

template<typename T>
bool NzVector2<T>::operator<(const NzVector2& vec) const
{
	if (x == vec.x)
		return y < vec.y;
	else
		return x < vec.x;
}

template<typename T>
bool NzVector2<T>::operator<=(const NzVector2& vec) const
{
	if (x == vec.x)
		return y <= vec.y;
	else
		return x < vec.x;
}

template<typename T>
bool NzVector2<T>::operator>(const NzVector2& vec) const
{
	return !operator<=(vec);
}

template<typename T>
bool NzVector2<T>::operator>=(const NzVector2& vec) const
{
	return !operator<(vec);
}

template<typename T>
NzVector2<T> NzVector2<T>::Lerp(const NzVector2& from, const NzVector2& to, T interpolation)
{
	return NzLerp(from, to, interpolation);
}

template<typename T>
NzVector2<T> NzVector2<T>::UnitX()
{
	NzVector2 vector;
	vector.MakeUnitX();

	return vector;
}

template<typename T>
NzVector2<T> NzVector2<T>::UnitY()
{
	NzVector2 vector;
	vector.MakeUnitY();

	return vector;
}

template<typename T>
NzVector2<T> NzVector2<T>::Zero()
{
	NzVector2 vector;
	vector.MakeZero();

	return vector;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzVector2<T>& vec)
{
	return out << vec.ToString();
}

template<typename T>
NzVector2<T> operator*(T scale, const NzVector2<T>& vec)
{
	return NzVector2<T>(scale * vec.x, scale * vec.y);
}

template<typename T>
NzVector2<T> operator/(T scale, const NzVector2<T>& vec)
{
	#if NAZARA_MATH_SAFE
	if (NzNumberEquals(vec.x, F(0.0)) || NzNumberEquals(vec.y, F(0.0)))
	{
		NzString error("Division by zero");

		NazaraError(error);
		throw std::domain_error(error);
	}
	#endif

	return NzVector2<T>(scale/vec.x, scale/vec.y);
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
