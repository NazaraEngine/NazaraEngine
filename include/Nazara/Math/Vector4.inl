// Copyright (C) 2015 Rémi Bèges - Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <cstring>
#include <stdexcept>
#include <Nazara/Core/Debug.hpp>

///FIXME: Les calculs effectués ici sont probablements tous faux, la composante W étant spéciale dans le monde de la 3D

#define F(a) static_cast<T>(a)

namespace Nz
{
	template<typename T>
	Vector4<T>::Vector4(T X, T Y, T Z, T W)
	{
		Set(X, Y, Z, W);
	}

	template<typename T>
	Vector4<T>::Vector4(T X, T Y, const Vector2<T>& vec)
	{
		Set(X, Y, vec);
	}

	template<typename T>
	Vector4<T>::Vector4(T X, const Vector2<T>& vec, T W)
	{
		Set(X, vec, W);
	}

	template<typename T>
	Vector4<T>::Vector4(T X, const Vector3<T>& vec)
	{
		Set(X, vec);
	}

	template<typename T>
	Vector4<T>::Vector4(T scale)
	{
		Set(scale);
	}

	template<typename T>
	Vector4<T>::Vector4(const T vec[4])
	{
		Set(vec);
	}

	template<typename T>
	Vector4<T>::Vector4(const Vector2<T>& vec, T Z, T W)
	{
		Set(vec, Z, W);
	}

	template<typename T>
	Vector4<T>::Vector4(const Vector3<T>& vec, T W)
	{
		Set(vec, W);
	}

	template<typename T>
	template<typename U>
	Vector4<T>::Vector4(const Vector4<U>& vec)
	{
		Set(vec);
	}

	template<typename T>
	T Vector4<T>::AbsDotProduct(const Vector4& vec) const
	{
		return std::abs(x * vec.x) + std::abs(y * vec.y) + std::abs(z * vec.z) + std::abs(w * vec.w);
	}

	template<typename T>
	T Vector4<T>::DotProduct(const Vector4& vec) const
	{
		return x*vec.x + y*vec.y + z*vec.z + w*vec.w;
	}

	template<typename T>
	Vector4<T> Vector4<T>::GetNormal(T* length) const
	{
		Vector4<T> vec(*this);
		vec.Normalize(length);

		return vec;
	}

	template<typename T>
	Vector4<T>& Vector4<T>::MakeUnitX()
	{
		return Set(F(1.0), F(0.0), F(0.0), F(1.0));
	}

	template<typename T>
	Vector4<T>& Vector4<T>::MakeUnitY()
	{
		return Set(F(0.0), F(1.0), F(0.0), F(1.0));
	}

	template<typename T>
	Vector4<T>& Vector4<T>::MakeUnitZ()
	{
		return Set(F(0.0), F(0.0), F(1.0), F(1.0));
	}

	template<typename T>
	Vector4<T>& Vector4<T>::MakeZero()
	{
		return Set(F(0.0), F(0.0), F(0.0), F(0.0));
	}

	template<typename T>
	Vector4<T>& Vector4<T>::Maximize(const Vector4& vec)
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
	Vector4<T>& Vector4<T>::Minimize(const Vector4& vec)
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
	Vector4<T>& Vector4<T>::Normalize(T* length)
	{
		T invLength = F(1.0)/w;
		x *= invLength; // Attention, briser cette logique casserait Frustum::Extract
		y *= invLength;
		z *= invLength;

		if (length)
			*length = w;

		w = F(1.0);

		return *this;
	}

	template<typename T>
	Vector4<T>& Vector4<T>::Set(T X, T Y, T Z, T W)
	{
		x = X;
		y = Y;
		z = Z;
		w = W;

		return *this;
	}

	template<typename T>
	Vector4<T>& Vector4<T>::Set(T X, T Y, const Vector2<T>& vec)
	{
		x = X;
		y = Y;
		z = vec.x;
		w = vec.y;

		return *this;
	}

