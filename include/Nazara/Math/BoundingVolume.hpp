// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATH_BOUNDINGVOLUME_HPP
#define NAZARA_MATH_BOUNDINGVOLUME_HPP

#include <Nazara/Core/Serialization.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Enums.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/OrientedBox.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <string>

namespace Nz
{
	template<typename T>
	class BoundingVolume
	{
		public:
			constexpr BoundingVolume();
			constexpr explicit BoundingVolume(Extent Extend);
			constexpr explicit BoundingVolume(const Box<T>& box);
			constexpr explicit BoundingVolume(const OrientedBox<T>& orientedBox);
			template<typename U> constexpr explicit BoundingVolume(const BoundingVolume<U>& volume);
			constexpr BoundingVolume(const BoundingVolume&) = default;
			constexpr BoundingVolume(BoundingVolume&&) = default;
			~BoundingVolume() = default;

			constexpr bool ApproxEqual(const BoundingVolume& volume, T maxDifference = std::numeric_limits<T>::epsilon()) const;

			constexpr BoundingVolume& ExtendTo(const BoundingVolume& volume);

			constexpr bool Intersect(const Box<T>& box) const;

			constexpr bool IsFinite() const;
			constexpr bool IsInfinite() const;
			constexpr bool IsNull() const;

			std::string ToString() const;

			constexpr void Update(const Matrix4<T>& transformMatrix);
			constexpr void Update(const Vector3<T>& translation);

			BoundingVolume& operator=(const BoundingVolume&) = default;
			BoundingVolume& operator=(BoundingVolume&&) = default;

			constexpr BoundingVolume operator*(T scalar) const;

			constexpr BoundingVolume& operator*=(T scalar);

			constexpr bool operator==(const BoundingVolume& volume) const;
			constexpr bool operator!=(const BoundingVolume& volume) const;

			static constexpr bool ApproxEqual(const BoundingVolume& lhs, const BoundingVolume& rhs, T maxDifference = std::numeric_limits<T>::epsilon());
			static constexpr BoundingVolume Infinite();
			static constexpr BoundingVolume Lerp(const BoundingVolume& from, const BoundingVolume& to, T interpolation);
			static constexpr BoundingVolume Null();

			Extent extent;
			Box<T> aabb;
			OrientedBox<T> obb;
	};

	using BoundingVolumed = BoundingVolume<double>;
	using BoundingVolumef = BoundingVolume<float>;

	template<typename T> bool Serialize(SerializationContext& context, const BoundingVolume<T>& boundingVolume, TypeTag<BoundingVolume<T>>);
	template<typename T> bool Unserialize(SerializationContext& context, BoundingVolume<T>* boundingVolume, TypeTag<BoundingVolume<T>>);

	template<typename T> std::ostream& operator<<(std::ostream& out, const Nz::BoundingVolume<T>& volume);
}

#include <Nazara/Math/BoundingVolume.inl>

#endif // NAZARA_MATH_BOUNDINGVOLUME_HPP
