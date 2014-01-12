// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VECTOR2_HPP
#define NAZARA_VECTOR2_HPP

#include <Nazara/Core/String.hpp>

template<typename T>
class NzVector2
{
	public:
		NzVector2() = default;
		NzVector2(T X, T Y);
		explicit NzVector2(T scale);
		NzVector2(const T vec[2]);
		template<typename U> explicit NzVector2(const NzVector2<U>& vec);
		NzVector2(const NzVector2& vec) = default;
		~NzVector2() = default;

		T AbsDotProduct(const NzVector2& vec) const;

		T Distance(const NzVector2& vec) const;
		float Distancef(const NzVector2& vec) const;

		T DotProduct(const NzVector2& vec) const;

		T GetLength() const;
		float GetLengthf() const;
		NzVector2 GetNormal(T* length = nullptr) const;
		T GetSquaredLength() const;

		NzVector2& MakeUnitX();
		NzVector2& MakeUnitY();
		NzVector2& MakeZero();

		NzVector2& Maximize(const NzVector2& vec);
		NzVector2& Minimize(const NzVector2& vec);

		NzVector2& Normalize(T* length = nullptr);

		NzVector2& Set(T X, T Y);
		NzVector2& Set(T scale);
		NzVector2& Set(const T vec[2]);
		NzVector2& Set(const NzVector2& vec);
		template<typename U> NzVector2& Set(const NzVector2<U>& vec);

		T SquaredDistance(const NzVector2& vec) const;

		NzString ToString() const;

		operator T*();
		operator const T*() const;

		T& operator[](unsigned int i);
		T operator[](unsigned int i) const;

		const NzVector2& operator+() const;
		NzVector2 operator-() const;

		NzVector2 operator+(const NzVector2& vec) const;
		NzVector2 operator-(const NzVector2& vec) const;
		NzVector2 operator*(const NzVector2& vec) const;
		NzVector2 operator*(T scale) const;
		NzVector2 operator/(const NzVector2& vec) const;
		NzVector2 operator/(T scale) const;

		NzVector2& operator+=(const NzVector2& vec);
		NzVector2& operator-=(const NzVector2& vec);
		NzVector2& operator*=(const NzVector2& vec);
		NzVector2& operator*=(T scale);
		NzVector2& operator/=(const NzVector2& vec);
		NzVector2& operator/=(T scale);

		bool operator==(const NzVector2& vec) const;
		bool operator!=(const NzVector2& vec) const;
		bool operator<(const NzVector2& vec) const;
		bool operator<=(const NzVector2& vec) const;
		bool operator>(const NzVector2& vec) const;
		bool operator>=(const NzVector2& vec) const;

		static NzVector2 Lerp(const NzVector2& from, const NzVector2& to, T interpolation);
		static NzVector2 UnitX();
		static NzVector2 UnitY();
		static NzVector2 Zero();

		T x, y;
};

template<typename T> std::ostream& operator<<(std::ostream& out, const NzVector2<T>& vec);

template<typename T> NzVector2<T> operator*(T scale, const NzVector2<T>& vec);
template<typename T> NzVector2<T> operator/(T scale, const NzVector2<T>& vec);

typedef NzVector2<double> NzVector2d;
typedef NzVector2<float> NzVector2f;
typedef NzVector2<int> NzVector2i;
typedef NzVector2<unsigned int> NzVector2ui;

#include <Nazara/Math/Vector2.inl>

#endif // NAZARA_VECTOR2_HPP
