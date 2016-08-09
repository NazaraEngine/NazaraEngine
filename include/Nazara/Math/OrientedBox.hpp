// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ORIENTEDBOX_HPP
#define NAZARA_ORIENTEDBOX_HPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Vector3.hpp>

namespace Nz
{
	struct SerializationContext;

	template<typename T>
	class OrientedBox
	{
		public:
			OrientedBox() = default;
			OrientedBox(T X, T Y, T Z, T Width, T Height, T Depth);
			OrientedBox(const Box<T>& box);
			OrientedBox(const Vector3<T>& vec1, const Vector3<T>& vec2);
			template<typename U> explicit OrientedBox(const OrientedBox<U>& orientedBox);
			OrientedBox(const OrientedBox& orientedBox) = default;
			~OrientedBox() = default;

			const Vector3<T>& GetCorner(BoxCorner corner) const;

			bool IsValid() const;

			OrientedBox& MakeZero();

			OrientedBox& Set(T X, T Y, T Z, T Width, T Height, T Depth);
			OrientedBox& Set(const Box<T>& box);
			OrientedBox& Set(const OrientedBox& orientedBox);
			OrientedBox& Set(const Vector3<T>& vec1, const Vector3<T>& vec2);
			template<typename U> OrientedBox& Set(const OrientedBox<U>& orientedBox);

			String ToString() const;

			void Update(const Matrix4<T>& transformMatrix);
			void Update(const Vector3<T>& transformMatrix);

			operator Vector3<T>* ();
			operator const Vector3<T>* () const;

			Vector3<T>& operator()(unsigned int i);
			Vector3<T> operator()(unsigned int i) const;

			OrientedBox operator*(T scalar) const;
			OrientedBox& operator=(const OrientedBox& other) = default;

			OrientedBox& operator*=(T scalar);

			bool operator==(const OrientedBox& box) const;
			bool operator!=(const OrientedBox& box) const;

			static OrientedBox Lerp(const OrientedBox& from, const OrientedBox& to, T interpolation);
			static OrientedBox Zero();

			Box<T> localBox;

		private:
			Vector3<T> m_corners[BoxCorner_Max+1]; // Ne peuvent pas être modifiés directement
	};

	typedef OrientedBox<double> OrientedBoxd;
	typedef OrientedBox<float> OrientedBoxf;

	template<typename T> bool Serialize(SerializationContext& context, const OrientedBox<T>& obb);
	template<typename T> bool Unserialize(SerializationContext& context, OrientedBox<T>* obb);
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nz::OrientedBox<T>& orientedBox);

#include <Nazara/Math/OrientedBox.inl>

#endif // NAZARA_ORIENTEDBOX_HPP
