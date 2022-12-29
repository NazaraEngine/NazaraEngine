// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATH_BOX_HPP
#define NAZARA_MATH_BOX_HPP

#include <Nazara/Math/Enums.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Sphere.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <string>

namespace Nz
{
	struct SerializationContext;

	template<typename T>
	class Box
	{
		public:
			Box() = default;
			Box(T Width, T Height, T Depth);
			Box(T X, T Y, T Z, T Width, T Height, T Depth);
			Box(const Rect<T>& rect);
			explicit Box(const Vector3<T>& lengths);
			explicit Box(const Vector3<T>& pos, const Vector3<T>& lengths);
			template<typename U> explicit Box(const Box<U>& box);
			Box(const Box&) = default;
			Box(Box&&) noexcept = default;
			~Box() = default;

			bool ApproxEquals(const Box& box, T maxDifference = 0) const;

			bool Contains(T X, T Y, T Z) const;
			bool Contains(const Box& box) const;
			bool Contains(const Vector3<T>& point) const;

			Box& ExtendTo(T X, T Y, T Z);
			Box& ExtendTo(const Box& box);
			Box& ExtendTo(const Vector3<T>& point);

			Sphere<T> GetBoundingSphere() const;
			Vector3<T> GetCenter() const;
			Vector3<T> GetCorner(BoxCorner corner) const;
			Vector3<T> GetLengths() const;
			Vector3<T> GetMaximum() const;
			Vector3<T> GetMinimum() const;
			Vector3<T> GetNegativeVertex(const Vector3<T>& normal) const;
			Vector3<T> GetPosition() const;
			Vector3<T> GetPositiveVertex(const Vector3<T>& normal) const;
			T GetRadius() const;
			Sphere<T> GetSquaredBoundingSphere() const;
			T GetSquaredRadius() const;

			bool Intersect(const Box& box, Box* intersection = nullptr) const;

			bool IsNull() const;
			bool IsValid() const;

			Box& Scale(T scalar);
			Box& Scale(const Vector3<T>& vec);

			std::string ToString() const;

			Box& Transform(const Matrix4<T>& matrix, bool applyTranslation = true);
			Box& Translate(const Vector3<T>& translation);

			T& operator[](std::size_t i);
			const T& operator[](std::size_t i) const;

			Box& operator=(const Box&) = default;
			Box& operator=(Box&&) noexcept = default;

			bool operator==(const Box& box) const;
			bool operator!=(const Box& box) const;

			static Box FromExtends(const Vector3<T>& vec1, const Vector3<T>& vec2);
			static Box Lerp(const Box& from, const Box& to, T interpolation);
			static Box Invalid();
			static Box Zero();

			T x, y, z, width, height, depth;
	};

	using Boxd = Box<double>;
	using Boxf = Box<float>;
	using Boxi = Box<int>;
	using Boxui = Box<unsigned int>;
	using Boxi32 = Box<Int32>;
	using Boxui32 = Box<UInt32>;

	template<typename T> bool Serialize(SerializationContext& context, const Box<T>& box, TypeTag<Box<T>>);
	template<typename T> bool Unserialize(SerializationContext& context, Box<T>* box, TypeTag<Box<T>>);

	template<typename T> std::ostream& operator<<(std::ostream& out, const Nz::Box<T>& box);
}

#include <Nazara/Math/Box.inl>

#endif // NAZARA_MATH_BOX_HPP
