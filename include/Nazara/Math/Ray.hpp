// Copyright (C) 2024 Gawaboumga (https://github.com/Gawaboumga) - Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATH_RAY_HPP
#define NAZARA_MATH_RAY_HPP

#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/OrientedBox.hpp>
#include <Nazara/Math/Plane.hpp>
#include <Nazara/Math/Sphere.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <string>

namespace Nz
{
	struct SerializationContext;

	template<typename T>
	class Ray
	{
		public:
			constexpr Ray() = default;
			constexpr Ray(T X, T Y, T Z, T directionX, T directionY, T directionZ);
			constexpr Ray(const Vector3<T>& origin, const Vector3<T>& direction);
			constexpr Ray(const T origin[3], const T direction[3]);
			Ray(const Plane<T>& planeOne, const Plane<T>& planeTwo);
			template<typename U> constexpr explicit Ray(const Ray<U>& ray);
			template<typename U> constexpr explicit Ray(const Vector3<U>& origin, const Vector3<U>& direction);
			constexpr Ray(const Ray&) = default;
			constexpr Ray(Ray&&) = default;
			~Ray() = default;

			constexpr bool ApproxEqual(const Ray& ray, T maxDifference = std::numeric_limits<T>::epsilon()) const;

			constexpr T ClosestPoint(const Vector3<T>& point) const;

			constexpr Vector3<T> GetPoint(T lambda) const;

			constexpr bool Intersect(const BoundingVolume<T>& volume, T* closestHit = nullptr, T* furthestHit = nullptr) const;
			constexpr bool Intersect(const Box<T>& box, T* closestHit = nullptr, T* furthestHit = nullptr) const;
			constexpr bool Intersect(const Box<T>& box, const Matrix4<T>& transform, T* closestHit = nullptr, T* furthestHit = nullptr) const;
			constexpr bool Intersect(const Plane<T>& plane, T* hit = nullptr) const;
			constexpr bool Intersect(const Sphere<T>& sphere, T* closestHit = nullptr, T* furthestHit = nullptr) const;
			constexpr bool Intersect(const Vector3<T>& firstPoint, const Vector3<T>& secondPoint, const Vector3<T>& thirdPoint, T* hit = nullptr) const;

			std::string ToString() const;

			constexpr Ray& operator=(const Ray&) = default;
			constexpr Ray& operator=(Ray&&) = default;

			constexpr Vector3<T> operator*(T lambda) const;

			constexpr bool operator==(const Ray& ray) const;
			constexpr bool operator!=(const Ray& ray) const;
			constexpr bool operator<(const Ray& ray) const;
			constexpr bool operator<=(const Ray& ray) const;
			constexpr bool operator>(const Ray& ray) const;
			constexpr bool operator>=(const Ray& ray) const;

			static constexpr bool ApproxEqual(const Ray& lhs, const Ray& rhs, T maxDifference = std::numeric_limits<T>::epsilon());
			static constexpr Ray AxisX();
			static constexpr Ray AxisY();
			static constexpr Ray AxisZ();
			static constexpr Ray Lerp(const Ray& from, const Ray& to, T interpolation);

			Vector3<T> direction, origin;
	};

	using Rayd = Ray<double>;
	using Rayf = Ray<float>;

	template<typename T> bool Serialize(SerializationContext& context, const Ray<T>& ray, TypeTag<Ray<T>>);
	template<typename T> bool Unserialize(SerializationContext& context, Ray<T>* ray, TypeTag<Ray<T>>);

	template<typename T> std::ostream& operator<<(std::ostream& out, const Nz::Ray<T>& vec);
}

#include <Nazara/Math/Ray.inl>

#endif // NAZARA_MATH_RAY_HPP
