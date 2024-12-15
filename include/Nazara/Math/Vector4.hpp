// Copyright (C) 2024 Rémi Bèges - Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_MATH_VECTOR4_HPP
#define NAZARA_MATH_VECTOR4_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <NazaraUtils/TypeTag.hpp>
#include <functional>
#include <limits>
#include <string>

namespace Nz
{
	struct SerializationContext;

	template<typename T> class Vector2;
	template<typename T> class Vector3;

	template<typename T>
	class Vector4
	{
		public:
			constexpr Vector4() = default;
			constexpr Vector4(T X, T Y, T Z, T W = 1.0);
			constexpr Vector4(T X, T Y, const Vector2<T>& vec);
			constexpr Vector4(T X, const Vector2<T>& vec, T W);
			constexpr Vector4(T X, const Vector3<T>& vec);
			constexpr explicit Vector4(T scale);
			constexpr Vector4(const Vector2<T>& vec, T Z = 0.0, T W = 1.0);
			constexpr Vector4(const Vector3<T>& vec, T W = 1.0);
			template<typename U> constexpr explicit Vector4(const Vector4<U>& vec);

			T AbsDotProduct(const Vector4& vec) const;
			template<typename F> constexpr Vector4& Apply(F&& func);
			constexpr bool ApproxEqual(const Vector4& vec, T maxDifference = std::numeric_limits<T>::epsilon()) const;

			constexpr T DotProduct(const Vector4& vec) const;

			Vector4 GetNormal(T* length = nullptr) const;

			constexpr Vector4& Maximize(const Vector4& vec);
			constexpr Vector4& Minimize(const Vector4& vec);

			Vector4& Normalize(T* length = nullptr);

			std::string ToString() const;

			constexpr T& operator[](std::size_t i);
			constexpr const T& operator[](std::size_t i) const;

			constexpr const Vector4& operator+() const;
			constexpr Vector4 operator-() const;

			constexpr Vector4 operator+(const Vector4& vec) const;
			constexpr Vector4 operator-(const Vector4& vec) const;
			constexpr Vector4 operator*(const Vector4& vec) const;
			constexpr Vector4 operator*(T scale) const;
			constexpr Vector4 operator/(const Vector4& vec) const;
			constexpr Vector4 operator/(T scale) const;
			constexpr Vector4 operator%(const Vector4& vec) const;
			constexpr Vector4 operator%(T mod) const;

			constexpr Vector4& operator+=(const Vector4& vec);
			constexpr Vector4& operator-=(const Vector4& vec);
			constexpr Vector4& operator*=(const Vector4& vec);
			constexpr Vector4& operator*=(T scale);
			constexpr Vector4& operator/=(const Vector4& vec);
			constexpr Vector4& operator/=(T scale);
			constexpr Vector4& operator%=(const Vector4& vec);
			constexpr Vector4& operator%=(T mod);

			constexpr bool operator==(const Vector4& vec) const;
			constexpr bool operator!=(const Vector4& vec) const;
			constexpr bool operator<(const Vector4& vec) const;
			constexpr bool operator<=(const Vector4& vec) const;
			constexpr bool operator>(const Vector4& vec) const;
			constexpr bool operator>=(const Vector4& vec) const;

			template<typename F> static constexpr auto Apply(const Vector4& vec, F&& func);
			static constexpr bool ApproxEqual(const Vector4& lhs, const Vector4& rhs, T maxDifference = std::numeric_limits<T>::epsilon());
			static constexpr T DotProduct(const Vector4& vec1, const Vector4& vec2);
			static constexpr Vector4 Lerp(const Vector4& from, const Vector4& to, T interpolation);
			static Vector4 Normalize(const Vector4& vec);
			static constexpr Vector4 UnitX();
			static constexpr Vector4 UnitY();
			static constexpr Vector4 UnitZ();
			static constexpr Vector4 Zero();

			T x, y, z, w;
	};

	using Vector4d = Vector4<double>;
	using Vector4f = Vector4<float>;
	using Vector4i = Vector4<int>;
	using Vector4ui = Vector4<unsigned int>;
	using Vector4i32 = Vector4<Int32>;
	using Vector4i64 = Vector4<Int64>;
	using Vector4ui32 = Vector4<UInt32>;
	using Vector4ui64 = Vector4<UInt64>;

	template<typename T> bool Serialize(SerializationContext& context, const Vector4<T>& vector, TypeTag<Vector4<T>>);
	template<typename T> bool Deserialize(SerializationContext& context, Vector4<T>* vector, TypeTag<Vector4<T>>);

	template<typename T> std::ostream& operator<<(std::ostream& out, const Vector4<T>& vec);

	template<typename T> constexpr Vector4<T> operator*(T scale, const Vector4<T>& vec);
	template<typename T> constexpr Vector4<T> operator/(T scale, const Vector4<T>& vec);
	template<typename T> constexpr Vector4<T> operator%(T mod, const Vector4<T>& vec);
}

namespace std
{
	template<class T> struct hash<Nz::Vector4<T>>;
}

#include <Nazara/Math/Vector4.inl>

#endif // NAZARA_MATH_VECTOR4_HPP
