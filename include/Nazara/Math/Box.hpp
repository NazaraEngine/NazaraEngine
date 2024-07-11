// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_MATH_BOX_HPP
#define NAZARA_MATH_BOX_HPP

#include <Nazara/Math/Enums.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Sphere.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <NazaraUtils/EnumArray.hpp>
#include <string>

namespace Nz
{
	struct SerializationContext;

	template<typename T>
	class Box
	{
		public:
			constexpr Box() = default;
			constexpr Box(T Width, T Height, T Depth);
			constexpr Box(T X, T Y, T Z, T Width, T Height, T Depth);
			constexpr Box(const Rect<T>& rect);
			constexpr explicit Box(const Vector3<T>& lengths);
			constexpr explicit Box(const Vector3<T>& pos, const Vector3<T>& lengths);
			template<typename U> constexpr explicit Box(const Box<U>& box);
			constexpr Box(const Box&) = default;
			constexpr Box(Box&&) = default;
			~Box() = default;

			constexpr bool ApproxEqual(const Box& box, T maxDifference = std::numeric_limits<T>::epsilon()) const;

			constexpr bool Contains(T X, T Y, T Z) const;
			constexpr bool Contains(const Box& box) const;
			constexpr bool Contains(const Vector3<T>& point) const;

			constexpr Box& ExtendTo(T X, T Y, T Z);
			constexpr Box& ExtendTo(const Box& box);
			constexpr Box& ExtendTo(const Vector3<T>& point);

			constexpr Sphere<T> GetBoundingSphere() const;
			constexpr Vector3<T> GetCenter() const;
			constexpr Vector3<T> GetCorner(BoxCorner corner) const;
			constexpr EnumArray<BoxCorner, Vector3<T>> GetCorners() const;
			constexpr Vector3<T> GetLengths() const;
			constexpr Vector3<T> GetMaximum() const;
			constexpr Vector3<T> GetMinimum() const;
			constexpr Vector3<T> GetNegativeVertex(const Vector3<T>& normal) const;
			constexpr Vector3<T> GetPosition() const;
			constexpr Vector3<T> GetPositiveVertex(const Vector3<T>& normal) const;
			constexpr T GetRadius() const;
			constexpr Sphere<T> GetSquaredBoundingSphere() const;
			constexpr T GetSquaredRadius() const;

			constexpr bool Intersect(const Box& box, Box* intersection = nullptr) const;

			constexpr bool IsNull() const;
			constexpr bool IsValid() const;

			constexpr Box& Scale(T scalar);
			constexpr Box& Scale(const Vector3<T>& vec);

			constexpr Box& ScaleAroundCenter(T scalar);
			constexpr Box& ScaleAroundCenter(const Vector3<T>& vec);

			std::string ToString() const;

			Box& Transform(const Matrix4<T>& matrix, bool applyTranslation = true);
			constexpr Box& Translate(const Vector3<T>& translation);

			constexpr T& operator[](std::size_t i);
			constexpr const T& operator[](std::size_t i) const;

			constexpr Box& operator=(const Box&) = default;
			constexpr Box& operator=(Box&&) = default;

			constexpr bool operator==(const Box& box) const;
			constexpr bool operator!=(const Box& box) const;

			static constexpr Box ApproxEqual(const Box& lhs, const Box& rhs, T maxDifference = std::numeric_limits<T>::epsilon());
			static constexpr Box FromExtents(const Vector3<T>& vec1, const Vector3<T>& vec2);
			static constexpr Box Lerp(const Box& from, const Box& to, T interpolation);
			static constexpr Box Invalid();
			static constexpr Box Zero();

			T x, y, z, width, height, depth;
	};

	using Boxd = Box<double>;
	using Boxf = Box<float>;
	using Boxi = Box<int>;
	using Boxui = Box<unsigned int>;
	using Boxi32 = Box<Int32>;
	using Boxui32 = Box<UInt32>;

	template<typename T> bool Serialize(SerializationContext& context, const Box<T>& box, TypeTag<Box<T>>);
	template<typename T> bool Deserialize(SerializationContext& context, Box<T>* box, TypeTag<Box<T>>);

	template<typename T> std::ostream& operator<<(std::ostream& out, const Nz::Box<T>& box);
}

#include <Nazara/Math/Box.inl>

#endif // NAZARA_MATH_BOX_HPP
