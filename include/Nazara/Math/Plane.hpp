// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PLANE_HPP
#define NAZARA_PLANE_HPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Vector3.hpp>

template<typename T>
class NzPlane
{
	public:
		NzPlane() = default;
		NzPlane(T normalX, T normalY, T normalZ, T Distance);
		NzPlane(const T plane[4]);
		NzPlane(const NzVector3<T>& Normal, T Distance);
		NzPlane(const NzVector3<T>& Normal, const NzVector3<T>& point);
		NzPlane(const NzVector3<T>& point1, const NzVector3<T>& point2, const NzVector3<T>& point3);
		template<typename U> explicit NzPlane(const NzPlane<U>& plane);
		NzPlane(const NzPlane& plane) = default;
		~NzPlane() = default;

		T Distance(const NzVector3<T>& point);
		T Distance(T x, T y, T z);

		NzPlane& Set(T normalX, T normalY, T normalZ, T Distance);
		NzPlane& Set(const T plane[4]);
		NzPlane& Set(const NzPlane& plane);
		NzPlane& Set(const NzVector3<T>& Normal, T Distance);
		NzPlane& Set(const NzVector3<T>& Normal, const NzVector3<T>& point);
		NzPlane& Set(const NzVector3<T>& point1, const NzVector3<T>& point2, const NzVector3<T>& point3);
		template<typename U> NzPlane& Set(const NzPlane<U>& plane);

		NzString ToString() const;

		static NzPlane Lerp(const NzPlane& from, const NzPlane& to, T interpolation);

		NzVector3<T> normal;
		T distance;
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzPlane<T>& plane);

typedef NzPlane<double> NzPlaned;
typedef NzPlane<float> NzPlanef;

#include <Nazara/Math/Plane.inl>

#endif // NAZARA_PLANE_HPP
