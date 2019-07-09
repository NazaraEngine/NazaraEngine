// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FRUSTUM_HPP
#define NAZARA_FRUSTUM_HPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Math/BoundingVolume.hpp>
#include <Nazara/Math/Enums.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/OrientedBox.hpp>
#include <Nazara/Math/Plane.hpp>
#include <Nazara/Math/Sphere.hpp>
#include <Nazara/Math/Vector3.hpp>

namespace Nz
{
	struct SerializationContext;

	template<typename T>
	class Frustum
	{
		public:
			Frustum() = default;
			template<typename U> explicit Frustum(const Frustum<U>& frustum);
			Frustum(const Frustum& frustum) = default;
			~Frustum() = default;

			Frustum& Build(T angle, T ratio, T zNear, T zFar, const Vector3<T>& eye, const Vector3<T>& target, const Vector3<T>& up = Vector3<T>::Up());

			bool Contains(const BoundingVolume<T>& volume) const;
			bool Contains(const Box<T>& box) const;
			bool Contains(const OrientedBox<T>& orientedBox) const;
			bool Contains(const Sphere<T>& sphere) const;
			bool Contains(const Vector3<T>& point) const;
			bool Contains(const Vector3<T>* points, unsigned int pointCount) const;

			Frustum& Extract(const Matrix4<T>& clipMatrix);
			Frustum& Extract(const Matrix4<T>& view, const Matrix4<T>& projection);

			const Vector3<T>& GetCorner(BoxCorner corner) const;
			const Plane<T>& GetPlane(FrustumPlane plane) const;

			IntersectionSide Intersect(const BoundingVolume<T>& volume) const;
			IntersectionSide Intersect(const Box<T>& box) const;
			IntersectionSide Intersect(const OrientedBox<T>& orientedBox) const;
			IntersectionSide Intersect(const Sphere<T>& sphere) const;
			IntersectionSide Intersect(const Vector3<T>* points, unsigned int pointCount) const;

			Frustum& operator=(const Frustum& other) = default;

			Frustum& Set(const Frustum& frustum);
			template<typename U> Frustum& Set(const Frustum<U>& frustum);

			String ToString() const;

			template<typename U>
			friend bool Serialize(SerializationContext& context, const Frustum<U>& frustum, TypeTag<Frustum<U>>);

			template<typename U>
			friend bool Unserialize(SerializationContext& context, Frustum<U>* frustum, TypeTag<Frustum<U>>);

		private:
			Vector3<T> m_corners[BoxCorner_Max+1];
			Plane<T> m_planes[FrustumPlane_Max+1];
	};

	using Frustumd = Frustum<double>;
	using Frustumf = Frustum<float>;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nz::Frustum<T>& frustum);

#include <Nazara/Math/Frustum.inl>

#endif // NAZARA_FRUSTUM_HPP
