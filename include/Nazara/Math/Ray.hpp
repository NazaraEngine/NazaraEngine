// Copyright (C) 2014 Rémi Bèges - Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_RAY_HPP
#define NAZARA_RAY_HPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Math/OrientedBox.hpp>
#include <Nazara/Math/Plane.hpp>
#include <Nazara/Math/Sphere.hpp>
#include <Nazara/Math/Vector3.hpp>

template<typename T> class NzRay
{
	public:
		NzRay() = default;
		NzRay(T X, T Y, T Z, T directionX, T directionY, T directionZ);
		NzRay(const T origin[3], const T direction[3]);
		NzRay(const NzVector3<T>& origin, const NzVector3<T>& direction);
		NzRay(const NzPlane<T>& planeOne, const NzPlane<T>& planeTwo);
		template<typename U> explicit NzRay(const NzVector3<U>& origin, const NzVector3<U>& direction);
		template<typename U> explicit NzRay(const NzRay<U>& ray);
		NzRay(const NzRay<T>& ray) = default;
		~NzRay() = default;

        NzVector3<T> GetClosestPoint(const NzVector3<T>& point) const;
		NzVector3<T> GetPoint(T lambda) const;

		bool Intersect(const NzBox<T>& box, NzVector3<T> * hitPoint = nullptr, NzVector3<T> * hitSecondPoint = nullptr) const;
		bool Intersect(const NzOrientedBox<T>& orientedBox, NzVector3<T> * hitPoint = nullptr, NzVector3<T> * hitSecondPoint = nullptr) const;
		bool Intersect(const NzPlane<T>& plane, NzVector3<T> * hitPoint = nullptr) const;
		bool Intersect(const NzSphere<T>& sphere, NzVector3<T> * hitPoint = nullptr, NzVector3<T> * hitSecondPoint = nullptr) const;

        NzVector3<T> operator*(T lambda) const;

		NzRay& Set(T X, T Y, T Z, T directionX, T directionY, T directionZ);
		NzRay& Set(const T origin[3], const T direction[3]);
		NzRay& Set(const NzVector3<T>& origin, const NzVector3<T>& direction);
		NzRay& Set(const NzPlane<T>& planeOne, const NzPlane<T>& planeTwo);
		template<typename U> NzRay& Set(const NzVector3<U>& origin, const NzVector3<U>& direction);
		template<typename U> NzRay& Set(const NzRay<U>& ray);
        NzRay& Set(const NzRay& ray);

        NzRay& SetDirection(const NzVector3<T>& direction);
        NzRay& SetOrigin(const NzVector3<T>& origin);

		NzString ToString() const;

		static NzRay Lerp(const NzRay& from, const NzRay& to, T interpolation);
		static NzRay UnitX();
		static NzRay UnitY();
		static NzRay UnitZ();

		NzVector3<T> direction, origin;
};

template<typename T> std::ostream& operator<<(std::ostream& out, const NzRay<T>& vec);

typedef NzRay<double> NzRayd;
typedef NzRay<float> NzRayf;

#include <Nazara/Math/Ray.inl>

#endif // NAZARA_RAY_HPP
