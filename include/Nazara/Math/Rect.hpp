// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RECT_HPP
#define NAZARA_RECT_HPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Enums.hpp>
#include <Nazara/Math/Vector2.hpp>

namespace Nz
{
	struct SerializationContext;

	template<typename T>
	class Rect
	{
		public:
			Rect() = default;
			Rect(T Width, T Height);
			Rect(T X, T Y, T Width, T Height);
			Rect(const T rect[4]);
			Rect(const Vector2<T>& lengths);
			Rect(const Vector2<T>& vec1, const Vector2<T>& vec2);
			template<typename U> explicit Rect(const Rect<U>& rect);
			Rect(const Rect& rect) = default;
			~Rect() = default;

			bool Contains(T X, T Y) const;
			bool Contains(const Rect& rect) const;
			bool Contains(const Vector2<T>& point) const;

			Rect& ExtendTo(T X, T Y);
			Rect& ExtendTo(const Rect& rect);
			Rect& ExtendTo(const Vector2<T>& point);

			Vector2<T> GetCenter() const;
			Vector2<T> GetCorner(RectCorner corner) const;
			Vector2<T> GetLengths() const;
			Vector2<T> GetMaximum() const;
			Vector2<T> GetMinimum() const;
			Vector2<T> GetNegativeVertex(const Vector2<T>& normal) const;
			Vector2<T> GetPosition() const;
			Vector2<T> GetPositiveVertex(const Vector2<T>& normal) const;

			bool Intersect(const Rect& rect, Rect* intersection = nullptr) const;

			bool IsValid() const;

			Rect& MakeZero();

			Rect& Set(T Width, T Height);
			Rect& Set(T X, T Y, T Width, T Height);
			Rect& Set(const T rect[4]);
			Rect& Set(const Rect<T>& rect);
			Rect& Set(const Vector2<T>& lengths);
			Rect& Set(const Vector2<T>& vec1, const Vector2<T>& vec2);
			template<typename U> Rect& Set(const Rect<U>& rect);

			String ToString() const;

			Rect& Translate(const Vector2<T>& translation);

			T& operator[](unsigned int i);
			T operator[](unsigned int i) const;

			Rect operator*(T scalar) const;
			Rect operator*(const Vector2<T>& vec) const;
			Rect operator/(T scalar) const;
			Rect operator/(const Vector2<T>& vec) const;
			Rect& operator=(const Rect& other) = default;

			Rect& operator*=(T scalar);
			Rect& operator*=(const Vector2<T>& vec);
			Rect& operator/=(T scalar);
			Rect& operator/=(const Vector2<T>& vec);

			bool operator==(const Rect& rect) const;
			bool operator!=(const Rect& rect) const;

			static Rect Lerp(const Rect& from, const Rect& to, T interpolation);
			static Rect Zero();

			T x, y, width, height;
	};

	typedef Rect<double> Rectd;
	typedef Rect<float> Rectf;
	typedef Rect<int> Recti;
	typedef Rect<unsigned int> Rectui;
	typedef Rect<Int32> Recti32;
	typedef Rect<UInt32> Rectui32;

	template<typename T> bool Serialize(SerializationContext& context, const Rect<T>& rect);
	template<typename T> bool Unserialize(SerializationContext& context, Rect<T>* rect);
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nz::Rect<T>& rect);

#include <Nazara/Math/Rect.inl>

#endif // NAZARA_RECT_HPP
