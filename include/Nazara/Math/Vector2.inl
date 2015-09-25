// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)

namespace Nz
{
	template<typename T>
	Vector2<T>::Vector2(T X, T Y)
	{
		Set(X, Y);
	}

	template<typename T>
	Vector2<T>::Vector2(T scale)
	{
		Set(scale);
	}

	template<typename T>
	Vector2<T>::Vector2(const T vec[2])
	{
		Set(vec);
	}

	template<typename T>
	template<typename U>
	Vector2<T>::Vector2(const Vector2<U>& vec)
	{
		Set(vec);
	}

	template<typename T>
	Vector2<T>::Vector2(const Vector3<T>& vec)
	{
		Set(vec);
	}

	template<typename T>
	Vector2<T>::Vector2(const Vector4<T>& vec)
	{
		Set(vec);
	}

	template<typename T>
	T Vector2<T>::AbsDotProduct(const Vector2& vec) const
	{
		return std::abs(x * vec.x) + std::abs(y * vec.y);
	}

	template<typename T>
	T Vector2<T>::AngleBetween(const Vector2& vec) const
	{
		return FromRadians(std::atan2(vec.y, vec.x) - std::atan2(y, x));
	}

	template<typename T>
	T Vector2<T>::Distance(const Vector2& vec) const
	{
		return std::sqrt(SquaredDistance(vec));
	}

	template<typename T>
	float Vector2<T>::Distancef(const Vector2& vec) const
	{
		return std::sqrt(static_cast<float>(SquaredDistance(vec)));
	}

	template<typename T>
	T Vector2<T>::DotProduct(const Vector2& vec) const
	{
		return x*vec.x + y*vec.y;
	}

	template<typename T>
	T Vector2<T>::GetLength() const
	{
		return std::sqrt(GetSquaredLength());
	}

	template<typename T>
	float Vector2<T>::GetLengthf() const
	{
		return std::sqrt(static_cast<float>(GetSquaredLength()));
	}

	template<typename T>
	Vector2<T> Vector2<T>::GetNormal(T* length) const
	{
		Vector2 vec(*this);
		vec.Normalize(length);

		return vec;
	}

	template<typename T>
	T Vector2<T>::GetSquaredLength() const
	{
		return x*x + y*y;
	}

	template<typename T>
	Vector2<T>& Vector2<T>::MakeUnit()
	{
		return Set(F(1.0), F(1.0));
	}

	template<typename T>
	Vector2<T>& Vector2<T>::MakeUnitX()
	{
		return Set(F(1.0), F(0.0));
	}

	template<typename T>
	Vector2<T>& Vector2<T>::MakeUnitY()
	{
		return Set(F(0.0), F(1.0));
	}

	template<typename T>
	Vector2<T>& Vector2<T>::MakeZero()
	{
		return Set(F(0.0), F(0.0));
	}

	template<typename T>
	Vector2<T>& Vector2<T>::Maximize(const Vector2& vec)
	{
		if (vec.x > x)
			x = vec.x;

		if (vec.y > y)
			y = vec.y;

		return *this;
	}

	template<typename T>
	Vector2<T>& Vector2<T>::Minimize(const Vector2& vec)
	{
		if (vec.x < x)
			x = vec.x;

		if (vec.y < y)
			y = vec.y;

		return *this;
	}

	template<typename T>
	Vector2<T>& Vector2<T>::Normalize(T* length)
	{
		T norm = GetLength();
		if (norm > F(0.0))
		{
			T invNorm = F(1.0) / norm;
			x *= invNorm;
			y *= invNorm;
		}

		if (length)
			*length = norm;

		return *this;
	}

	template<typename T>
	Vector2<T>& Vector2<T>::Set(T X, T Y)
	{
		x = X;
		y = Y;

		return *this;
	}

	template<typename T>
	Vector2<T>& Vector2<T>::Set(T scale)
	{
		x = scale;
		y = scale;

		return *this;
	}

	template<typename T>
	Vector2<T>& Vector2<T>::Set(const T vec[2])
	{
		std::memcpy(&x, vec, 2*sizeof(T));

		return *this;
	}

	template<typename T>
	Vector2<T>& Vector2<T>::Set(const Vector2& vec)
	{
		std::memcpy(this, &vec, sizeof(Vector2));

		return *this;
	}

	template<typename T>
	template<typename U>
	Vector2<T>& Vector2<T>::Set(const Vector2<U>& vec)
	{
		x = F(vec.x);
		y = F(vec.y);

		return *this;
	}

	template<typename T>
	Vector2<T>& Vector2<T>::Set(const Vector3<T>& vec)
	{
		x = vec.x;
		y = vec.y;

		return *this;
	}

	template<typename T>
	Vector2<T>& Vector2<T>::Set(const Vector4<T>& vec)
	{
		x = vec.x;
		y = vec.y;

		return *this;
	}

	template<typename T>
	T Vector2<T>::SquaredDistance(const Vector2& vec) const
	{
		return (*this - vec).GetSquaredLength();
	}

	template<typename T>
	String Vector2<T>::ToString() const
	{
		StringStream ss;

		return ss << "Vector2(" << x << ", " << y << ')';
	}

	template<typename T>
	Vector2<T>::operator T*()
	{
		return &x;
	}

	template<typename T>
	Vector2<T>::operator const T*() const
	{
		return &x;
	}

	template<typename T>
	const Vector2<T>& Vector2<T>::operator+() const
	{
		return *this;
	}

	template<typename T>
	Vector2<T> Vector2<T>::operator-() const
	{
		return Vector2(-x, -y);
	}

