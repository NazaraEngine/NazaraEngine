// Copyright (C) 2026 Rémi Bèges - Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_MATH_VECTOR3_HPP
#define NAZARA_MATH_VECTOR3_HPP

#include <NazaraUtils/Prerequisites.hpp>
#include <Nazara/Math/Angle.hpp>
#include <NazaraUtils/TypeTag.hpp>
#include <functional>
#include <string>

namespace Nz
{
	struct SerializationContext;

	template<typename T> class Vector2;
	template<typename T> class Vector4;

	template<typename T>
	class Vector3
	{
		public:
			constexpr Vector3() = default;
			constexpr Vector3(T X, T Y, T Z);
			constexpr Vector3(T X, const Vector2<T>& vec);
			constexpr explicit Vector3(T scale);
			constexpr Vector3(const Vector2<T>& vec, T Z = 0.0);
			template<typename U> constexpr explicit Vector3(const Vector3<U>& vec);
			constexpr explicit Vector3(const Vector4<T>& vec);

			T AbsDotProduct(const Vector3& vec) const;
			RadianAngle<T> AngleBetween(const Vector3& vec) const;
			template<typename F> constexpr Vector3& Apply(F&& func);
			constexpr bool ApproxEqual(const Vector3& vec, T maxDifference = std::numeric_limits<T>::epsilon()) const;

			constexpr Vector3 CrossProduct(const Vector3& vec) const;

			template<typename U = T> U Distance(const Vector3& vec) const;
			constexpr T DotProduct(const Vector3& vec) const;

			Vector3 GetAbs() const;
			template<typename U = T> U GetLength() const;
			Vector3 GetNormal(T* length = nullptr) const;
			constexpr T GetSquaredLength() const;

			constexpr Vector3& Maximize(const Vector3& vec);
			constexpr Vector3& Minimize(const Vector3& vec);

			constexpr Vector3 Project(const Vector3& normal) const;
			constexpr Vector3 ProjectOnPlane(const Vector3& normal) const;

			Vector3& Normalize(T* length = nullptr);

			constexpr T SquaredDistance(const Vector3& vec) const;

			std::string ToString() const;

			constexpr T& operator[](std::size_t i);
			constexpr const T& operator[](std::size_t i) const;

			constexpr const Vector3& operator+() const;
			constexpr Vector3 operator-() const;

			constexpr Vector3 operator+(const Vector3& vec) const;
			constexpr Vector3 operator-(const Vector3& vec) const;
			constexpr Vector3 operator*(const Vector3& vec) const;
			constexpr Vector3 operator*(T scale) const;
			constexpr Vector3 operator/(const Vector3& vec) const;
			constexpr Vector3 operator/(T scale) const;
			constexpr Vector3 operator%(const Vector3& vec) const;
			constexpr Vector3 operator%(T mod) const;

			constexpr Vector3& operator+=(const Vector3& vec);
			constexpr Vector3& operator-=(const Vector3& vec);
			constexpr Vector3& operator*=(const Vector3& vec);
			constexpr Vector3& operator*=(T scale);
			constexpr Vector3& operator/=(const Vector3& vec);
			constexpr Vector3& operator/=(T scale);
			constexpr Vector3& operator%=(const Vector3& vec);
			constexpr Vector3& operator%=(T mod);

			constexpr bool operator==(const Vector3& vec) const;
			constexpr bool operator!=(const Vector3& vec) const;
			constexpr bool operator<(const Vector3& vec) const;
			constexpr bool operator<=(const Vector3& vec) const;
			constexpr bool operator>(const Vector3& vec) const;
			constexpr bool operator>=(const Vector3& vec) const;

			template<typename F> static constexpr auto Apply(const Vector3& vec, F&& func);
			static constexpr bool ApproxEqual(const Vector3& lhs, const Vector3& rhs, T maxDifference = std::numeric_limits<T>::epsilon());
			static constexpr Vector3 Backward();
			static constexpr Vector3 Clamp(const Vector3& vec, const Vector3& min, const Vector3& max);
			static constexpr Vector3 CrossProduct(const Vector3& vec1, const Vector3& vec2);
			template<typename U = T> static U Distance(const Vector3& vec1, const Vector3& vec2);
			static constexpr T DotProduct(const Vector3& vec1, const Vector3& vec2);
			static constexpr Vector3 Down();
			static constexpr Vector3 Forward();
			static constexpr Vector3 Left();
			static constexpr Vector3 Lerp(const Vector3& from, const Vector3& to, T interpolation);
			static constexpr Vector3 Max(const Vector3& lhs, const Vector3& rhs);
			static constexpr Vector3 Min(const Vector3& lhs, const Vector3& rhs);
			static Vector3 RotateTowards(const Vector3& from, const Vector3& to, RadianAngle<T> maxAngle);
			static Vector3 Normalize(const Vector3& vec);
			static constexpr Vector3 Right();
			static constexpr T SquaredDistance(const Vector3& vec1, const Vector3& vec2);
			static constexpr Vector3 Unit();
			static constexpr Vector3 UnitX();
			static constexpr Vector3 UnitY();
			static constexpr Vector3 UnitZ();
			static constexpr Vector3 Up();
			static constexpr Vector3 Zero();

			T x, y, z;
	};

	using Vector3d = Vector3<double>;
	using Vector3f = Vector3<float>;
	using Vector3i = Vector3<int>;
	using Vector3ui = Vector3<unsigned int>;
	using Vector3i32 = Vector3<Int32>;
	using Vector3i64 = Vector3<Int64>;
	using Vector3ui32 = Vector3<UInt32>;
	using Vector3ui64 = Vector3<UInt64>;

	template<typename T> bool Serialize(SerializationContext& context, const Vector3<T>& vector, TypeTag<Vector3<T>>);
	template<typename T> bool Deserialize(SerializationContext& context, Vector3<T>* vector, TypeTag<Vector3<T>>);

	template<typename T> std::ostream& operator<<(std::ostream& out, const Vector3<T>& vec);

	template<typename T> constexpr Vector3<T> operator*(T scale, const Vector3<T>& vec);
	template<typename T> constexpr Vector3<T> operator/(T scale, const Vector3<T>& vec);
	template<typename T> constexpr Vector3<T> operator%(T scale, const Vector3<T>& vec);
}

namespace std
{
	template<class T> struct hash<Nz::Vector3<T>>;
}

#include <Nazara/Math/Vector3.inl>

#endif // NAZARA_MATH_VECTOR3_HPP
