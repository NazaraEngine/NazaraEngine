// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_CUBE_HPP
#define NAZARA_CUBE_HPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Rect.hpp>
#include <Nazara/Math/Vector3.hpp>

template<typename T>
class NzCube
{
	public:
		NzCube();
		NzCube(T X, T Y, T Z, T Width, T Height, T Depth);
		NzCube(const T cube[6]);
		NzCube(const NzRect<T>& rect);
		NzCube(const NzVector3<T>& vec1, const NzVector3<T>& vec2);
		template<typename U> explicit NzCube(const NzCube<U>& rect);
		NzCube(const NzCube& rect) = default;
		~NzCube() = default;

		bool Contains(T X, T Y, T Z) const;
		bool Contains(const NzVector3<T>& point) const;
		bool Contains(const NzCube& rect) const;

		void ExtendTo(const NzVector3<T>& point);
		void ExtendTo(const NzCube& rect);

		NzVector3<T> GetCenter() const;

		bool Intersect(const NzCube& rect, NzCube* intersection = nullptr) const;

		bool IsValid() const;

		void Set(T X, T Y, T Z, T Width, T Height, T Depth);
		void Set(const T rect[6]);
		void Set(const NzRect<T>& rect);
		void Set(const NzVector3<T>& vec1, const NzVector3<T>& vec2);
		template<typename U> void Set(const NzCube<U>& rect);

		NzString ToString() const;

		operator NzString() const;

		T& operator[](unsigned int i);
		T operator[](unsigned int i) const;

		T x, y, z, width, height, depth;
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzCube<T>& vec);

typedef NzCube<double> NzCubed;
typedef NzCube<float> NzCubef;
typedef NzCube<int> NzCubei;
typedef NzCube<unsigned int> NzCubeui;

#include <Nazara/Math/Cube.inl>

#endif // NAZARA_CUBE_HPP
