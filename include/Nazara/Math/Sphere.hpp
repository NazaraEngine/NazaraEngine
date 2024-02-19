// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_MATH_SPHERE_HPP
#define NAZARA_MATH_SPHERE_HPP

#include <Nazara/Math/Vector3.hpp>
#include <string>

namespace Nz
{
	struct SerializationContext;

	template<typename T> class Box;

	template<typename T>
	class Sphere
	{
		public:
			constexpr Sphere() = default;
			constexpr Sphere(T X, T Y, T Z, T Radius);
			constexpr Sphere(const Vector3<T>& center, T Radius);
			constexpr Sphere(const T sphere[4]);
			template<typename U> constexpr explicit Sphere(const Sphere<U>& sphere);
			constexpr Sphere(const Sphere&) = default;
			constexpr Sphere(Sphere&&) = default;
			~Sphere() = default;

			constexpr bool ApproxEqual(const Sphere& sphere, T maxDifference = std::numeric_limits<T>::epsilon()) const;

			constexpr bool Contains(T X, T Y, T Z, T epsilon = std::numeric_limits<T>::epsilon()) const;
			constexpr bool Contains(const Box<T>& box, T epsilon = std::numeric_limits<T>::epsilon()) const;
			constexpr bool Contains(const Vector3<T>& point, T epsilon = std::numeric_limits<T>::epsilon()) const;

			T Distance(T X, T Y, T Z) const;
			T Distance(const Vector3<T>& point) const;

			Sphere& ExtendTo(T X, T Y, T Z);
			Sphere& ExtendTo(const Vector3<T>& point);

			constexpr Vector3<T> GetNegativeVertex(const Vector3<T>& normal) const;
			constexpr Vector3<T> GetPosition() const;
			constexpr Vector3<T> GetPositiveVertex(const Vector3<T>& normal) const;

			constexpr bool Intersect(const Box<T>& box) const;
			constexpr bool Intersect(const Sphere& sphere) const;

			constexpr bool IsValid() const;

			std::string ToString() const;

			constexpr T& operator[](std::size_t i);
			constexpr T operator[](std::size_t i) const;

			constexpr Sphere operator*(T scalar) const;

			constexpr Sphere& operator=(const Sphere& other) = default;
			constexpr Sphere& operator=(Sphere&&) = default;

			constexpr Sphere& operator*=(T scalar);

			constexpr bool operator==(const Sphere& sphere) const;
			constexpr bool operator!=(const Sphere& sphere) const;
			constexpr bool operator<(const Sphere& sphere) const;
			constexpr bool operator<=(const Sphere& sphere) const;
			constexpr bool operator>(const Sphere& sphere) const;
			constexpr bool operator>=(const Sphere& sphere) const;

			static constexpr bool ApproxEqual(const Sphere& lhs, const Sphere& rhs, T maxDifference = std::numeric_limits<T>::epsilon());
			static constexpr Sphere Lerp(const Sphere& from, const Sphere& to, T interpolation);
			static constexpr Sphere Unit();
			static constexpr Sphere Zero();

			T x, y, z, radius;
	};

	using Sphered = Sphere<double>;
	using Spheref = Sphere<float>;

	template<typename T> bool Serialize(SerializationContext& context, const Sphere<T>& sphere, TypeTag<Sphere<T>>);
	template<typename T> bool Unserialize(SerializationContext& context, Sphere<T>* sphere, TypeTag<Sphere<T>>);

	template<typename T> std::ostream& operator<<(std::ostream& out, const Nz::Sphere<T>& sphere);
}

#include <Nazara/Math/Sphere.inl>

#endif // NAZARA_MATH_SPHERE_HPP
