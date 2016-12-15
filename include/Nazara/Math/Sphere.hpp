// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_SPHERE_HPP
#define NAZARA_SPHERE_HPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Vector3.hpp>

namespace Nz
{
	struct SerializationContext;

	template<typename T> class Box;

	template<typename T>
	class Sphere
	{
		public:
			Sphere() = default;
			Sphere(T X, T Y, T Z, T Radius);
			//Sphere(const Circle<T>& circle);
			Sphere(const Vector3<T>& center, T Radius);
			Sphere(const T sphere[4]);
			template<typename U> explicit Sphere(const Sphere<U>& sphere);
			Sphere(const Sphere& sphere) = default;
			~Sphere() = default;

			bool Contains(T X, T Y, T Z) const;
			bool Contains(const Box<T>& box) const;
			bool Contains(const Vector3<T>& point) const;

			T Distance(T X, T Y, T Z) const;
			T Distance(const Vector3<T>& point) const;

			Sphere& ExtendTo(T X, T Y, T Z);
			Sphere& ExtendTo(const Vector3<T>& point);

			Vector3<T> GetNegativeVertex(const Vector3<T>& normal) const;
			Vector3<T> GetPosition() const;
			Vector3<T> GetPositiveVertex(const Vector3<T>& normal) const;

			bool Intersect(const Box<T>& box) const;
			bool Intersect(const Sphere& sphere) const;

			bool IsValid() const;

			Sphere& MakeUnit();
			Sphere& MakeZero();

			Sphere& Set(T X, T Y, T Z, T Radius);
			//Sphere& Set(const Circle<T>& rect);
			Sphere& Set(const Sphere& sphere);
			Sphere& Set(const Vector3<T>& center, T Radius);
			Sphere& Set(const T sphere[4]);
			template<typename U> Sphere& Set(const Sphere<U>& sphere);

			String ToString() const;

			T& operator[](unsigned int i);
			T operator[](unsigned int i) const;

			Sphere operator*(T scalar) const;
			Sphere& operator=(const Sphere& other) = default;

			Sphere& operator*=(T scalar);

			bool operator==(const Sphere& sphere) const;
			bool operator!=(const Sphere& sphere) const;

			static Sphere Lerp(const Sphere& from, const Sphere& to, T interpolation);
			static Sphere Unit();
			static Sphere Zero();

			T x, y, z, radius;
	};

	typedef Sphere<double> Sphered;
	typedef Sphere<float> Spheref;

	template<typename T> bool Serialize(SerializationContext& context, const Sphere<T>& sphere);
	template<typename T> bool Unserialize(SerializationContext& context, Sphere<T>* sphere);
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nz::Sphere<T>& sphere);

#include <Nazara/Math/Sphere.inl>

#endif // NAZARA_SPHERE_HPP
