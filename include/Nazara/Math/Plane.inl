// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)

namespace Nz
{
	template<typename T>
	Plane<T>::Plane(T normalX, T normalY, T normalZ, T D)
	{
		Set(normalX, normalY, normalZ, D);
	}

	template<typename T>
	Plane<T>::Plane(const T plane[4])
	{
		Set(plane);
	}

	template<typename T>
	Plane<T>::Plane(const Vector3<T>& Normal, T D)
	{
		Set(Normal, D);
	}

	template<typename T>
	Plane<T>::Plane(const Vector3<T>& Normal, const Vector3<T>& point)
	{
		Set(Normal, point);
	}

	template<typename T>
	Plane<T>::Plane(const Vector3<T>& point1, const Vector3<T>& point2, const Vector3<T>& point3)
	{
		Set(point1, point2, point3);
	}

	template<typename T>
	template<typename U>
	Plane<T>::Plane(const Plane<U>& plane)
	{
		Set(plane);
	}

	template<typename T>
	T Plane<T>::Distance(const Vector3<T>& point) const
	{
		return normal.DotProduct(point) - distance; // ax + by + cd - d = 0.
	}

	template<typename T>
	T Plane<T>::Distance(T x, T y, T z) const
	{
		return Distance(Vector3<T>(x, y, z));
	}

	template<typename T>
	Plane<T>& Plane<T>::Set(T normalX, T normalY, T normalZ, T D)
	{
		distance = D;
		normal.Set(normalX, normalY, normalZ);

		return *this;
	}

	template<typename T>
	Plane<T>& Plane<T>::Set(const T plane[4])
	{
		normal.Set(plane[0], plane[1], plane[2]);
		distance = plane[3];

		return *this;
	}

	template<typename T>
	Plane<T>& Plane<T>::Set(const Plane& plane)
	{
		std::memcpy(this, &plane, sizeof(Plane));

		return *this;
	}

	template<typename T>
	Plane<T>& Plane<T>::Set(const Vector3<T>& Normal, T D)
	{
		distance = D;
		normal = Normal;

		return *this;
	}

	template<typename T>
	Plane<T>& Plane<T>::Set(const Vector3<T>& Normal, const Vector3<T>& point)
	{
		normal = Normal;
		distance = -normal.DotProduct(point);

		return *this;
	}

	template<typename T>
	Plane<T>& Plane<T>::Set(const Vector3<T>& point1, const Vector3<T>& point2, const Vector3<T>& point3)
	{
		Vector3<T> edge1 = point2 - point1;
		Vector3<T> edge2 = point3 - point1;
		normal = edge1.CrossProduct(edge2);
		normal.Normalize();

		distance = normal.DotProduct(point3);

		return *this;
	}

	template<typename T>
	template<typename U>
	Plane<T>& Plane<T>::Set(const Plane<U>& plane)
	{
		normal.Set(plane.normal);
		distance = F(plane.distance);

		return *this;
	}

	template<typename T>
	String Plane<T>::ToString() const
	{
		StringStream ss;

		return ss << "Plane(Normal: " << normal.ToString() << "; Distance: " << distance << ')';
	}

	template<typename T>
	bool Plane<T>::operator==(const Plane& plane) const
	{
		return (normal == plane.normal && NumberEquals(distance, plane.distance)) || (normal == -plane.normal && NumberEquals(distance, -plane.distance));
	}

	template<typename T>
	bool Plane<T>::operator!=(const Plane& plane) const
	{
		return !operator==(plane);
	}

	template<typename T>
	Plane<T> Plane<T>::Lerp(const Plane& from, const Plane& to, T interpolation)
	{
		#ifdef NAZARA_DEBUG
		if (interpolation < F(0.0) || interpolation > F(1.0))
		{
			NazaraError("Interpolation must be in range [0..1] (Got " + String::Number(interpolation) + ')');
			return Plane();
		}
		#endif

		Plane plane;
		plane.distance = Lerp(from.distance, to.distance, interpolation);
		plane.normal = Vector3<T>::Lerp(from.normal, to.normal, interpolation);
		plane.normal.Normalize();

		return plane;
	}

	template<typename T>
	Plane<T> Plane<T>::XY()
	{
		return Plane<T>(F(0.0), F(0.0), F(1.0), F(0.0));
	}

	template<typename T>
	Plane<T> Plane<T>::XZ()
	{
		return Plane<T>(F(0.0), F(1.0), F(0.0), F(0.0));
	}

	template<typename T>
	Plane<T> Plane<T>::YZ()
	{
		return Plane<T>(F(1.0), F(0.0), F(0.0), F(0.0));
	}
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nz::Plane<T>& plane)
{
	return out << plane.ToString();
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
