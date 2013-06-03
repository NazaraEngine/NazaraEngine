// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ORIENTEDBOX_HPP
#define NAZARA_ORIENTEDBOX_HPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Vector3.hpp>

template<typename T>
class NzOrientedBox
{
	public:
		NzOrientedBox() = default;
		NzOrientedBox(T X, T Y, T Z, T Width, T Height, T Depth);
		NzOrientedBox(const NzBox<T>& box);
		NzOrientedBox(const NzVector3<T>& vec1, const NzVector3<T>& vec2);
		template<typename U> explicit NzOrientedBox(const NzOrientedBox<U>& orientedBox);
		NzOrientedBox(const NzOrientedBox& orientedBox) = default;
		~NzOrientedBox() = default;

		const NzVector3<T>& GetCorner(nzCorner corner) const;

		bool IsValid() const;

		NzOrientedBox& MakeZero();

		NzOrientedBox& Set(T X, T Y, T Z, T Width, T Height, T Depth);
		NzOrientedBox& Set(const NzBox<T>& box);
		NzOrientedBox& Set(const NzOrientedBox& orientedBox);
		NzOrientedBox& Set(const NzVector3<T>& vec1, const NzVector3<T>& vec2);
		template<typename U> NzOrientedBox& Set(const NzOrientedBox<U>& orientedBox);

		NzString ToString() const;

		void Update(const NzMatrix4<T>& transformMatrix);

		operator NzVector3<T>*();
		operator const NzVector3<T>*() const;

		NzVector3<T>& operator()(unsigned int i);
		NzVector3<T> operator()(unsigned int i) const;

		NzOrientedBox operator*(T scalar) const;

		NzOrientedBox& operator*=(T scalar);

		bool operator==(const NzOrientedBox& box) const;
		bool operator!=(const NzOrientedBox& box) const;

		static NzOrientedBox Lerp(const NzOrientedBox& from, const NzOrientedBox& to, T interpolation);
		static NzOrientedBox Zero();

		NzBox<T> localBox;

	private:
		NzVector3<T> m_corners[nzCorner_Max+1]; // Ne peuvent pas être modifiés directement
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzOrientedBox<T>& orientedBox);

typedef NzOrientedBox<double> NzOrientedBoxd;
typedef NzOrientedBox<float> NzOrientedBoxf;

#include <Nazara/Math/OrientedBox.inl>

#endif // NAZARA_ORIENTEDBOX_HPP
