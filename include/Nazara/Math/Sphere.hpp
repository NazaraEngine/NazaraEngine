// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPHERE_HPP
#define NAZARA_SPHERE_HPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Vector3.hpp>

template<typename T>
class NzSphere
{
	public:
		NzSphere() = default;
		NzSphere(T X, T Y, T Z, T Radius);
		//NzSphere(const NzCircle<T>& circle);
		NzSphere(const NzVector3<T>& center, T Radius);
		NzSphere(const T sphere[4]);
		template<typename U> explicit NzSphere(const NzSphere<U>& sphere);
		NzSphere(const NzSphere& sphere) = default;
		~NzSphere() = default;

		bool Contains(T X, T Y, T Z) const;
		//bool Contains(const NzCube<T>& cube) const;
		bool Contains(const NzVector3<T>& point) const;

		T Distance(T X, T Y, T Z) const;
		T Distance(const NzVector3<T>& point) const;

		NzSphere& ExtendTo(T X, T Y, T Z);
		NzSphere& ExtendTo(const NzVector3<T>& point);

		NzVector3<T> GetNegativeVertex(const NzVector3<T>& normal) const;
		NzVector3<T> GetPosition() const;
		NzVector3<T> GetPositiveVertex(const NzVector3<T>& normal) const;

		//bool Intersect(const NzCube<T>& cube) const;
		bool Intersect(const NzSphere& sphere) const;

		bool IsValid() const;

		NzSphere& MakeZero();

		NzSphere& Set(T X, T Y, T Z, T Radius);
		//NzSphere& Set(const NzCircle<T>& rect);
		NzSphere& Set(const NzSphere& sphere);
		NzSphere& Set(const NzVector3<T>& center, T Radius);
		NzSphere& Set(const T sphere[4]);
		template<typename U> NzSphere& Set(const NzSphere<U>& sphere);

		T SquaredDistance(T X, T Y, T Z) const;
		T SquaredDistance(const NzVector3<T>& point) const;

		NzString ToString() const;

		T& operator[](unsigned int i);
		T operator[](unsigned int i) const;

		NzSphere operator*(T scalar) const;

		NzSphere& operator*=(T scalar);

		bool operator==(const NzSphere& sphere) const;
		bool operator!=(const NzSphere& sphere) const;

		static NzSphere Lerp(const NzSphere& from, const NzSphere& to, T interpolation);
		static NzSphere Zero();

		T x, y, z, radius;
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzSphere<T>& sphere);

typedef NzSphere<double> NzSphered;
typedef NzSphere<float> NzSpheref;

#include <Nazara/Math/Sphere.inl>

#endif // NAZARA_SPHERE_HPP