	template<typename T>
	Vector4<T>& Vector4<T>::Set(T X, const Vector2<T>& vec, T W)
	{
		x = X;
		y = vec.x;
		z = vec.y;
		w = W;

		return *this;
	}

	template<typename T>
	Vector4<T>& Vector4<T>::Set(T X, const Vector3<T>& vec)
	{
		x = X;
		y = vec.x;
		z = vec.y;
		w = vec.z;

		return *this;
	}

	template<typename T>
	Vector4<T>& Vector4<T>::Set(T scale)
	{
		x = scale;
		y = scale;
		z = scale;
		w = scale;

		return *this;
	}

	template<typename T>
	Vector4<T>& Vector4<T>::Set(const T vec[4])
	{
		std::memcpy(&x, vec, 4*sizeof(T));

		return *this;
	}

	template<typename T>
	Vector4<T>& Vector4<T>::Set(const Vector2<T>& vec, T Z, T W)
	{
		x = vec.x;
		y = vec.y;
		z = Z;
		w = W;

		return *this;
	}

	template<typename T>
	Vector4<T>& Vector4<T>::Set(const Vector3<T>& vec, T W)
	{
		x = vec.x;
		y = vec.y;
		z = vec.z;
		w = W;

		return *this;
	}

	template<typename T>
	Vector4<T>& Vector4<T>::Set(const Vector4& vec)
	{
		std::memcpy(this, &vec, sizeof(Vector4));

		return *this;
	}

	template<typename T>
	template<typename U>
	Vector4<T>& Vector4<T>::Set(const Vector4<U>& vec)
	{
		x = F(vec.x);
		y = F(vec.y);
		z = F(vec.z);
		w = F(vec.w);

		return *this;
	}

	template<typename T>
	String Vector4<T>::ToString() const
	{
		StringStream ss;

		return ss << "Vector4(" << x << ", " << y << ", " << z << ", " << w << ')';
	}

	template<typename T>
	Vector4<T>::operator T*()
	{
		return &x;
	}

	template<typename T>
	Vector4<T>::operator const T*() const
	{
		return &x;
	}

	template<typename T>
	const Vector4<T>& Vector4<T>::operator+() const
	{
		return *this;
	}

	template<typename T>
	Vector4<T> Vector4<T>::operator-() const
	{
		return Vector4(-x, -y, -z, -w);
	}

	template<typename T>
	Vector4<T> Vector4<T>::operator+(const Vector4& vec) const
	{
		return Vector4(x + vec.x, y + vec.y, z + vec.z, w + vec.w);
	}

	template<typename T>
	Vector4<T> Vector4<T>::operator-(const Vector4& vec) const
	{
		return Vector4(x - vec.x, y - vec.y, z - vec.z, w - vec.w);
	}

	template<typename T>
	Vector4<T> Vector4<T>::operator*(const Vector4& vec) const
	{
		return Vector4(x * vec.x, y * vec.y, z * vec.z, w * vec.w);
	}

	template<typename T>
	Vector4<T> Vector4<T>::operator*(T scale) const
	{
		return Vector4(x * scale, y * scale, z * scale, w * scale);
	}

	template<typename T>
	Vector4<T> Vector4<T>::operator/(const Vector4& vec) const
	{
		#if NAZARA_MATH_SAFE
		if (NumberEquals(vec.x, F(0.0)) || NumberEquals(vec.y, F(0.0)) || NumberEquals(vec.z, F(0.0)) || NumberEquals(vec.w, F(0.0)))
		{
			String error("Division by zero");

			NazaraError(error);
			throw std::domain_error(error);
		}
		#endif

		return Vector4(x / vec.x, y / vec.y, z / vec.z, w / vec.w);
	}

	template<typename T>
	Vector4<T> Vector4<T>::operator/(T scale) const
	{
		#if NAZARA_MATH_SAFE
		if (NumberEquals(scale, F(0.0)))
		{
			String error("Division by zero");

			NazaraError(error);
			throw std::domain_error(error);
		}
		#endif

		return Vector4(x / scale, y / scale, z / scale, w / scale);
	}

