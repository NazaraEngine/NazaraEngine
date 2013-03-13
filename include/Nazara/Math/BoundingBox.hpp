// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_BOUNDINGBOX_HPP
#define NAZARA_BOUNDINGBOX_HPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Cube.hpp>
#include <Nazara/Math/Enums.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/OrientedCube.hpp>
#include <Nazara/Math/Vector3.hpp>

template<typename T>
class NzBoundingBox
{
	public:
		NzBoundingBox();
		NzBoundingBox(nzExtend Extend);
		NzBoundingBox(T X, T Y, T Z, T Width, T Height, T Depth);
		NzBoundingBox(const NzCube<T>& Cube);
		NzBoundingBox(const NzVector3<T>& vec1, const NzVector3<T>& vec2);
		template<typename U> explicit NzBoundingBox(const NzBoundingBox<U>& box);
		NzBoundingBox(const NzBoundingBox& box) = default;
		~NzBoundingBox() = default;

		bool IsFinite() const;
		bool IsInfinite() const;
		bool IsNull() const;

		NzBoundingBox& MakeInfinite();
		NzBoundingBox& MakeNull();

		NzBoundingBox& Set(nzExtend Extend);
		NzBoundingBox& Set(T X, T Y, T Z, T Width, T Height, T Depth);
		NzBoundingBox& Set(const NzBoundingBox<T>& box);
		NzBoundingBox& Set(const NzCube<T>& Cube);
		NzBoundingBox& Set(const NzVector3<T>& vec1, const NzVector3<T>& vec2);
		template<typename U> NzBoundingBox& Set(const NzBoundingBox<U>& box);

		NzString ToString() const;

		void Update(const NzMatrix4<T>& transformMatrix);

		NzBoundingBox operator*(T scalar) const;

		NzBoundingBox& operator*=(T scalar);

		bool operator==(const NzBoundingBox& box) const;
		bool operator!=(const NzBoundingBox& box) const;

		static NzBoundingBox Infinite();
		static NzBoundingBox Lerp(const NzBoundingBox& from, const NzBoundingBox& to, T interpolation);
		static NzBoundingBox Null();

		nzExtend extend;
		NzCube<T> aabb;
		NzOrientedCube<T> obb;
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzBoundingBox<T>& box);

typedef NzBoundingBox<double> NzBoundingBoxd;
typedef NzBoundingBox<float> NzBoundingBoxf;

#include <Nazara/Math/BoundingBox.inl>

#endif // NAZARA_BOUNDINGBOX_HPP
