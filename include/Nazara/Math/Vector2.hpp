// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_MATH_VECTOR2_HPP
#define NAZARA_MATH_VECTOR2_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Math/Angle.hpp>
#include <NazaraUtils/TypeTag.hpp>
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
			constexpr Vector2() = default;
			constexpr Vector2(T X, T Y);
			constexpr explicit Vector2(T scale);
			template<typename U> constexpr explicit Vector2(const Vector2<U>& vec);
			constexpr Vector2(const Vector2&) = default;
			constexpr Vector2(Vector2&&) = default;
			constexpr explicit Vector2(const Vector3<T>& vec);
			constexpr explicit Vector2(const Vector4<T>& vec);
			~Vector2() = default;

			T AbsDotProduct(const Vector2& vec) const;
			RadianAngle<T> AngleBetween(const Vector2& vec) const;
			template<typename F> constexpr Vector2& Apply(F&& func);
			constexpr bool ApproxEqual(const Vector2& vec, T maxDifference = std::numeric_limits<T>::epsilon()) const;

			template<typename U = T> U Distance(const Vector2& vec) const;
			constexpr T DotProduct(const Vector2& vec) const;

			template<typename U = T> T GetLength() const;
			Vector2 GetNormal(T* length = nullptr) const;
			constexpr T GetSquaredLength() const;

			constexpr Vector2& Maximize(const Vector2& vec);
			constexpr Vector2& Minimize(const Vector2& vec);

			Vector2& Normalize(T* length = nullptr);

			constexpr T SquaredDistance(const Vector2& vec) const;

			std::string ToString() const;

			constexpr T& operator[](std::size_t i);
			constexpr T operator[](std::size_t i) const;

			constexpr const Vector2& operator+() const;
			constexpr Vector2 operator-() const;

			constexpr Vector2 operator+(const Vector2& vec) const;
			constexpr Vector2 operator-(const Vector2& vec) const;
			constexpr Vector2 operator*(const Vector2& vec) const;
			constexpr Vector2 operator*(T scale) const;
			constexpr Vector2 operator/(const Vector2& vec) const;
			constexpr Vector2 operator/(T scale) const;
			constexpr Vector2 operator%(const Vector2& vec) const;
			constexpr Vector2 operator%(T mod) const;

			constexpr Vector2& operator=(const Vector2&) = default;
			constexpr Vector2& operator=(Vector2&&) = default;

			constexpr Vector2& operator+=(const Vector2& vec);
			constexpr Vector2& operator-=(const Vector2& vec);
			constexpr Vector2& operator*=(const Vector2& vec);
			constexpr Vector2& operator*=(T scale);
			constexpr Vector2& operator/=(const Vector2& vec);
			constexpr Vector2& operator/=(T scale);
			constexpr Vector2& operator%=(const Vector2& vec);
			constexpr Vector2& operator%=(T mod);

			constexpr bool operator==(const Vector2& vec) const;
			constexpr bool operator!=(const Vector2& vec) const;
			constexpr bool operator<(const Vector2& vec) const;
			constexpr bool operator<=(const Vector2& vec) const;
			constexpr bool operator>(const Vector2& vec) const;
			constexpr bool operator>=(const Vector2& vec) const;

			template<typename F> static constexpr auto Apply(const Vector2& vec, F&& func);
			static constexpr bool ApproxEqual(const Vector2& lhs, const Vector2& rhs, T maxDifference = std::numeric_limits<T>::epsilon());
			template<typename U = T> static U Distance(const Vector2& vec1, const Vector2& vec2);
			static constexpr T DotProduct(const Vector2& vec1, const Vector2& vec2);
			static constexpr Vector2 Lerp(const Vector2& from, const Vector2& to, T interpolation);
			static Vector2 Normalize(const Vector2& vec);
			static constexpr Vector2 Unit();
			static constexpr Vector2 UnitX();
			static constexpr Vector2 UnitY();
			static constexpr Vector2 Zero();

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
	template<typename T> bool Deserialize(SerializationContext& context, Vector2<T>* vector, TypeTag<Vector2<T>>);

	template<typename T> std::ostream& operator<<(std::ostream& out, const Vector2<T>& vec);

	template<typename T> constexpr Vector2<T> operator*(T scale, const Vector2<T>& vec);
	template<typename T> constexpr Vector2<T> operator/(T scale, const Vector2<T>& vec);
	template<typename T> constexpr Vector2<T> operator%(T mod, const Vector2<T>& vec);
}

namespace std
{
	template<class T> struct hash<Nz::Vector2<T>>;
}

#include <Nazara/Math/Vector2.inl>

#endif // NAZARA_MATH_VECTOR2_HPP
