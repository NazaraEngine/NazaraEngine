// Copyright (C) 2015 Rémi Bèges - Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VECTOR3_HPP
#define NAZARA_VECTOR3_HPP

#include <Nazara/Core/String.hpp>

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
			Vector3(const T vec[3]);
			Vector3(const Vector2<T>& vec, T Z = 0.0);
			template<typename U> explicit Vector3(const Vector3<U>& vec);
			Vector3(const Vector3& vec) = default;
			explicit Vector3(const Vector4<T>& vec);
			~Vector3() = default;

			T AbsDotProduct(const Vector3& vec) const;
			T AngleBetween(const Vector3& vec) const;

			Vector3 CrossProduct(const Vector3& vec) const;

			T Distance(const Vector3& vec) const;
			float Distancef(const Vector3& vec) const;
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
			Vector3& Set(const T vec[3]);
			Vector3& Set(const Vector2<T>& vec, T Z = 0.0);
			Vector3& Set(const Vector3<T>& vec);
			template<typename U> Vector3& Set(const Vector3<U>& vec);
			Vector3& Set(const Vector4<T>& vec);

			T SquaredDistance(const Vector3& vec) const;

			String ToString() const;

			operator T* ();
			operator const T* () const;

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
			static T Distance(const Vector3& vec1, const Vector3& vec2);
			static float Distancef(const Vector3& vec1, const Vector3& vec2);
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

	typedef Vector3<double> Vector3d;
	typedef Vector3<float> Vector3f;
	typedef Vector3<int> Vector3i;
	typedef Vector3<unsigned int> Vector3ui;
	typedef Vector3<Int32> Vector3i32;
	typedef Vector3<UInt32> Vector3ui32;

	template<typename T> bool Serialize(SerializationContext& context, const Vector3<T>& vector);
	template<typename T> bool Unserialize(SerializationContext& context, Vector3<T>* vector);
}

template<typename T> std::ostream& operator<<(std::ostream& out, const Nz::Vector3<T>& vec);

template<typename T> Nz::Vector3<T> operator*(T scale, const Nz::Vector3<T>& vec);
template<typename T> Nz::Vector3<T> operator/(T scale, const Nz::Vector3<T>& vec);

#include <Nazara/Math/Vector3.inl>

#endif // NAZARA_VECTOR3_HPP