	template<typename T>
	Vector4<T>& Vector4<T>::operator+=(const Vector4& vec)
	{
		x += vec.x;
		y += vec.y;
		z += vec.z;
		w += vec.w;

		return *this;
	}

	template<typename T>
	Vector4<T>& Vector4<T>::operator-=(const Vector4& vec)
	{
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
		w -= vec.w;

		return *this;
	}

	template<typename T>
	Vector4<T>& Vector4<T>::operator*=(const Vector4& vec)
	{
		x *= vec.x;
		y *= vec.y;
		z *= vec.z;
		w *= vec.w;

		return *this;
	}

	template<typename T>
	Vector4<T>& Vector4<T>::operator*=(T scale)
	{
		x *= scale;
		y *= scale;
		z *= scale;
		w *= scale;

		return *this;
	}

	template<typename T>
	Vector4<T>& Vector4<T>::operator/=(const Vector4& vec)
	{
		#if NAZARA_MATH_SAFE
		if (NumberEquals(vec.x, F(0.0)) || NumberEquals(vec.y, F(0.0)) || NumberEquals(vec.z, F(0.0)) || NumberEquals(vec.w, F(0.0)))
		{
			String error("Division by zero");

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
	Vector4<T>& Vector4<T>::operator/=(T scale)
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
		z /= scale;
		w /= scale;

		return *this;
	}

	template<typename T>
	bool Vector4<T>::operator==(const Vector4& vec) const
	{
		return NumberEquals(x, vec.x) &&
			   NumberEquals(y, vec.y) &&
			   NumberEquals(z, vec.z) &&
			   NumberEquals(w, vec.w);
	}

	template<typename T>
	bool Vector4<T>::operator!=(const Vector4& vec) const
	{
		return !operator==(vec);
	}

	template<typename T>
	bool Vector4<T>::operator<(const Vector4& vec) const
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
	bool Vector4<T>::operator<=(const Vector4& vec) const
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
	bool Vector4<T>::operator>(const Vector4& vec) const
	{
		return !operator<=(vec);
	}

	template<typename T>
	bool Vector4<T>::operator>=(const Vector4& vec) const
	{
		return !operator<(vec);
	}

	template<typename T>
	Vector4<T> Vector4<T>::UnitX()
	{
		Vector4 vector;
		vector.MakeUnitX();

		return vector;
	}

	template<typename T>
	Vector4<T> Vector4<T>::UnitY()
	{
		Vector4 vector;
		vector.MakeUnitY();

		return vector;
	}

	template<typename T>
	Vector4<T> Vector4<T>::UnitZ()
	{
		Vector4 vector;
		vector.MakeUnitZ();

		return vector;
	}

	template<typename T>
	Vector4<T> Vector4<T>::Zero()
	{
		Vector4 vector;
		vector.MakeZero();

		return vector;
	}
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nz::Vector4<T>& vec)
{
	return out << vec.ToString();
}

template<typename T>
Nz::Vector4<T> operator*(T scale, const Nz::Vector4<T>& vec)
{
	return Nz::Vector4<T>(scale * vec.x, scale * vec.y, scale * vec.z, scale * vec.w);
}

template<typename T>
Nz::Vector4<T> operator/(T scale, const Nz::Vector4<T>& vec)
{
	#if NAZARA_MATH_SAFE
	if (NumberEquals(vec.x, F(0.0)) || NumberEquals(vec.y, F(0.0)) || NumberEquals(vec.z, F(0.0)) || NumberEquals(vec.w, F(0.0)))
	{
		Nz::String error("Division by zero");

		NazaraError(error);
		throw std::domain_error(error);
	}
	#endif

	return Nz::Vector4<T>(scale / vec.x, scale / vec.y, scale / vec.z, scale / vec.w);
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
