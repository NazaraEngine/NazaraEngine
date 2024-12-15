// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_MATH_RECT_HPP
#define NAZARA_MATH_RECT_HPP

#include <Nazara/Math/Enums.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <NazaraUtils/Constants.hpp>
#include <NazaraUtils/EnumArray.hpp>
#include <string>

namespace Nz
{
	struct SerializationContext;

	template<typename T>
	class Rect
	{
		public:
			constexpr Rect() = default;
			constexpr Rect(T Width, T Height);
			constexpr Rect(T X, T Y, T Width, T Height);
			constexpr explicit Rect(const Vector2<T>& lengths);
			constexpr explicit Rect(const Vector2<T>& pos, const Vector2<T>& lengths);
			template<typename U> constexpr explicit Rect(const Rect<U>& rect);

			constexpr bool ApproxEqual(const Rect& rect, T maxDifference = std::numeric_limits<T>::epsilon()) const;

			constexpr bool Contains(T X, T Y) const;
			constexpr bool Contains(const Rect& rect) const;
			constexpr bool Contains(const Vector2<T>& point) const;

			constexpr Rect& ExtendTo(T X, T Y);
			constexpr Rect& ExtendTo(const Rect& rect);
			constexpr Rect& ExtendTo(const Vector2<T>& point);

			constexpr Vector2<T> GetCenter() const;
			template<CoordinateSystem CS = CoordinateSystem::Cartesian> constexpr Vector2<T> GetCorner(RectCorner corner) const;
			template<CoordinateSystem CS = CoordinateSystem::Cartesian> constexpr EnumArray<RectCorner, Vector2<T>> GetCorners() const;
			constexpr Vector2<T> GetLengths() const;
			constexpr Vector2<T> GetMaximum() const;
			constexpr T GetMaxX() const;
			constexpr T GetMaxY() const;
			constexpr Vector2<T> GetMinimum() const;
			constexpr T GetMinX() const;
			constexpr T GetMinY() const;
			constexpr Vector2<T> GetNegativeVertex(const Vector2<T>& normal) const;
			constexpr Vector2<T> GetPosition() const;
			constexpr Vector2<T> GetPositiveVertex(const Vector2<T>& normal) const;

			constexpr bool Intersect(const Rect& rect, Rect* intersection = nullptr) const;

			constexpr bool IsNull() const;
			constexpr bool IsValid() const;

			constexpr Rect& Scale(T scalar);
			constexpr Rect& Scale(const Vector2<T>& vec);

			constexpr Rect& ScaleAroundCenter(T scalar);
			constexpr Rect& ScaleAroundCenter(const Vector2<T>& vec);

			std::string ToString() const;

			constexpr Rect& Translate(const Vector2<T>& translation);

			constexpr T& operator[](std::size_t i);
			constexpr const T& operator[](std::size_t i) const;

			constexpr bool operator==(const Rect& rect) const;
			constexpr bool operator!=(const Rect& rect) const;

			static constexpr bool ApproxEqual(const Rect& lhs, const Rect& rhs, T maxDifference = std::numeric_limits<T>::epsilon());
			static constexpr Rect FromExtends(const Vector2<T>& vec1, const Vector2<T>& vec2);
			static constexpr Rect Lerp(const Rect& from, const Rect& to, T interpolation);
			static constexpr Rect Invalid();
			static constexpr Rect Zero();

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
	template<typename T> bool Deserialize(SerializationContext& context, Rect<T>* rect, TypeTag<Rect<T>>);

	template<typename T> std::ostream& operator<<(std::ostream& out, const Nz::Rect<T>& rect);
}

#include <Nazara/Math/Rect.inl>

#endif // NAZARA_MATH_RECT_HPP
