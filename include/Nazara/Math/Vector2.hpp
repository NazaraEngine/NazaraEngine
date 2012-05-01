// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VECTOR2_HPP
#define NAZARA_VECTOR2_HPP

#include <Nazara/Core/String.hpp>

template<typename T> class NzVector2
{
	public:
		NzVector2();
		NzVector2(T X, T Y);
		explicit NzVector2(T scale);
		NzVector2(T vec[2]);
		template<typename U> explicit NzVector2(const NzVector2<U>& vec);
		NzVector2(const NzVector2& vec) = default;
		~NzVector2() = default;

		T AbsDotProduct(const NzVector2& vec) const;
		double Distance(const NzVector2& vec) const;
		T DotProduct(const NzVector2& vec) const;
		NzVector2 GetNormal() const;
		void MakeCeil(const NzVector2& vec);
		void MakeFloor(const NzVector2& vec);
		double Length() const;
		double Normalize();
		T SquaredDistance(const NzVector2& vec) const;
		T SquaredLength() const;

		NzString ToString() const;

		operator NzString() const;

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

		T x;
		T y;
};

template<typename T> std::ostream& operator<<(std::ostream& out, const NzVector2<T>& vec);

template<typename T> NzVector2<T> operator*(T scale, const NzVector2<T>& vec);
template<typename T> NzVector2<T> operator/(T scale, const NzVector2<T>& vec);

typedef NzVector2<double> NzVector2d;
typedef NzVector2<float> NzVector2f;
typedef NzVector2<int> NzVector2i;

#include <Nazara/Math/Vector2.inl>

#endif // NAZARA_VECTOR2_HPP
