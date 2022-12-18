// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATH_RECT_HPP
#define NAZARA_MATH_RECT_HPP

#include <Nazara/Math/Enums.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <string>

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
			explicit Rect(const Vector2<T>& lengths);
			explicit Rect(const Vector2<T>& pos, const Vector2<T>& lengths);
			template<typename U> explicit Rect(const Rect<U>& rect);
			Rect(const Rect&) = default;
			Rect(Rect&&) noexcept = default;
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

			bool IsNull() const;
			bool IsValid() const;

			Rect& MakeZero();

			Rect& Scale(T scalar);
			Rect& Scale(const Vector2<T>& vec);

			std::string ToString() const;

			Rect& Translate(const Vector2<T>& translation);

			T& operator[](std::size_t i);
			const T& operator[](std::size_t i) const;

			Rect& operator=(const Rect&) = default;
			Rect& operator=(Rect&&) noexcept = default;

			bool operator==(const Rect& rect) const;
			bool operator!=(const Rect& rect) const;

			static Rect FromExtends(const Vector2<T>& vec1, const Vector2<T>& vec2);
			static Rect Lerp(const Rect& from, const Rect& to, T interpolation);
			static Rect Invalid();
			static Rect Zero();

			T x, y, width, height;
	};

	using Rectd = Rect<double>;
	using Rectf = Rect<float>;
	using Recti = Rect<int>;
	using Rectui = Rect<unsigned int>;
	using Recti32 = Rect<Int32>;
	using Recti64 = Rect<Int64>;
	using Rectui32 = Rect<UInt32>;
	using Rectui64 = Rect<UInt64>;

	template<typename T> bool Serialize(SerializationContext& context, const Rect<T>& rect, TypeTag<Rect<T>>);
	template<typename T> bool Unserialize(SerializationContext& context, Rect<T>* rect, TypeTag<Rect<T>>);
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nz::Rect<T>& rect);

#include <Nazara/Math/Rect.inl>

#endif // NAZARA_MATH_RECT_HPP
