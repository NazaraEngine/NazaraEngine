// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VECTOR2_HPP
#define NAZARA_VECTOR2_HPP

#include <Nazara/Core/String.hpp>
#include <functional>

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
			Vector2(const T vec[2]);
			template<typename U> explicit Vector2(const Vector2<U>& vec);
			Vector2(const Vector2& vec) = default;
			explicit Vector2(const Vector3<T>& vec);
			explicit Vector2(const Vector4<T>& vec);
			~Vector2() = default;

			T AbsDotProduct(const Vector2& vec) const;
			T AngleBetween(const Vector2& vec) const;

			T Distance(const Vector2& vec) const;
			float Distancef(const Vector2& vec) const;
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
			Vector2& Set(const T vec[2]);
			Vector2& Set(const Vector2& vec);
			Vector2& Set(const Vector3<T>& vec);
			Vector2& Set(const Vector4<T>& vec);
			template<typename U> Vector2& Set(const Vector2<U>& vec);

			T SquaredDistance(const Vector2& vec) const;

			String ToString() const;

			operator T* ();
			operator const T* () const;

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

			static T DotProduct(const Vector2& vec1, const Vector2& vec2);
			static Vector2 Lerp(const Vector2& from, const Vector2& to, T interpolation);
			static Vector2 Normalize(const Vector2& vec);
			static Vector2 Unit();
			static Vector2 UnitX();
			static Vector2 UnitY();
			static Vector2 Zero();

			T x, y;
	};

	typedef Vector2<double> Vector2d;
	typedef Vector2<float> Vector2f;
	typedef Vector2<int> Vector2i;
	typedef Vector2<unsigned int> Vector2ui;
	typedef Vector2<Int32> Vector2i32;
	typedef Vector2<UInt32> Vector2ui32;

	template<typename T> bool Serialize(SerializationContext& context, const Vector2<T>& vector);
	template<typename T> bool Unserialize(SerializationContext& context, Vector2<T>* vector);
}

template<typename T> std::ostream& operator<<(std::ostream& out, const Nz::Vector2<T>& vec);

template<typename T> Nz::Vector2<T> operator*(T scale, const Nz::Vector2<T>& vec);
template<typename T> Nz::Vector2<T> operator/(T scale, const Nz::Vector2<T>& vec);

namespace std
{
	template<class T> struct hash<Nz::Vector2<T>>;
}

#include <Nazara/Math/Vector2.inl>

#endif // NAZARA_VECTOR2_HPP
