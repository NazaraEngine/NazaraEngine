// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATH_PLANE_HPP
#define NAZARA_MATH_PLANE_HPP

#include <Nazara/Math/Vector3.hpp>
#include <string>

namespace Nz
{
	struct SerializationContext;

	template<typename T>
	class Plane
	{
		// Note: this class follows the ax + by + cz + d = 0 equation, which means d is the distance from origin

		public:
			constexpr Plane() = default;
			constexpr Plane(T normalX, T normalY, T normalZ, T Distance);
			constexpr Plane(const T plane[4]);
			constexpr Plane(const Vector3<T>& Normal, T Distance);
			constexpr Plane(const Vector3<T>& Normal, const Vector3<T>& point);
			Plane(const Vector3<T>& point1, const Vector3<T>& point2, const Vector3<T>& point3);
			template<typename U> constexpr explicit Plane(const Plane<U>& plane);
			constexpr Plane(const Plane& plane) = default;
			~Plane() = default;

			constexpr bool ApproxEqual(const Plane& plane, T maxDifference = std::numeric_limits<T>::epsilon()) const;

			Plane& Normalize(T* length = nullptr);

			constexpr T SignedDistance(const Vector3<T>& point) const;

			std::string ToString() const;

			constexpr Plane& operator=(const Plane& other) = default;

			constexpr bool operator==(const Plane& plane) const;
			constexpr bool operator!=(const Plane& plane) const;

			static constexpr bool ApproxEqual(const Plane& lhs, const Plane& rhs, T maxDifference = std::numeric_limits<T>::epsilon());
			static constexpr Vector3<T> Intersect(const Plane& p0, const Plane& p1, const Plane& p2);
			static constexpr Plane Lerp(const Plane& from, const Plane& to, T interpolation);
			static Plane Normalize(const Plane& plane, T* length = nullptr);
			static constexpr Plane XY();
			static constexpr Plane XZ();
			static constexpr Plane YZ();

			Vector3<T> normal;
			T distance;
	};

	using Planed = Plane<double>;
	using Planef = Plane<float>;

	template<typename T> bool Serialize(SerializationContext& context, const Plane<T>& plane, TypeTag<Plane<T>>);
	template<typename T> bool Unserialize(SerializationContext& context, Plane<T>* plane, TypeTag<Plane<T>>);

	template<typename T> std::ostream& operator<<(std::ostream& out, const Nz::Plane<T>& plane);
}

#include <Nazara/Math/Plane.inl>

#endif // NAZARA_MATH_PLANE_HPP
