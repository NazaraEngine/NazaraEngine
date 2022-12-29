// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATH_VECTOR2_HPP
#define NAZARA_MATH_VECTOR2_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Math/Angle.hpp>
#include <Nazara/Utils/TypeTag.hpp>
#include <functional>
#include <string>

namespace Nz
{
	struct SerializationContext;

	template<typename T> class Vector3;
	template<typename T> class Vector4;

	template<typename T>
	class Vector2
	{
		public:
			Vector2() = default;
			Vector2(T X, T Y);
			explicit Vector2(T scale);
			template<typename U> explicit Vector2(const Vector2<U>& vec);
			Vector2(const Vector2& vec) = default;
			explicit Vector2(const Vector3<T>& vec);
			explicit Vector2(const Vector4<T>& vec);
			~Vector2() = default;

			T AbsDotProduct(const Vector2& vec) const;
			RadianAngle<T> AngleBetween(const Vector2& vec) const;

			template<typename U = T>
			U Distance(const Vector2& vec) const;
			T DotProduct(const Vector2& vec) const;

			T GetLength() const;
			float GetLengthf() const;
			Vector2 GetNormal(T* length = nullptr) const;
			T GetSquaredLength() const;

			Vector2& MakeUnit();
			Vector2& MakeUnitX();
			Vector2& MakeUnitY();
			Vector2& MakeZero();

			Vector2& Maximize(const Vector2& vec);
			Vector2& Minimize(const Vector2& vec);

			Vector2& Normalize(T* length = nullptr);

			Vector2& Set(T X, T Y);
			Vector2& Set(T scale);
			Vector2& Set(const T* vec);
			Vector2& Set(const Vector3<T>& vec);
			Vector2& Set(const Vector4<T>& vec);
			template<typename U> Vector2& Set(const Vector2<U>& vec);

			T SquaredDistance(const Vector2& vec) const;

			std::string ToString() const;

			T& operator[](std::size_t i);
			T operator[](std::size_t i) const;

			const Vector2& operator+() const;
			Vector2 operator-() const;

			Vector2 operator+(const Vector2& vec) const;
			Vector2 operator-(const Vector2& vec) const;
			Vector2 operator*(const Vector2& vec) const;
			Vector2 operator*(T scale) const;
			Vector2 operator/(const Vector2& vec) const;
			Vector2 operator/(T scale) const;
			Vector2& operator=(const Vector2& other) = default;

			Vector2& operator+=(const Vector2& vec);
			Vector2& operator-=(const Vector2& vec);
			Vector2& operator*=(const Vector2& vec);
			Vector2& operator*=(T scale);
			Vector2& operator/=(const Vector2& vec);
			Vector2& operator/=(T scale);

			bool operator==(const Vector2& vec) const;
			bool operator!=(const Vector2& vec) const;
			bool operator<(const Vector2& vec) const;
			bool operator<=(const Vector2& vec) const;
			bool operator>(const Vector2& vec) const;
			bool operator>=(const Vector2& vec) const;

			template<typename U = T> static U Distance(const Vector2& vec1, const Vector2& vec2);
			static T DotProduct(const Vector2& vec1, const Vector2& vec2);
			static Vector2 Lerp(const Vector2& from, const Vector2& to, T interpolation);
			static Vector2 Normalize(const Vector2& vec);
			static Vector2 Unit();
			static Vector2 UnitX();
			static Vector2 UnitY();
			static Vector2 Zero();

			T x, y;
	};

	using Vector2d = Vector2<double>;
	using Vector2f = Vector2<float>;
	using Vector2i = Vector2<int>;
	using Vector2ui = Vector2<unsigned int>;
	using Vector2i32 = Vector2<Int32>;
	using Vector2i64 = Vector2<Int64>;
	using Vector2ui32 = Vector2<UInt32>;
	using Vector2ui64 = Vector2<UInt64>;

	template<typename T> bool Serialize(SerializationContext& context, const Vector2<T>& vector, TypeTag<Vector2<T>>);
	template<typename T> bool Unserialize(SerializationContext& context, Vector2<T>* vector, TypeTag<Vector2<T>>);

	template<typename T> std::ostream& operator<<(std::ostream& out, const Vector2<T>& vec);

	template<typename T> Vector2<T> operator*(T scale, const Nz::Vector2<T>& vec);
	template<typename T> Vector2<T> operator/(T scale, const Nz::Vector2<T>& vec);
}

namespace std
{
	template<class T> struct hash<Nz::Vector2<T>>;
}

#include <Nazara/Math/Vector2.inl>

#endif // NAZARA_MATH_VECTOR2_HPP
