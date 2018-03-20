// Copyright (C) 2017 Rémi Bèges - Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VECTOR4_HPP
#define NAZARA_VECTOR4_HPP

#include <Nazara/Core/String.hpp>
#include <functional>

namespace Nz
{
	struct SerializationContext;

	template<typename T> class Vector2;
	template<typename T> class Vector3;

	template<typename T>
	class Vector4
	{
		public:
			Vector4() = default;
			Vector4(T X, T Y, T Z, T W = 1.0);
			Vector4(T X, T Y, const Vector2<T>& vec);
			Vector4(T X, const Vector2<T>& vec, T W);
			Vector4(T X, const Vector3<T>& vec);
			explicit Vector4(T scale);
			Vector4(const Vector2<T>& vec, T Z = 0.0, T W = 1.0);
			Vector4(const Vector3<T>& vec, T W = 1.0);
			template<typename U> explicit Vector4(const Vector4<U>& vec);
			Vector4(const Vector4& vec) = default;
			~Vector4() = default;

			T AbsDotProduct(const Vector4& vec) const;

			T DotProduct(const Vector4& vec) const;

			Vector4 GetNormal(T* length = nullptr) const;

			Vector4& MakeUnitX();
			Vector4& MakeUnitY();
			Vector4& MakeUnitZ();
			Vector4& MakeZero();

			Vector4& Maximize(const Vector4& vec);
			Vector4& Minimize(const Vector4& vec);

			Vector4& Normalize(T* length = nullptr);

			Vector4& Set(T X, T Y, T Z, T W = 1.0);
			Vector4& Set(T X, T Y, const Vector2<T>& vec);
			Vector4& Set(T X, const Vector2<T>& vec, T W);
			Vector4& Set(T X, const Vector3<T>& vec);
			Vector4& Set(T scale);
			Vector4& Set(const T vec[4]);
			Vector4& Set(const Vector2<T>& vec, T Z = 0.0, T W = 1.0);
			Vector4& Set(const Vector3<T>& vec, T W = 1.0);
			Vector4& Set(const Vector4<T>& vec);
			template<typename U> Vector4& Set(const Vector4<U>& vec);

			String ToString() const;

			operator T* ();
			operator const T* () const;

			const Vector4& operator+() const;
			Vector4 operator-() const;

			Vector4 operator+(const Vector4& vec) const;
			Vector4 operator-(const Vector4& vec) const;
			Vector4 operator*(const Vector4& vec) const;
			Vector4 operator*(T scale) const;
			Vector4 operator/(const Vector4& vec) const;
			Vector4 operator/(T scale) const;
			Vector4& operator=(const Vector4& other) = default;

			Vector4& operator+=(const Vector4& vec);
			Vector4& operator-=(const Vector4& vec);
			Vector4& operator*=(const Vector4& vec);
			Vector4& operator*=(T scale);
			Vector4& operator/=(const Vector4& vec);
			Vector4& operator/=(T scale);

			bool operator==(const Vector4& vec) const;
			bool operator!=(const Vector4& vec) const;
			bool operator<(const Vector4& vec) const;
			bool operator<=(const Vector4& vec) const;
			bool operator>(const Vector4& vec) const;
			bool operator>=(const Vector4& vec) const;

			static T DotProduct(const Vector4& vec1, const Vector4& vec2);
			static Vector4 Lerp(const Vector4& from, const Vector4& to, T interpolation);
			static Vector4 Normalize(const Vector4& vec);
			static Vector4 UnitX();
			static Vector4 UnitY();
			static Vector4 UnitZ();
			static Vector4 Zero();

			T x, y, z, w;
	};

	typedef Vector4<double> Vector4d;
	typedef Vector4<float> Vector4f;
	typedef Vector4<int> Vector4i;
	typedef Vector4<unsigned int> Vector4ui;
	typedef Vector4<Int32> Vector4i32;
	typedef Vector4<UInt32> Vector4ui32;

	template<typename T> bool Serialize(SerializationContext& context, const Vector4<T>& vector, TypeTag<Vector4<T>>);
	template<typename T> bool Unserialize(SerializationContext& context, Vector4<T>* vector, TypeTag<Vector4<T>>);
}

template<typename T> std::ostream& operator<<(std::ostream& out, const Nz::Vector4<T>& vec);

template<typename T> Nz::Vector4<T> operator*(T scale, const Nz::Vector4<T>& vec);
template<typename T> Nz::Vector4<T> operator/(T scale, const Nz::Vector4<T>& vec);

namespace std
{
	template<class T> struct hash<Nz::Vector4<T>>;
}

#include <Nazara/Math/Vector4.inl>

#endif // NAZARA_VECTOR4_HPP
