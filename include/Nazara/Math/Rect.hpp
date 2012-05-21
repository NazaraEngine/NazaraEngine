// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
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
		NzRect();
		NzRect(T X, T Y, T Width, T Height);
		NzRect(T rect[4]);
		template<typename U> explicit NzRect(const NzRect<U>& rect);
		NzRect(const NzRect& rect) = default;
		~NzRect() = default;

		bool Contains(T X, T Y) const;
		bool Contains(const NzVector2<T>& point) const;
		bool Contains(const NzRect& rect) const;

		void ExtendTo(const NzVector2<T>& point);
		void ExtendTo(const NzRect& rect);

		bool Intersect(const NzRect& rect) const;
		bool Intersect(const NzRect& rect, NzRect& intersection) const;

		bool IsValid() const;

		NzString ToString() const;

		operator NzString() const;

		T& operator[](unsigned int i);
		T operator[](unsigned int i) const;

		T x, y, width, height;
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzRect<T>& vec);

typedef NzRect<double> NzRectd;
typedef NzRect<float> NzRectf;
typedef NzRect<int> NzRecti;
typedef NzRect<unsigned int> NzRectui;

#include <Nazara/Math/Rect.inl>

#endif // NAZARA_RECT_HPP
