// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATH_ORIENTEDBOX_HPP
#define NAZARA_MATH_ORIENTEDBOX_HPP

#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Enums.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <NazaraUtils/EnumArray.hpp>
#include <string>

namespace Nz
{
	struct SerializationContext;

	template<typename T>
	class OrientedBox
	{
		public:
			constexpr OrientedBox() = default;
			constexpr OrientedBox(const Box<T>& box);
			template<typename U> constexpr explicit OrientedBox(const OrientedBox<U>& orientedBox);
			constexpr OrientedBox(const OrientedBox&) = default;
			constexpr OrientedBox(OrientedBox&&) noexcept = default;
			~OrientedBox() = default;

			constexpr bool ApproxEqual(const OrientedBox& obb, T maxDifference = std::numeric_limits<T>::epsilon()) const;

			constexpr const Vector3<T>& GetCorner(BoxCorner corner) const;
			constexpr const EnumArray<BoxCorner, Vector3<T>>& GetCorners() const;

			constexpr bool IsValid() const;

			std::string ToString() const;

			constexpr void Update(const Matrix4<T>& transformMatrix);
			constexpr void Update(const Vector3<T>& transformMatrix);

			constexpr Vector3<T>& operator()(unsigned int i);
			constexpr const Vector3<T>& operator()(unsigned int i) const;

			constexpr OrientedBox& operator=(const OrientedBox&) = default;
			constexpr OrientedBox& operator=(OrientedBox&&) noexcept = default;

			constexpr bool operator==(const OrientedBox& box) const;
			constexpr bool operator!=(const OrientedBox& box) const;

			static constexpr bool ApproxEqual(const OrientedBox& lhs, const OrientedBox& rhs, T maxDifference = std::numeric_limits<T>::epsilon());
			static constexpr OrientedBox Lerp(const OrientedBox& from, const OrientedBox& to, T interpolation);
			static constexpr OrientedBox Zero();

			template<typename U> friend bool Serialize(SerializationContext& context, const OrientedBox<U>& obb, TypeTag<OrientedBox<U>>);
			template<typename U> friend bool Unserialize(SerializationContext& context, OrientedBox<U>* obb, TypeTag<OrientedBox<U>>);

			Box<T> localBox;

		private:
			EnumArray<BoxCorner, Vector3<T>> m_corners;
	};

	using OrientedBoxd = OrientedBox<double>;
	using OrientedBoxf = OrientedBox<float>;

	template<typename T> std::ostream& operator<<(std::ostream& out, const Nz::OrientedBox<T>& orientedBox);
}

#include <Nazara/Math/OrientedBox.inl>

#endif // NAZARA_MATH_ORIENTEDBOX_HPP
