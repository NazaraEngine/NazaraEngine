// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_BOX_HPP
#define NAZARA_BOX_HPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Enums.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Sphere.hpp>
#include <Nazara/Math/Vector3.hpp>

template<typename T>
class NzBox
{
	public:
		NzBox() = default;
		NzBox(T Width, T Height, T Depth);
		NzBox(T X, T Y, T Z, T Width, T Height, T Depth);
		NzBox(const T box[6]);
		NzBox(const NzRect<T>& rect);
		NzBox(const NzVector3<T>& lengths);
		NzBox(const NzVector3<T>& vec1, const NzVector3<T>& vec2);
		template<typename U> explicit NzBox(const NzBox<U>& box);
		NzBox(const NzBox& box) = default;
		~NzBox() = default;

		bool Contains(T X, T Y, T Z) const;
		bool Contains(const NzBox& box) const;
		bool Contains(const NzVector3<T>& point) const;

		NzBox& ExtendTo(T X, T Y, T Z);
		NzBox& ExtendTo(const NzBox& box);
		NzBox& ExtendTo(const NzVector3<T>& point);

		NzSphere<T> GetBoundingSphere() const;
		NzVector3<T> GetCorner(nzCorner corner) const;
		NzVector3<T> GetCenter() const;
		NzVector3<T> GetLengths() const;
		NzVector3<T> GetMaximum() const;
		NzVector3<T> GetMinimum() const;
		NzVector3<T> GetNegativeVertex(const NzVector3<T>& normal) const;
		NzVector3<T> GetPosition() const;
		NzVector3<T> GetPositiveVertex(const NzVector3<T>& normal) const;
		T GetRadius() const;
		T GetSquaredRadius() const;

		bool Intersect(const NzBox& box, NzBox* intersection = nullptr) const;

		bool IsValid() const;

		NzBox& MakeZero();

		NzBox& Set(T Width, T Height, T Depth);
		NzBox& Set(T X, T Y, T Z, T Width, T Height, T Depth);
		NzBox& Set(const T box[6]);
		NzBox& Set(const NzBox& box);
		NzBox& Set(const NzRect<T>& rect);
		NzBox& Set(const NzVector3<T>& lengths);
		NzBox& Set(const NzVector3<T>& vec1, const NzVector3<T>& vec2);
		template<typename U> NzBox& Set(const NzBox<U>& box);

		NzString ToString() const;

		NzBox& Transform(const NzMatrix4<T>& matrix, bool applyTranslation = true);
		NzBox& Translate(const NzVector3<T>& translation);

		T& operator[](unsigned int i);
		T operator[](unsigned int i) const;

		NzBox operator*(T scalar) const;
		NzBox operator*(const NzVector3<T>& vec) const;

		NzBox& operator*=(T scalar);
		NzBox& operator*=(const NzVector3<T>& vec);

		bool operator==(const NzBox& box) const;
		bool operator!=(const NzBox& box) const;

		static NzBox Lerp(const NzBox& from, const NzBox& to, T interpolation);
		static NzBox Zero();

		T x, y, z, width, height, depth;
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzBox<T>& box);

typedef NzBox<double> NzBoxd;
typedef NzBox<float> NzBoxf;
typedef NzBox<int> NzBoxi;
typedef NzBox<unsigned int> NzBoxui;

#include <Nazara/Math/Box.inl>

#endif // NAZARA_BOX_HPP
