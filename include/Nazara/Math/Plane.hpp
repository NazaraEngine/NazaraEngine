// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_PLANE_HPP
#define NAZARA_PLANE_HPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Vector3.hpp>

namespace Nz
{
	struct SerializationContext;

	template<typename T>
	class Plane
	{
		public:
			Plane() = default;
			Plane(T normalX, T normalY, T normalZ, T Distance);
			Plane(const T plane[4]);
			Plane(const Vector3<T>& Normal, T Distance);
			Plane(const Vector3<T>& Normal, const Vector3<T>& point);
			Plane(const Vector3<T>& point1, const Vector3<T>& point2, const Vector3<T>& point3);
			template<typename U> explicit Plane(const Plane<U>& plane);
			Plane(const Plane& plane) = default;
			~Plane() = default;

			T Distance(T x, T y, T z) const;
			T Distance(const Vector3<T>& point) const;

			Plane& MakeXY();
			Plane& MakeXZ();
			Plane& MakeYZ();

			Plane& Set(T normalX, T normalY, T normalZ, T Distance);
			Plane& Set(const T plane[4]);
			Plane& Set(const Plane& plane);
			Plane& Set(const Vector3<T>& Normal, T Distance);
			Plane& Set(const Vector3<T>& Normal, const Vector3<T>& point);
			Plane& Set(const Vector3<T>& point1, const Vector3<T>& point2, const Vector3<T>& point3);
			template<typename U> Plane& Set(const Plane<U>& plane);

			String ToString() const;

			Plane& operator=(const Plane& other) = default;

			bool operator==(const Plane& plane) const;
			bool operator!=(const Plane& plane) const;

			static Plane Lerp(const Plane& from, const Plane& to, T interpolation);
			static Plane XY();
			static Plane XZ();
			static Plane YZ();

			Vector3<T> normal;
			T distance;
	};

	typedef Plane<double> Planed;
	typedef Plane<float> Planef;

	template<typename T> bool Serialize(SerializationContext& context, const Plane<T>& plane, TypeTag<Plane<T>>);
	template<typename T> bool Unserialize(SerializationContext& context, Plane<T>* plane, TypeTag<Plane<T>>);
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nz::Plane<T>& plane);

#include <Nazara/Math/Plane.inl>

#endif // NAZARA_PLANE_HPP
