// Copyright (C) 2015 Rémi Bèges - Jérôme Leclercq
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
	Vector3<T>::Vector3(T X, T Y, T Z)
	{
		Set(X, Y, Z);
	}

	template<typename T>
	Vector3<T>::Vector3(T X, const Vector2<T>& vec)
	{
		Set(X, vec);
	}

	template<typename T>
	Vector3<T>::Vector3(T scale)
	{
		Set(scale);
	}

	template<typename T>
	Vector3<T>::Vector3(const T vec[3])
	{
		Set(vec);
	}

	template<typename T>
	Vector3<T>::Vector3(const Vector2<T>& vec, T Z)
	{
		Set(vec, Z);
	}

	template<typename T>
	template<typename U>
	Vector3<T>::Vector3(const Vector3<U>& vec)
	{
		Set(vec);
	}

	template<typename T>
	Vector3<T>::Vector3(const Vector4<T>& vec)
	{
		Set(vec);
	}

	template<typename T>
	T Vector3<T>::AbsDotProduct(const Vector3& vec) const
	{
		return std::abs(x * vec.x) + std::abs(y * vec.y) + std::abs(z * vec.z);
	}

	template<typename T>
	T Vector3<T>::AngleBetween(const Vector3& vec) const
	{
		// sqrt(a) * sqrt(b) = sqrt(a*b)
		T divisor = std::sqrt(GetSquaredLength() * vec.GetSquaredLength());

		#if NAZARA_MATH_SAFE
		if (NumberEquals(divisor, F(0.0)))
		{
			String error("Division by zero");

			NazaraError(error);
			throw std::domain_error(error);
		}
		#endif

		T alpha = DotProduct(vec)/divisor;
		return FromRadians(std::acos(Clamp(alpha, F(-1.0), F(1.0))));
	}

	template<typename T>
	Vector3<T> Vector3<T>::CrossProduct(const Vector3& vec) const
	{
		return Vector3(y * vec.z - z * vec.y, z * vec.x - x * vec.z, x * vec.y - y * vec.x);
	}

	template<typename T>
	T Vector3<T>::Distance(const Vector3& vec) const
	{
		return std::sqrt(SquaredDistance(vec));
	}

	template<typename T>
	float Vector3<T>::Distancef(const Vector3& vec) const
	{
		return std::sqrt(static_cast<float>(SquaredDistance(vec)));
	}

	template<typename T>
	T Vector3<T>::DotProduct(const Vector3& vec) const
	{
		return x*vec.x + y*vec.y + z*vec.z;
	}

	template<typename T>
	T Vector3<T>::GetLength() const
	{
		return static_cast<T>(std::sqrt(GetSquaredLength()));
	}

	template<typename T>
	float Vector3<T>::GetLengthf() const
	{
		return std::sqrt(static_cast<float>(GetSquaredLength()));
	}

	template<typename T>
	Vector3<T> Vector3<T>::GetNormal(T* length) const
	{
		Vector3 vec(*this);
		vec.Normalize(length);

		return vec;
	}

	template<typename T>
	T Vector3<T>::GetSquaredLength() const
	{
		return x*x + y*y + z*z;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::MakeBackward()
	{
		return Set(F(0.0), F(0.0), F(1.0));
	}

	template<typename T>
	Vector3<T>& Vector3<T>::MakeDown()
	{
		return Set(F(0.0), F(-1.0), F(0.0));
	}

	template<typename T>
	Vector3<T>& Vector3<T>::MakeForward()
	{
		return Set(F(0.0), F(0.0), F(-1.0));
	}

	template<typename T>
	Vector3<T>& Vector3<T>::MakeLeft()
	{
		return Set(F(-1.0), F(0.0), F(0.0));
	}

	template<typename T>
	Vector3<T>& Vector3<T>::MakeRight()
	{
		return Set(F(1.0), F(0.0), F(0.0));
	}

	template<typename T>
	Vector3<T>& Vector3<T>::MakeUnit()
	{
		return Set(F(1.0), F(1.0), F(1.0));
	}

	template<typename T>
	Vector3<T>& Vector3<T>::MakeUnitX()
	{
		return Set(F(1.0), F(0.0), F(0.0));
	}

	template<typename T>
	Vector3<T>& Vector3<T>::MakeUnitY()
	{
		return Set(F(0.0), F(1.0), F(0.0));
	}

	template<typename T>
	Vector3<T>& Vector3<T>::MakeUnitZ()
	{
		return Set(F(0.0), F(0.0), F(1.0));
	}

	template<typename T>
	Vector3<T>& Vector3<T>::MakeUp()
	{
		return Set(F(0.0), F(1.0), F(0.0));
	}

	template<typename T>
	Vector3<T>& Vector3<T>::MakeZero()
	{
		return Set(F(0.0), F(0.0), F(0.0));
	}

	template<typename T>
	Vector3<T>& Vector3<T>::Maximize(const Vector3& vec)
	{
		if (vec.x > x)
			x = vec.x;

		if (vec.y > y)
			y = vec.y;

		if (vec.z > z)
			z = vec.z;

		return *this;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::Minimize(const Vector3& vec)
	{
		if (vec.x < x)
			x = vec.x;

		if (vec.y < y)
			y = vec.y;

		if (vec.z < z)
			z = vec.z;

		return *this;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::Normalize(T* length)
	{
		T norm = GetLength();
		if (norm > F(0.0))
		{
			T invNorm = F(1.0) / norm;
			x *= invNorm;
			y *= invNorm;
			z *= invNorm;
		}

		if (length)
			*length = norm;

		return *this;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::Set(T X, T Y, T Z)
	{
		x = X;
		y = Y;
		z = Z;

		return *this;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::Set(T X, const Vector2<T>& vec)
	{
		x = X;
		y = vec.x;
		z = vec.y;

		return *this;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::Set(T scale)
	{
		x = scale;
		y = scale;
		z = scale;

		return *this;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::Set(const T vec[3])
	{
		std::memcpy(&x, vec, 3*sizeof(T));

		return *this;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::Set(const Vector2<T>& vec, T Z)
	{
		x = vec.x;
		y = vec.y;
		z = Z;

		return *this;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::Set(const Vector3& vec)
	{
		std::memcpy(this, &vec, sizeof(Vector3));

		return *this;
	}

	template<typename T>
	template<typename U>
	Vector3<T>& Vector3<T>::Set(const Vector3<U>& vec)
	{
		x = F(vec.x);
		y = F(vec.y);
		z = F(vec.z);

		return *this;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::Set(const Vector4<T>& vec)
	{
		x = vec.x;
		y = vec.y;
		z = vec.z;

		return *this;
	}

	template<typename T>
	T Vector3<T>::SquaredDistance(const Vector3& vec) const
	{
		return (*this - vec).GetSquaredLength();
	}

	template<typename T>
	String Vector3<T>::ToString() const
	{
		StringStream ss;

		return ss << "Vector3(" << x << ", " << y << ", " << z <<')';
	}

	template<typename T>
	Vector3<T>::operator T*()
	{
		return &x;
	}

	template<typename T>
	Vector3<T>::operator const T*() const
	{
		return &x;
	}

	template<typename T>
	const Vector3<T>& Vector3<T>::operator+() const
	{
		return *this;
	}

	template<typename T>
	Vector3<T> Vector3<T>::operator-() const
	{
		return Vector3(-x, -y, -z);
	}

	template<typename T>
	Vector3<T> Vector3<T>::operator+(const Vector3& vec) const
	{
		return Vector3(x + vec.x, y + vec.y, z + vec.z);
	}

	template<typename T>
	Vector3<T> Vector3<T>::operator-(const Vector3& vec) const
	{
		return Vector3(x - vec.x, y - vec.y, z - vec.z);
	}

	template<typename T>
	Vector3<T> Vector3<T>::operator*(const Vector3& vec) const
	{
		return Vector3(x * vec.x, y * vec.y, z * vec.z);
	}

	template<typename T>
	Vector3<T> Vector3<T>::operator*(T scale) const
	{
		return Vector3(x * scale, y * scale, z * scale);
	}

	template<typename T>
	Vector3<T> Vector3<T>::operator/(const Vector3& vec) const
	{
		#if NAZARA_MATH_SAFE
		if (NumberEquals(vec.x, F(0.0)) || NumberEquals(vec.y, F(0.0)) || NumberEquals(vec.z, F(0.0)))
		{
			String error("Division by zero");

			NazaraError(error);
			throw std::domain_error(error);
		}
		#endif

		return Vector3(x / vec.x, y / vec.y, z / vec.z);
	}

	template<typename T>
	Vector3<T> Vector3<T>::operator/(T scale) const
	{
		#if NAZARA_MATH_SAFE
		if (NumberEquals(scale, F(0.0)))
		{
			String error("Division by zero");

			NazaraError(error);
			throw std::domain_error(error);
		}
		#endif

		return Vector3(x / scale, y / scale, z / scale);
	}

	template<typename T>
	Vector3<T>& Vector3<T>::operator+=(const Vector3& vec)
	{
		x += vec.x;
		y += vec.y;
		z += vec.z;

		return *this;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::operator-=(const Vector3& vec)
	{
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;

		return *this;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::operator*=(const Vector3& vec)
	{
		x *= vec.x;
		y *= vec.y;
		z *= vec.z;

		return *this;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::operator*=(T scale)
	{
		x *= scale;
		y *= scale;
		z *= scale;

		return *this;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::operator/=(const Vector3& vec)
	{
		if (NumberEquals(vec.x, F(0.0)) || NumberEquals(vec.y, F(0.0)) || NumberEquals(vec.z, F(0.0)))
		{
			String error("Division by zero");

			NazaraError(error);
			throw std::domain_error(error);
		}

		x /= vec.x;
		y /= vec.y;
		z /= vec.z;

		return *this;
	}

	template<typename T>
	Vector3<T>& Vector3<T>::operator/=(T scale)
	{
		if (NumberEquals(scale, F(0.0)))
		{
			String error("Division by zero");

			NazaraError(error);
			throw std::domain_error(error);
		}

		x /= scale;
		y /= scale;
		z /= scale;

		return *this;
	}

	template<typename T>
	bool Vector3<T>::operator==(const Vector3& vec) const
	{
		return NumberEquals(x, vec.x) &&
			   NumberEquals(y, vec.y) &&
			   NumberEquals(z, vec.z);
	}

	template<typename T>
	bool Vector3<T>::operator!=(const Vector3& vec) const
	{
		return !operator==(vec);
	}

	template<typename T>
	bool Vector3<T>::operator<(const Vector3& vec) const
	{
		if (x == vec.x)
		{
			if (y == vec.y)
				return z < vec.z;
			else
				return y < vec.y;
		}
		else
			return x < vec.x;
	}

	template<typename T>
	bool Vector3<T>::operator<=(const Vector3& vec) const
	{
		if (x == vec.x)
		{
			if (y == vec.y)
				return z <= vec.z;
			else
				return y < vec.y;
		}
		else
			return x < vec.x;
	}

	template<typename T>
	bool Vector3<T>::operator>(const Vector3& vec) const
	{
		return !operator<=(vec);
	}

	template<typename T>
	bool Vector3<T>::operator>=(const Vector3& vec) const
	{
		return !operator<(vec);
	}

	template<typename T>
	Vector3<T> Vector3<T>::CrossProduct(const Vector3& vec1, const Vector3& vec2)
	{
		return vec1.CrossProduct(vec2);
	}

	template<typename T>
	T Vector3<T>::DotProduct(const Vector3& vec1, const Vector3& vec2)
	{
		return vec1.DotProduct(vec2);
	}

	template<typename T>
	Vector3<T> Vector3<T>::Backward()
	{
		Vector3 vector;
		vector.MakeBackward();

		return vector;
	}

	template<typename T>
	Vector3<T> Vector3<T>::Down()
	{
		Vector3 vector;
		vector.MakeDown();

		return vector;
	}

	template<typename T>
	Vector3<T> Vector3<T>::Forward()
	{
		Vector3 vector;
		vector.MakeForward();

		return vector;
	}

	template<typename T>
	Vector3<T> Vector3<T>::Left()
	{
		Vector3 vector;
		vector.MakeLeft();

		return vector;
	}

	template<typename T>
	Vector3<T> Vector3<T>::Lerp(const Vector3& from, const Vector3& to, T interpolation)
	{
		return Nz::Lerp(from, to, interpolation);
	}

	template<typename T>
	Vector3<T> Vector3<T>::Normalize(const Vector3& vec)
	{
		return vec.GetNormal();
	}

	template<typename T>
	Vector3<T> Vector3<T>::Right()
	{
		Vector3 vector;
		vector.MakeRight();

		return vector;
	}

	template<typename T>
	Vector3<T> Vector3<T>::Unit()
	{
		Vector3 vector;
		vector.MakeUnit();

		return vector;
	}

	template<typename T>
	Vector3<T> Vector3<T>::UnitX()
	{
		Vector3 vector;
		vector.MakeUnitX();

		return vector;
	}

	template<typename T>
	Vector3<T> Vector3<T>::UnitY()
	{
		Vector3 vector;
		vector.MakeUnitY();

		return vector;
	}

	template<typename T>
	Vector3<T> Vector3<T>::UnitZ()
	{
		Vector3 vector;
		vector.MakeUnitZ();

		return vector;
	}

	template<typename T>
	Vector3<T> Vector3<T>::Up()
	{
		Vector3 vector;
		vector.MakeUp();

		return vector;
	}

	template<typename T>
	Vector3<T> Vector3<T>::Zero()
	{
		Vector3 vector;
		vector.MakeZero();

		return vector;
	}
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nz::Vector3<T>& vec)
{
	return out << vec.ToString();
}

template<typename T>
Nz::Vector3<T> operator*(T scale, const Nz::Vector3<T>& vec)
{
	return Nz::Vector3<T>(scale * vec.x, scale * vec.y, scale * vec.z);
}

template<typename T>
Nz::Vector3<T> operator/(T scale, const Nz::Vector3<T>& vec)
{
	#if NAZARA_MATH_SAFE
	if (Nz::NumberEquals(vec.x, F(0.0)) || Nz::NumberEquals(vec.y, F(0.0)) || Nz::NumberEquals(vec.z, F(0.0)))
	{
		Nz::String error("Division by zero");

		NazaraError(error);
		throw std::domain_error(error);
	}
	#endif

	return Nz::Vector3<T>(scale / vec.x, scale / vec.y, scale / vec.z);
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
