// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATH_FRUSTUM_HPP
#define NAZARA_MATH_FRUSTUM_HPP

#include <Nazara/Math/Angle.hpp>
#include <Nazara/Math/BoundingVolume.hpp>
#include <Nazara/Math/Enums.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/OrientedBox.hpp>
#include <Nazara/Math/Plane.hpp>
#include <Nazara/Math/Sphere.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <array>
#include <string>

namespace Nz
{
	struct SerializationContext;

	template<typename T>
	class Frustum
	{
		public:
			Frustum() = default;
			explicit Frustum(const std::array<Plane<T>, FrustumPlaneCount>& planes);
			template<typename U> explicit Frustum(const Frustum<U>& frustum);
			Frustum(const Frustum& frustum) = default;
			~Frustum() = default;

			Vector3<T> ComputeCorner(BoxCorner corner) const;

			bool Contains(const BoundingVolume<T>& volume) const;
			bool Contains(const Box<T>& box) const;
			bool Contains(const OrientedBox<T>& orientedBox) const;
			bool Contains(const Sphere<T>& sphere) const;
			bool Contains(const Vector3<T>& point) const;
			bool Contains(const Vector3<T>* points, std::size_t pointCount) const;

			const Plane<T>& GetPlane(FrustumPlane plane) const;

			IntersectionSide Intersect(const BoundingVolume<T>& volume) const;
			IntersectionSide Intersect(const Box<T>& box) const;
			IntersectionSide Intersect(const OrientedBox<T>& orientedBox) const;
			IntersectionSide Intersect(const Sphere<T>& sphere) const;
			IntersectionSide Intersect(const Vector3<T>* points, std::size_t pointCount) const;

			std::string ToString() const;

			Frustum& operator=(const Frustum& other) = default;

			static Frustum Build(RadianAngle<T> angle, T ratio, T zNear, T zFar, const Vector3<T>& eye, const Vector3<T>& target, const Vector3<T>& up = Vector3<T>::Up());
			static Frustum Extract(const Matrix4<T>& viewProjMatrix);

			template<typename U>
			friend bool Serialize(SerializationContext& context, const Frustum<U>& frustum, TypeTag<Frustum<U>>);

			template<typename U>
			friend bool Unserialize(SerializationContext& context, Frustum<U>* frustum, TypeTag<Frustum<U>>);

		private:
			std::array<Plane<T>, FrustumPlaneCount> m_planes;
	};

	using Frustumd = Frustum<double>;
	using Frustumf = Frustum<float>;

	template<typename T> std::ostream& operator<<(std::ostream& out, const Nz::Frustum<T>& frustum);
}

#include <Nazara/Math/Frustum.inl>

#endif // NAZARA_MATH_FRUSTUM_HPP
