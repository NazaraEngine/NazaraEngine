// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FRUSTUM_HPP
#define NAZARA_FRUSTUM_HPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Math/BoundingBox.hpp>
#include <Nazara/Math/Enums.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/OrientedCube.hpp>
#include <Nazara/Math/Plane.hpp>
#include <Nazara/Math/Sphere.hpp>
#include <Nazara/Math/Vector3.hpp>

template<typename T>
class NzFrustum
{
	public:
		NzFrustum() = default;
		template<typename U> explicit NzFrustum(const NzFrustum<U>& frustum);
		NzFrustum(const NzFrustum& frustum) = default;
		~NzFrustum() = default;

		NzFrustum& Build(T angle, T ratio, T zNear, T zFar, const NzVector3<T>& eye, const NzVector3<T>& target, const NzVector3<T>& up = NzVector3<T>::Up());

		bool Contains(const NzBoundingBox<T>& box) const;
		bool Contains(const NzCube<T>& cube) const;
		bool Contains(const NzOrientedCube<T>& orientedCube) const;
		bool Contains(const NzSphere<T>& sphere) const;
		bool Contains(const NzVector3<T>& point) const;
		bool Contains(const NzVector3<T>* points, unsigned int pointCount) const;

		NzFrustum& Extract(const NzMatrix4<T>& clipMatrix);
		NzFrustum& Extract(const NzMatrix4<T>& view, const NzMatrix4<T>& projection);

		const NzVector3<T>& GetCorner(nzCorner corner) const;
		const NzPlane<T>& GetPlane(nzFrustumPlane plane) const;

		nzIntersectionSide Intersect(const NzBoundingBox<T>& box) const;
		nzIntersectionSide Intersect(const NzCube<T>& cube) const;
		nzIntersectionSide Intersect(const NzOrientedCube<T>& orientedCube) const;
		nzIntersectionSide Intersect(const NzSphere<T>& sphere) const;
		nzIntersectionSide Intersect(const NzVector3<T>* points, unsigned int pointCount) const;

		NzFrustum& Set(const NzFrustum& frustum);
		template<typename U> NzFrustum& Set(const NzFrustum<U>& frustum);

		NzString ToString() const;

	private:
		NzVector3<T> m_corners[nzCorner_Max+1];
		NzPlane<T> m_planes[nzFrustumPlane_Max+1];
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzFrustum<T>& frustum);

typedef NzFrustum<double> NzFrustumd;
typedef NzFrustum<float> NzFrustumf;

#include <Nazara/Math/Frustum.inl>

#endif // NAZARA_FRUSTUM_HPP
