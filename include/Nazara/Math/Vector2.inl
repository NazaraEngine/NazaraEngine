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
NzVector2<T>::NzVector2()
{
}

template<typename T>
NzVector2<T>::NzVector2(T X, T Y) :
x(X),
y(Y)
{
}

template<typename T>
NzVector2<T>::NzVector2(T scale) :
x(scale),
y(scale)
{
}

template<typename T>
NzVector2<T>::NzVector2(T vec[2]) :
x(vec[0]),
y(vec[1])
{
}

template<typename T>
template<typename U>
NzVector2<T>::NzVector2(const NzVector2<U>& vec) :
x(static_cast<T>(vec.x)),
y(static_cast<T>(vec.y))
{
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
	return x * vec.x + y * vec.y;
}

template<typename T>
NzVector2<T> NzVector2<T>::GetNormal() const
{
	NzVector2 vec(*this);
	vec.Normalize();

	return vec;
}

template<typename T>
void NzVector2<T>::MakeCeil(const NzVector2& vec)
{
	if (vec.x > x)
		x = vec.x;

	if  (vec.y > y)
		y = vec.y;
}

template<typename T>
void NzVector2<T>::MakeFloor(const NzVector2& vec)
{
	if (vec.x < x)
		x = vec.x;

	if  (vec.y < y)
		y = vec.y;
}

template<typename T>
T NzVector2<T>::Length() const
{
	return std::sqrt(SquaredLength());
}

template<typename T>
float NzVector2<T>::Lengthf() const
{
	return std::sqrt(static_cast<float>(SquaredLength()));
}

template<typename T>
void NzVector2<T>::Normalize()
{
	auto length = Length();

	if (!NzNumberEquals(length, static_cast<T>(0.0)))
	{
		x /= length;
		y /= length;
	}
}

template<typename T>
T NzVector2<T>::SquaredDistance(const NzVector2& vec) const
{
	return operator-(vec).SquaredLength();
}

template<typename T>
T NzVector2<T>::SquaredLength() const
{
	return x * x + y * y;
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
T& NzVector2<T>::operator[](unsigned int i)
{
	if (i >= 2)
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Index out of range (" << i << " >= 2)";

		throw std::domain_error(ss.ToString());
	}

	return *(&x+i);
}

template<typename T>
T NzVector2<T>::operator[](unsigned int i) const
{
	if (i >= 2)
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Index out of range (" << i << " >= 2)";

		throw std::domain_error(ss.ToString());
	}

	return *(&x+i);
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
	if (NzNumberEquals(vec.x, static_cast<T>(0.0)) || NzNumberEquals(vec.y, static_cast<T>(0.0)))
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Division by zero";

		throw std::domain_error(ss.ToString());
	}

	return NzVector2(x / vec.x, y / vec.y);
}

template<typename T>
NzVector2<T> NzVector2<T>::operator/(T scale) const
{
	if (NzNumberEquals(scale, static_cast<T>(0.0)))
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Division by zero";

		throw std::domain_error(ss.ToString());
	}

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
	if (NzNumberEquals(vec.x, static_cast<T>(0.0)) || NzNumberEquals(vec.y, static_cast<T>(0.0)) || NzNumberEquals(vec.z, static_cast<T>(0.0)))
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Division by zero";

		throw std::domain_error(ss.ToString());
	}

	x /= vec.x;
	y /= vec.y;

	return *this;
}

template<typename T>
NzVector2<T>& NzVector2<T>::operator/=(T scale)
{
	if (NzNumberEquals(scale, static_cast<T>(0.0)))
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Division by zero";

		throw std::domain_error(ss.ToString());
	}

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
	return x < vec.x && y < vec.y;
}

template<typename T>
bool NzVector2<T>::operator<=(const NzVector2& vec) const
{
	return operator<(vec) || operator==(vec);
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
	if (NzNumberEquals(vec.x, static_cast<T>(0.0)) || NzNumberEquals(vec.y, static_cast<T>(0.0)) || NzNumberEquals(vec.z, static_cast<T>(0.0)))
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Division by zero";

		throw std::domain_error(ss.ToString());
	}

	return NzVector2<T>(scale/vec.x, scale/vec.y);
}

#include <Nazara/Core/DebugOff.hpp>