	template<typename T>
	Vector2<T> Vector2<T>::operator+(const Vector2& vec) const
	{
		return Vector2(x + vec.x, y + vec.y);
	}

	template<typename T>
	Vector2<T> Vector2<T>::operator-(const Vector2& vec) const
	{
		return Vector2(x - vec.x, y - vec.y);
	}

	template<typename T>
	Vector2<T> Vector2<T>::operator*(const Vector2& vec) const
	{
		return Vector2(x * vec.x, y * vec.y);
	}

	template<typename T>
	Vector2<T> Vector2<T>::operator*(T scale) const
	{
		return Vector2(x * scale, y * scale);
	}

	template<typename T>
	Vector2<T> Vector2<T>::operator/(const Vector2& vec) const
	{
		#if NAZARA_MATH_SAFE
		if (NumberEquals(vec.x, F(0.0)) || NumberEquals(vec.y, F(0.0)))
		{
			String error("Division by zero");

			NazaraError(error);
			throw std::domain_error(error);
		}
		#endif

		return Vector2(x / vec.x, y / vec.y);
	}

	template<typename T>
	Vector2<T> Vector2<T>::operator/(T scale) const
	{
		#if NAZARA_MATH_SAFE
		if (NumberEquals(scale, F(0.0)))
		{
			String error("Division by zero");

			NazaraError(error);
			throw std::domain_error(error);
		}
		#endif

		return Vector2(x / scale, y / scale);
	}

	template<typename T>
	Vector2<T>& Vector2<T>::operator+=(const Vector2& vec)
	{
		x += vec.x;
		y += vec.y;

		return *this;
	}

	template<typename T>
	Vector2<T>& Vector2<T>::operator-=(const Vector2& vec)
	{
		x -= vec.x;
		y -= vec.y;

		return *this;
	}

	template<typename T>
	Vector2<T>& Vector2<T>::operator*=(const Vector2& vec)
	{
		x *= vec.x;
		y *= vec.y;

		return *this;
	}

	template<typename T>
	Vector2<T>& Vector2<T>::operator*=(T scale)
	{
		x *= scale;
		y *= scale;

		return *this;
	}

	template<typename T>
	Vector2<T>& Vector2<T>::operator/=(const Vector2& vec)
	{
		#if NAZARA_MATH_SAFE
		if (NumberEquals(vec.x, F(0.0)) || NumberEquals(vec.y, F(0.0)))
		{
			String error("Division by zero");

			NazaraError(error);
			throw std::domain_error(error);
		}
		#endif

		x /= vec.x;
		y /= vec.y;

		return *this;
	}

	template<typename T>
	Vector2<T>& Vector2<T>::operator/=(T scale)
	{
		#if NAZARA_MATH_SAFE
		if (NumberEquals(scale, F(0.0)))
		{
			String error("Division by zero");

			NazaraError(error);
			throw std::domain_error(error);
		}
		#endif

		x /= scale;
		y /= scale;

		return *this;
	}

	template<typename T>
	bool Vector2<T>::operator==(const Vector2& vec) const
	{
		return NumberEquals(x, vec.x) &&
			   NumberEquals(y, vec.y);
	}

	template<typename T>
	bool Vector2<T>::operator!=(const Vector2& vec) const
	{
		return !operator==(vec);
	}

	template<typename T>
	bool Vector2<T>::operator<(const Vector2& vec) const
	{
		if (x == vec.x)
			return y < vec.y;
		else
			return x < vec.x;
	}

	template<typename T>
	bool Vector2<T>::operator<=(const Vector2& vec) const
	{
		if (x == vec.x)
			return y <= vec.y;
		else
			return x < vec.x;
	}

	template<typename T>
	bool Vector2<T>::operator>(const Vector2& vec) const
	{
		return !operator<=(vec);
	}

	template<typename T>
	bool Vector2<T>::operator>=(const Vector2& vec) const
	{
		return !operator<(vec);
	}

	template<typename T>
	Vector2<T> Vector2<T>::Lerp(const Vector2& from, const Vector2& to, T interpolation)
	{
		return Lerp(from, to, interpolation);
	}

	template<typename T>
	Vector2<T> Vector2<T>::Unit()
	{
		Vector2 vector;
		vector.MakeUnit();

		return vector;
	}

	template<typename T>
	Vector2<T> Vector2<T>::UnitX()
	{
		Vector2 vector;
		vector.MakeUnitX();

		return vector;
	}

	template<typename T>
	Vector2<T> Vector2<T>::UnitY()
	{
		Vector2 vector;
		vector.MakeUnitY();

		return vector;
	}

	template<typename T>
	Vector2<T> Vector2<T>::Zero()
	{
		Vector2 vector;
		vector.MakeZero();

		return vector;
	}
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nz::Vector2<T>& vec)
{
	return out << vec.ToString();
}

template<typename T>
Nz::Vector2<T> operator*(T scale, const Nz::Vector2<T>& vec)
{
	return Nz::Vector2<T>(scale * vec.x, scale * vec.y);
}

template<typename T>
Nz::Vector2<T> operator/(T scale, const Nz::Vector2<T>& vec)
{
	#if NAZARA_MATH_SAFE
	if (Nz::NumberEquals(vec.x, F(0.0)) || Nz::NumberEquals(vec.y, F(0.0)))
	{
		Nz::String error("Division by zero");

		NazaraError(error);
		throw std::domain_error(error);
	}
	#endif

	return Nz::Vector2<T>(scale/vec.x, scale/vec.y);
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
