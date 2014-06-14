// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_BOUNDINGVOLUME_HPP
#define NAZARA_BOUNDINGVOLUME_HPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Enums.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/OrientedBox.hpp>
#include <Nazara/Math/Vector3.hpp>

template<typename T>
class NzBoundingVolume
{
	public:
		NzBoundingVolume();
		NzBoundingVolume(nzExtend Extend);
		NzBoundingVolume(T X, T Y, T Z, T Width, T Height, T Depth);
		NzBoundingVolume(const NzBox<T>& box);
		NzBoundingVolume(const NzOrientedBox<T>& orientedBox);
		NzBoundingVolume(const NzVector3<T>& vec1, const NzVector3<T>& vec2);
		template<typename U> explicit NzBoundingVolume(const NzBoundingVolume<U>& volume);
		NzBoundingVolume(const NzBoundingVolume& volume) = default;
		~NzBoundingVolume() = default;

		bool IsFinite() const;
		bool IsInfinite() const;
		bool IsNull() const;

		NzBoundingVolume& MakeInfinite();
		NzBoundingVolume& MakeNull();

		NzBoundingVolume& Set(nzExtend Extend);
		NzBoundingVolume& Set(T X, T Y, T Z, T Width, T Height, T Depth);
		NzBoundingVolume& Set(const NzBoundingVolume<T>& volume);
		NzBoundingVolume& Set(const NzBox<T>& box);
		NzBoundingVolume& Set(const NzOrientedBox<T>& orientedBox);
		NzBoundingVolume& Set(const NzVector3<T>& vec1, const NzVector3<T>& vec2);
		template<typename U> NzBoundingVolume& Set(const NzBoundingVolume<U>& volume);

		NzString ToString() const;

		void Update(const NzMatrix4<T>& transformMatrix);

		NzBoundingVolume operator*(T scalar) const;

		NzBoundingVolume& operator*=(T scalar);

		bool operator==(const NzBoundingVolume& volume) const;
		bool operator!=(const NzBoundingVolume& volume) const;

		static NzBoundingVolume Infinite();
		static NzBoundingVolume Lerp(const NzBoundingVolume& from, const NzBoundingVolume& to, T interpolation);
		static NzBoundingVolume Null();

		nzExtend extend;
		NzBox<T> aabb;
		NzOrientedBox<T> obb;
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzBoundingVolume<T>& volume);

typedef NzBoundingVolume<double> NzBoundingVolumed;
typedef NzBoundingVolume<float> NzBoundingVolumef;

#include <Nazara/Math/BoundingVolume.inl>

#endif // NAZARA_BOUNDINGVOLUME_HPP
