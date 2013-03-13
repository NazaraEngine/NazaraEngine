// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ORIENTEDCUBE_HPP
#define NAZARA_ORIENTEDCUBE_HPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Cube.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Vector3.hpp>

template<typename T>
class NzOrientedCube
{
	public:
		NzOrientedCube() = default;
		NzOrientedCube(T X, T Y, T Z, T Width, T Height, T Depth);
		NzOrientedCube(const NzCube<T>& cube);
		NzOrientedCube(const NzVector3<T>& vec1, const NzVector3<T>& vec2);
		template<typename U> explicit NzOrientedCube(const NzOrientedCube<U>& orientedCube);
		NzOrientedCube(const NzOrientedCube& orientedCube) = default;
		~NzOrientedCube() = default;

		const NzVector3<T>& GetCorner(nzCorner corner) const;

		bool IsValid() const;

		NzOrientedCube& MakeZero();

		NzOrientedCube& Set(T X, T Y, T Z, T Width, T Height, T Depth);
		NzOrientedCube& Set(const NzCube<T>& cube);
		NzOrientedCube& Set(const NzOrientedCube& orientedCube);
		NzOrientedCube& Set(const NzVector3<T>& vec1, const NzVector3<T>& vec2);
		template<typename U> NzOrientedCube& Set(const NzOrientedCube<U>& orientedCube);

		NzString ToString() const;

		void Update(const NzMatrix4<T>& transformMatrix);

		operator NzVector3<T>*();
		operator const NzVector3<T>*() const;

		NzVector3<T>& operator()(unsigned int i);
		NzVector3<T> operator()(unsigned int i) const;

		NzOrientedCube operator*(T scalar) const;

		NzOrientedCube& operator*=(T scalar);

		bool operator==(const NzOrientedCube& cube) const;
		bool operator!=(const NzOrientedCube& cube) const;

		static NzOrientedCube Lerp(const NzOrientedCube& from, const NzOrientedCube& to, T interpolation);
		static NzOrientedCube Zero();

		NzCube<T> localCube;

	private:
		NzVector3<T> m_corners[nzCorner_Max+1]; // Ne peuvent pas être modifiés directement
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzOrientedCube<T>& orientedCube);

typedef NzOrientedCube<double> NzOrientedCubed;
typedef NzOrientedCube<float> NzOrientedCubef;

#include <Nazara/Math/OrientedCube.inl>

#endif // NAZARA_ORIENTEDCUBE_HPP
