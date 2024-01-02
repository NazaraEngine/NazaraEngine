// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
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
#include <NazaraUtils/EnumArray.hpp>
#include <array>
#include <string>

namespace Nz
{
	struct SerializationContext;

	template<typename T>
	class Frustum
	{
		public:
			constexpr Frustum() = default;
			constexpr explicit Frustum(const EnumArray<FrustumPlane, Plane<T>>& planes);
			template<typename U> constexpr explicit Frustum(const Frustum<U>& frustum);
			constexpr Frustum(const Frustum&) = default;
			constexpr Frustum(Frustum&&) = default;
			~Frustum() = default;

			constexpr bool ApproxEqual(const Frustum& frustum, T maxDifference = std::numeric_limits<T>::epsilon()) const;

			constexpr Vector3<T> ComputeCorner(BoxCorner corner) const;
			constexpr EnumArray<BoxCorner, Vector3<T>> ComputeCorners() const;

			constexpr bool Contains(const BoundingVolume<T>& volume) const;
			constexpr bool Contains(const Box<T>& box) const;
			constexpr bool Contains(const OrientedBox<T>& orientedBox) const;
			constexpr bool Contains(const Sphere<T>& sphere) const;
			constexpr bool Contains(const Vector3<T>& point) const;
			constexpr bool Contains(const Vector3<T>* points, std::size_t pointCount) const;

			constexpr Box<T> GetAABB() const;
			constexpr const Plane<T>& GetPlane(FrustumPlane plane) const;
			constexpr const EnumArray<FrustumPlane, Plane<T>>& GetPlanes() const;

			constexpr IntersectionSide Intersect(const BoundingVolume<T>& volume) const;
			constexpr IntersectionSide Intersect(const Box<T>& box) const;
			constexpr IntersectionSide Intersect(const OrientedBox<T>& orientedBox) const;
			constexpr IntersectionSide Intersect(const Sphere<T>& sphere) const;
			constexpr IntersectionSide Intersect(const Vector3<T>* points, std::size_t pointCount) const;

			constexpr Frustum<T> Reduce(T nearFactor, T farFactor) const;

			template<typename F> constexpr void Split(std::initializer_list<T> splitFactors, F&& callback) const;
			template<typename F> constexpr void Split(const T* splitFactors, std::size_t factorCount, F&& callback) const;

			std::string ToString() const;

			constexpr Frustum& operator=(const Frustum&) = default;
			constexpr Frustum& operator=(Frustum&&) = default;

			constexpr bool operator==(const Frustum& angles) const;
			constexpr bool operator!=(const Frustum& angles) const;

			static constexpr bool ApproxEqual(const Frustum& lhs, const Frustum& rhs, T maxDifference = std::numeric_limits<T>::epsilon());
			static Frustum Build(RadianAngle<T> angle, T ratio, T zNear, T zFar, const Vector3<T>& eye, const Vector3<T>& target, const Vector3<T>& up = Vector3<T>::Up());
			static Frustum Extract(const Matrix4<T>& viewProjMatrix);

			template<typename U> friend bool Serialize(SerializationContext& context, const Frustum<U>& frustum, TypeTag<Frustum<U>>);
			template<typename U> friend bool Unserialize(SerializationContext& context, Frustum<U>* frustum, TypeTag<Frustum<U>>);

		private:
			EnumArray<FrustumPlane, Plane<T>> m_planes;
	};

	using Frustumd = Frustum<double>;
	using Frustumf = Frustum<float>;

	template<typename T> std::ostream& operator<<(std::ostream& out, const Nz::Frustum<T>& frustum);
}

#include <Nazara/Math/Frustum.inl>

#endif // NAZARA_MATH_FRUSTUM_HPP
