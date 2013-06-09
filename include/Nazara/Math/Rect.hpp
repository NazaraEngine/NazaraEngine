// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RECT_HPP
#define NAZARA_RECT_HPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Vector2.hpp>

template<typename T>
class NzRect
{
	public:
		NzRect() = default;
		NzRect(T Width, T Height);
		NzRect(T X, T Y, T Width, T Height);
		NzRect(const T rect[4]);
		NzRect(const NzVector2<T>& lengths);
		NzRect(const NzVector2<T>& vec1, const NzVector2<T>& vec2);
		template<typename U> explicit NzRect(const NzRect<U>& rect);
		NzRect(const NzRect& rect) = default;
		~NzRect() = default;

		bool Contains(T X, T Y) const;
		bool Contains(const NzVector2<T>& point) const;
		bool Contains(const NzRect& rect) const;

		NzRect& ExtendTo(T X, T Y);
		NzRect& ExtendTo(const NzVector2<T>& point);
		NzRect& ExtendTo(const NzRect& rect);

		NzVector2<T> GetCenter() const;
		NzVector2<T> GetLengths() const;
		NzVector2<T> GetMaximum() const;
		NzVector2<T> GetMinimum() const;
		NzVector2<T> GetNegativeVertex(const NzVector2<T>& normal) const;
		NzVector2<T> GetPosition() const;
		NzVector2<T> GetPositiveVertex(const NzVector2<T>& normal) const;

		bool Intersect(const NzRect& rect, NzRect* intersection = nullptr) const;

		bool IsValid() const;

		NzRect& MakeZero();

		NzRect& Set(T Width, T Height);
		NzRect& Set(T X, T Y, T Width, T Height);
		NzRect& Set(const T rect[4]);
		NzRect& Set(const NzRect<T>& rect);
		NzRect& Set(const NzVector2<T>& lengths);
		NzRect& Set(const NzVector2<T>& vec1, const NzVector2<T>& vec2);
		template<typename U> NzRect& Set(const NzRect<U>& rect);

		NzString ToString() const;

		NzRect& Translate(const NzVector2<T>& translation);

		T& operator[](unsigned int i);
		T operator[](unsigned int i) const;

		NzRect operator*(T scalar) const;
		NzRect operator*(const NzVector2<T>& vec) const;

		NzRect& operator*=(T scalar);
		NzRect& operator*=(const NzVector2<T>& vec);

		bool operator==(const NzRect& rect) const;
		bool operator!=(const NzRect& rect) const;

		static NzRect Lerp(const NzRect& from, const NzRect& to, T interpolation);
		static NzRect Zero();

		T x, y, width, height;
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzRect<T>& rect);

typedef NzRect<double> NzRectd;
typedef NzRect<float> NzRectf;
typedef NzRect<int> NzRecti;
typedef NzRect<unsigned int> NzRectui;

#include <Nazara/Math/Rect.inl>

#endif // NAZARA_RECT_HPP
