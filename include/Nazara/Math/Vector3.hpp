// Copyright (C) 2012 Rémi Bèges - Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VECTOR3_HPP
#define NAZARA_VECTOR3_HPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Vector2.hpp>

template<typename T> class NzVector3
{
	public:
		NzVector3();
		NzVector3(T X, T Y, T Z);
		explicit NzVector3(T scale);
		NzVector3(T vec[3]);
		NzVector3(const NzVector2<T>& vec, T Z = 0.0);
		template<typename U> explicit NzVector3(const NzVector3<U>& vec);
		NzVector3(const NzVector3& vec) = default;
		~NzVector3() = default;

		T AbsDotProduct(const NzVector3& vec) const;

		NzVector3 CrossProduct(const NzVector3& vec) const;

		T Distance(const NzVector3& vec) const;
		float Distancef(const NzVector3& vec) const;

		T DotProduct(const NzVector3& vec) const;

		NzVector3 GetNormal() const;

		T Length() const;
		float Lengthf() const;

		void MakeForward();
		void MakeLeft();
		void MakeUnitX();
		void MakeUnitY();
		void MakeUnitZ();
		void MakeUp();
		void MakeZero();

		void Maximize(const NzVector3& vec);
		void Minimize(const NzVector3& vec);

		void Normalize();

		void Set(T X, T Y, T Z);
		void Set(T scale);
		void Set(T vec[3]);
		void Set(const NzVector2<T>& vec, T Z = 0.0);
		template<typename U> void Set(const NzVector3<U>& vec);

		T SquaredDistance(const NzVector3& vec) const;
		T SquaredLength() const;

		NzString ToString() const;

		operator NzString() const;

		operator T*();
		operator const T*() const;

		T& operator[](unsigned int i);
		T operator[](unsigned int i) const;

		const NzVector3& operator+() const;
		NzVector3 operator-() const;

		NzVector3 operator+(const NzVector3& vec) const;
		NzVector3 operator-(const NzVector3& vec) const;
		NzVector3 operator*(const NzVector3& vec) const;
		NzVector3 operator*(T scale) const;
		NzVector3 operator/(const NzVector3& vec) const;
		NzVector3 operator/(T scale) const;

		NzVector3& operator+=(const NzVector3& vec);
		NzVector3& operator-=(const NzVector3& vec);
		NzVector3& operator*=(const NzVector3& vec);
		NzVector3& operator*=(T scale);
		NzVector3& operator/=(const NzVector3& vec);
		NzVector3& operator/=(T scale);

		bool operator==(const NzVector3& vec) const;
		bool operator!=(const NzVector3& vec) const;
		bool operator<(const NzVector3& vec) const;
		bool operator<=(const NzVector3& vec) const;
		bool operator>(const NzVector3& vec) const;
		bool operator>=(const NzVector3& vec) const;

		static NzVector3 CrossProduct(const NzVector3& vec1, const NzVector3& vec2);
		static T DotProduct(const NzVector3& vec1, const NzVector3& vec2);
		static NzVector3 Forward();
		static NzVector3 Left();
		static NzVector3 Normalize(const NzVector3& vec);
		static NzVector3 UnitX();
		static NzVector3 UnitY();
		static NzVector3 UnitZ();
		static NzVector3 Up();
		static NzVector3 Zero();

		T x, y, z;
};

template<typename T> std::ostream& operator<<(std::ostream& out, const NzVector3<T>& vec);

template<typename T> NzVector3<T> operator*(T scale, const NzVector3<T>& vec);
template<typename T> NzVector3<T> operator/(T scale, const NzVector3<T>& vec);

typedef NzVector3<double> NzVector3d;
typedef NzVector3<float> NzVector3f;
typedef NzVector3<int> NzVector3i;
typedef NzVector3<unsigned int> NzVector3ui;

#include <Nazara/Math/Vector3.inl>

#endif // NAZARA_VECTOR3_HPP
