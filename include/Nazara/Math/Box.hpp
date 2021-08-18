// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BOX_HPP
#define NAZARA_BOX_HPP

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
			Box(const T box[6]);
			Box(const Rect<T>& rect);
			Box(const Vector3<T>& lengths);
			Box(const Vector3<T>& vec1, const Vector3<T>& vec2);
			template<typename U> explicit Box(const Box<U>& box);
			Box(const Box& box) = default;
			~Box() = default;

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

			bool IsValid() const;

			Box& MakeZero();

			Box& Set(T Width, T Height, T Depth);
			Box& Set(T X, T Y, T Z, T Width, T Height, T Depth);
			Box& Set(const T box[6]);
			Box& Set(const Rect<T>& rect);
			Box& Set(const Vector3<T>& lengths);
			Box& Set(const Vector3<T>& vec1, const Vector3<T>& vec2);
			template<typename U> Box& Set(const Box<U>& box);

			std::string ToString() const;

			Box& Transform(const Matrix4<T>& matrix, bool applyTranslation = true);
			Box& Translate(const Vector3<T>& translation);

			T& operator[](std::size_t i);
			T operator[](std::size_t i) const;

			Box operator*(T scalar) const;
			Box operator*(const Vector3<T>& vec) const;
			Box& operator=(const Box& other) = default;

			Box& operator*=(T scalar);
			Box& operator*=(const Vector3<T>& vec);

			bool operator==(const Box& box) const;
			bool operator!=(const Box& box) const;

			static Box Lerp(const Box& from, const Box& to, T interpolation);
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
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nz::Box<T>& box);


#include <Nazara/Math/Box.inl>

#endif // NAZARA_BOX_HPP
