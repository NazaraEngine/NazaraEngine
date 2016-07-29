// Copyright (C) 2015 Gawaboumga (https://github.com/Gawaboumga) - Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RAY_HPP
#define NAZARA_RAY_HPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/OrientedBox.hpp>
#include <Nazara/Math/Plane.hpp>
#include <Nazara/Math/Sphere.hpp>
#include <Nazara/Math/Vector3.hpp>

namespace Nz
{
	struct SerializationContext;

	template<typename T>
	class Ray
	{
		public:
			Ray() = default;
			Ray(T X, T Y, T Z, T directionX, T directionY, T directionZ);
			Ray(const Vector3<T>& origin, const Vector3<T>& direction);
			Ray(const T origin[3], const T direction[3]);
			Ray(const Plane<T>& planeOne, const Plane<T>& planeTwo);
			template<typename U> explicit Ray(const Ray<U>& ray);
			template<typename U> explicit Ray(const Vector3<U>& origin, const Vector3<U>& direction);
			Ray(const Ray<T>& ray) = default;
			~Ray() = default;

			T ClosestPoint(const Vector3<T>& point) const;

			Vector3<T> GetPoint(T lambda) const;

			bool Intersect(const BoundingVolume<T>& volume, T* closestHit = nullptr, T* furthestHit = nullptr) const;
			bool Intersect(const Box<T>& box, T* closestHit = nullptr, T* furthestHit = nullptr) const;
			bool Intersect(const Box<T>& box, const Matrix4<T>& transform, T* closestHit = nullptr, T* furthestHit = nullptr) const;
			bool Intersect(const OrientedBox<T>& orientedBox, T* closestHit = nullptr, T* furthestHit = nullptr) const;
			bool Intersect(const Plane<T>& plane, T* hit = nullptr) const;
			bool Intersect(const Sphere<T>& sphere, T* closestHit = nullptr, T* furthestHit = nullptr) const;
			bool Intersect(const Vector3<T>& firstPoint, const Vector3<T>& secondPoint, const Vector3<T>& thirdPoint, T* hit = nullptr) const;

			Ray& MakeAxisX();
			Ray& MakeAxisY();
			Ray& MakeAxisZ();

			Ray& Set(T X, T Y, T Z, T directionX, T directionY, T directionZ);
			Ray& Set(const Vector3<T>& origin, const Vector3<T>& direction);
			Ray& Set(const T origin[3], const T direction[3]);
			Ray& Set(const Plane<T>& planeOne, const Plane<T>& planeTwo);
			Ray& Set(const Ray& ray);
			template<typename U> Ray& Set(const Ray<U>& ray);
			template<typename U> Ray& Set(const Vector3<U>& origin, const Vector3<U>& direction);

			String ToString() const;

			Vector3<T> operator*(T lambda) const;
			Ray& operator=(const Ray& other) = default;

			bool operator==(const Ray& ray) const;
			bool operator!=(const Ray& ray) const;

			static Ray AxisX();
			static Ray AxisY();
			static Ray AxisZ();
			static Ray Lerp(const Ray& from, const Ray& to, T interpolation);

			Vector3<T> direction, origin;
	};

	typedef Ray<double> Rayd;
	typedef Ray<float> Rayf;

	template<typename T> bool Serialize(SerializationContext& context, const Ray<T>& ray);
	template<typename T> bool Unserialize(SerializationContext& context, Ray<T>* ray);
}

template<typename T> std::ostream& operator<<(std::ostream& out, const Nz::Ray<T>& vec);

#include <Nazara/Math/Ray.inl>

#endif // NAZARA_RAY_HPP
