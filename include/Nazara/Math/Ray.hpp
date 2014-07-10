// Copyright (C) 2014 Gawaboumga (https://github.com/Gawaboumga) - Jérôme Leclercq
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

template<typename T>
class NzRay
{
	public:
		NzRay() = default;
		NzRay(T X, T Y, T Z, T directionX, T directionY, T directionZ);
		NzRay(const T origin[3], const T direction[3]);
		NzRay(const NzPlane<T>& planeOne, const NzPlane<T>& planeTwo);
		NzRay(const NzVector3<T>& origin, const NzVector3<T>& direction);
		template<typename U> explicit NzRay(const NzRay<U>& ray);
		template<typename U> explicit NzRay(const NzVector3<U>& origin, const NzVector3<U>& direction);
		NzRay(const NzRay<T>& ray) = default;
		~NzRay() = default;

		T ClosestPoint(const NzVector3<T>& point) const;

		NzVector3<T> GetPoint(T lambda) const;

		//bool Intersect(const NzBoundingVolume<T>& volume, T* closestHit = nullptr, T* farthestHit = nullptr) const;
		bool Intersect(const NzBox<T>& box, T* closestHit = nullptr, T* farthestHit = nullptr) const;
		bool Intersect(const NzBox<T>& box, const NzMatrix4<T>& transform, T* closestHit = nullptr, T* farthestHit = nullptr) const;
		//bool Intersect(const NzOrientedBox<T>& orientedBox, T* closestHit = nullptr, T* farthestHit = nullptr) const;
		bool Intersect(const NzPlane<T>& plane, T* hit = nullptr) const;
		bool Intersect(const NzSphere<T>& sphere, T* closestHit = nullptr, T* farthestHit = nullptr) const;

		NzRay& MakeAxisX();
		NzRay& MakeAxisY();
		NzRay& MakeAxisZ();

		NzRay& Set(T X, T Y, T Z, T directionX, T directionY, T directionZ);
		NzRay& Set(const T origin[3], const T direction[3]);
		NzRay& Set(const NzPlane<T>& planeOne, const NzPlane<T>& planeTwo);
		NzRay& Set(const NzRay& ray);
		NzRay& Set(const NzVector3<T>& origin, const NzVector3<T>& direction);
		template<typename U> NzRay& Set(const NzRay<U>& ray);
		template<typename U> NzRay& Set(const NzVector3<U>& origin, const NzVector3<U>& direction);

		NzString ToString() const;

		NzVector3<T> operator*(T lambda) const;

		static NzRay AxisX();
		static NzRay AxisY();
		static NzRay AxisZ();
		static NzRay Lerp(const NzRay& from, const NzRay& to, T interpolation);

		NzVector3<T> direction, origin;
};

template<typename T> std::ostream& operator<<(std::ostream& out, const NzRay<T>& vec);

typedef NzRay<double> NzRayd;
typedef NzRay<float> NzRayf;

#include <Nazara/Math/Ray.inl>

#endif // NAZARA_RAY_HPP
