// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATH_BOUNDINGVOLUME_HPP
#define NAZARA_MATH_BOUNDINGVOLUME_HPP

#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Enums.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/OrientedBox.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <string>

namespace Nz
{
	struct SerializationContext;

	template<typename T>
	class BoundingVolume
	{
		public:
			BoundingVolume();
			BoundingVolume(Extend Extend);
			BoundingVolume(const Box<T>& box);
			BoundingVolume(const OrientedBox<T>& orientedBox);
			template<typename U> explicit BoundingVolume(const BoundingVolume<U>& volume);
			BoundingVolume(const BoundingVolume& volume) = default;
			~BoundingVolume() = default;

			BoundingVolume& ExtendTo(const BoundingVolume& volume);

			bool Intersect(const Box<T>& box) const;

			bool IsFinite() const;
			bool IsInfinite() const;
			bool IsNull() const;

			std::string ToString() const;

			void Update(const Matrix4<T>& transformMatrix);
			void Update(const Vector3<T>& translation);

			BoundingVolume operator*(T scalar) const;
			BoundingVolume& operator=(const BoundingVolume& other) = default;

			BoundingVolume& operator*=(T scalar);

			bool operator==(const BoundingVolume& volume) const;
			bool operator!=(const BoundingVolume& volume) const;

			static BoundingVolume Infinite();
			static BoundingVolume Lerp(const BoundingVolume& from, const BoundingVolume& to, T interpolation);
			static BoundingVolume Null();

			Extend extend;
			Box<T> aabb;
			OrientedBox<T> obb;
	};

	using BoundingVolumed = BoundingVolume<double>;
	using BoundingVolumef = BoundingVolume<float>;

	template<typename T> bool Serialize(SerializationContext& context, const BoundingVolume<T>& boundingVolume, TypeTag<BoundingVolume<T>>);
	template<typename T> bool Unserialize(SerializationContext& context, BoundingVolume<T>* boundingVolume, TypeTag<BoundingVolume<T>>);
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nz::BoundingVolume<T>& volume);

#include <Nazara/Math/BoundingVolume.inl>

#endif // NAZARA_MATH_BOUNDINGVOLUME_HPP
