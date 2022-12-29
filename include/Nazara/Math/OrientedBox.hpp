// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATH_ORIENTEDBOX_HPP
#define NAZARA_MATH_ORIENTEDBOX_HPP

#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Enums.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <string>

namespace Nz
{
	struct SerializationContext;

	template<typename T>
	class OrientedBox
	{
		public:
			OrientedBox() = default;
			OrientedBox(const Box<T>& box);
			template<typename U> explicit OrientedBox(const OrientedBox<U>& orientedBox);
			OrientedBox(const OrientedBox&) = default;
			OrientedBox(OrientedBox&&) noexcept = default;
			~OrientedBox() = default;

			const Vector3<T>& GetCorner(BoxCorner corner) const;
			const Vector3<T>* GetCorners() const;

			bool IsValid() const;

			std::string ToString() const;

			void Update(const Matrix4<T>& transformMatrix);
			void Update(const Vector3<T>& transformMatrix);

			Vector3<T>& operator()(unsigned int i);
			const Vector3<T>& operator()(unsigned int i) const;

			OrientedBox& operator=(const OrientedBox&) = default;
			OrientedBox& operator=(OrientedBox&&) noexcept = default;

			bool operator==(const OrientedBox& box) const;
			bool operator!=(const OrientedBox& box) const;

			static OrientedBox Lerp(const OrientedBox& from, const OrientedBox& to, T interpolation);
			static OrientedBox Zero();

			Box<T> localBox;

		private:
			Vector3<T> m_corners[BoxCornerCount];
	};

	using OrientedBoxd = OrientedBox<double>;
	using OrientedBoxf = OrientedBox<float>;

	template<typename T> bool Serialize(SerializationContext& context, const OrientedBox<T>& obb, TypeTag<OrientedBox<T>>);
	template<typename T> bool Unserialize(SerializationContext& context, OrientedBox<T>* obb, TypeTag<OrientedBox<T>>);

	template<typename T> std::ostream& operator<<(std::ostream& out, const Nz::OrientedBox<T>& orientedBox);
}

#include <Nazara/Math/OrientedBox.inl>

#endif // NAZARA_MATH_ORIENTEDBOX_HPP
