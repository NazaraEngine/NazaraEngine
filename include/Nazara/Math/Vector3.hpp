// Copyright (C) 2017 Rémi Bèges - Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VECTOR3_HPP
#define NAZARA_VECTOR3_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/TypeTag.hpp>
#include <Nazara/Math/Angle.hpp>
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
			Vector3() = default;
			Vector3(T X, T Y, T Z);
			Vector3(T X, const Vector2<T>& vec);
			explicit Vector3(T scale);
			Vector3(const Vector2<T>& vec, T Z = 0.0);
			template<typename U> explicit Vector3(const Vector3<U>& vec);
			Vector3(const Vector3& vec) = default;
			explicit Vector3(const Vector4<T>& vec);
			~Vector3() = default;

			T AbsDotProduct(const Vector3& vec) const;
			RadianAngle<T> AngleBetween(const Vector3& vec) const;

			Vector3 CrossProduct(const Vector3& vec) const;

			template<typename U = T>
			U Distance(const Vector3& vec) const;
			T DotProduct(const Vector3& vec) const;

			T GetLength() const;
			float GetLengthf() const;
			Vector3 GetNormal(T* length = nullptr) const;
			T GetSquaredLength() const;

			Vector3& MakeBackward();
			Vector3& MakeDown();
			Vector3& MakeForward();
			Vector3& MakeLeft();
			Vector3& MakeRight();
			Vector3& MakeUnit();
			Vector3& MakeUnitX();
			Vector3& MakeUnitY();
			Vector3& MakeUnitZ();
			Vector3& MakeUp();
			Vector3& MakeZero();

			Vector3& Maximize(const Vector3& vec);
			Vector3& Minimize(const Vector3& vec);

			Vector3& Normalize(T* length = nullptr);

			Vector3& Set(T X, T Y, T Z);
			Vector3& Set(T X, const Vector2<T>& vec);
			Vector3& Set(T scale);
			Vector3& Set(const T* vec);
			Vector3& Set(const Vector2<T>& vec, T Z = 0.0);
			template<typename U> Vector3& Set(const Vector3<U>& vec);
			Vector3& Set(const Vector4<T>& vec);

			T SquaredDistance(const Vector3& vec) const;

			std::string ToString() const;

			T& operator[](std::size_t i);
			T operator[](std::size_t i) const;

			const Vector3& operator+() const;
			Vector3 operator-() const;

			Vector3 operator+(const Vector3& vec) const;
			Vector3 operator-(const Vector3& vec) const;
			Vector3 operator*(const Vector3& vec) const;
			Vector3 operator*(T scale) const;
			Vector3 operator/(const Vector3& vec) const;
			Vector3 operator/(T scale) const;
			Vector3& operator=(const Vector3& vec) = default;

			Vector3& operator+=(const Vector3& vec);
			Vector3& operator-=(const Vector3& vec);
			Vector3& operator*=(const Vector3& vec);
			Vector3& operator*=(T scale);
			Vector3& operator/=(const Vector3& vec);
			Vector3& operator/=(T scale);

			bool operator==(const Vector3& vec) const;
			bool operator!=(const Vector3& vec) const;
			bool operator<(const Vector3& vec) const;
			bool operator<=(const Vector3& vec) const;
			bool operator>(const Vector3& vec) const;
			bool operator>=(const Vector3& vec) const;

			static Vector3 Backward();
			static Vector3 CrossProduct(const Vector3& vec1, const Vector3& vec2);
			static T DotProduct(const Vector3& vec1, const Vector3& vec2);
			template<typename U = T> static U Distance(const Vector3& vec1, const Vector3& vec2);
			static Vector3 Down();
			static Vector3 Forward();
			static Vector3 Left();
			static Vector3 Lerp(const Vector3& from, const Vector3& to, T interpolation);
			static Vector3 Normalize(const Vector3& vec);
			static Vector3 Right();
			static T SquaredDistance(const Vector3& vec1, const Vector3& vec2);
			static Vector3 Unit();
			static Vector3 UnitX();
			static Vector3 UnitY();
			static Vector3 UnitZ();
			static Vector3 Up();
			static Vector3 Zero();

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
	template<typename T> bool Unserialize(SerializationContext& context, Vector3<T>* vector, TypeTag<Vector3<T>>);
}

template<typename T> std::ostream& operator<<(std::ostream& out, const Nz::Vector3<T>& vec);

template<typename T> Nz::Vector3<T> operator*(T scale, const Nz::Vector3<T>& vec);
template<typename T> Nz::Vector3<T> operator/(T scale, const Nz::Vector3<T>& vec);

namespace std
{
	template<class T> struct hash<Nz::Vector3<T>>;
}

#include <Nazara/Math/Vector3.inl>

#endif // NAZARA_VECTOR3_HPP
