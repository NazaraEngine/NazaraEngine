// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_AXISALIGNEDBOX_HPP
#define NAZARA_AXISALIGNEDBOX_HPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Cube.hpp>
#include <Nazara/Math/Enums.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Math/Vector3.hpp>

template<typename T>
class NzAxisAlignedBox
{
	public:
		NzAxisAlignedBox();
		NzAxisAlignedBox(nzExtend Extend);
		NzAxisAlignedBox(T X, T Y, T Z, T Width, T Height, T Depth);
		NzAxisAlignedBox(const NzCube<T>& Cube);
		NzAxisAlignedBox(const NzVector3<T>& vec1, const NzVector3<T>& vec2);
		template<typename U> explicit NzAxisAlignedBox(const NzAxisAlignedBox<U>& box);
		NzAxisAlignedBox(const NzAxisAlignedBox& box) = default;
		~NzAxisAlignedBox() = default;

		bool Contains(T X, T Y, T Z) const;
		bool Contains(const NzAxisAlignedBox& box) const;
		bool Contains(const NzVector3<T>& vector) const;

		NzAxisAlignedBox& ExtendTo(T X, T Y, T Z);
		NzAxisAlignedBox& ExtendTo(const NzAxisAlignedBox& box);
		NzAxisAlignedBox& ExtendTo(const NzVector3<T>& vector);

		NzVector3<T> GetCorner(nzCorner corner) const;
		NzVector3<T> GetCenter() const;
		NzCube<T> GetCube() const;
		nzExtend GetExtend() const;
		NzVector3<T> GetNegativeVertex(const NzVector3<T>& normal) const;
		NzVector3<T> GetPosition() const;
		NzVector3<T> GetPositiveVertex(const NzVector3<T>& normal) const;
		NzVector3<T> GetSize() const;

		bool Intersect(const NzAxisAlignedBox& box, NzAxisAlignedBox* intersection = nullptr) const;

		bool IsFinite() const;
		bool IsInfinite() const;
		bool IsNull() const;

		NzAxisAlignedBox& MakeInfinite();
		NzAxisAlignedBox& MakeNull();

		NzAxisAlignedBox& Set(nzExtend Extend);
		NzAxisAlignedBox& Set(T X, T Y, T Z, T Width, T Height, T Depth);
		NzAxisAlignedBox& Set(const NzAxisAlignedBox<T>& box);
		NzAxisAlignedBox& Set(const NzCube<T>& Cube);
		NzAxisAlignedBox& Set(const NzVector3<T>& vec1, const NzVector3<T>& vec2);
		template<typename U> NzAxisAlignedBox& Set(const NzAxisAlignedBox<U>& box);

		NzString ToString() const;

		NzAxisAlignedBox& Transform(const NzMatrix4<T>& matrix, bool applyTranslation = true);

		NzAxisAlignedBox operator*(T scalar) const;

		NzAxisAlignedBox& operator*=(T scalar);

		bool operator==(const NzAxisAlignedBox& box) const;
		bool operator!=(const NzAxisAlignedBox& box) const;

		static NzAxisAlignedBox Infinite();
		static NzAxisAlignedBox Lerp(const NzAxisAlignedBox& from, const NzAxisAlignedBox& to, T interpolation);
		static NzAxisAlignedBox Null();

		nzExtend extend;
		NzCube<T> cube;
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzAxisAlignedBox<T>& box);

typedef NzAxisAlignedBox<double> NzAxisAlignedBoxd;
typedef NzAxisAlignedBox<float> NzAxisAlignedBoxf;

#include <Nazara/Math/AxisAlignedBox.inl>

#endif // NAZARA_AXISALIGNEDBOX_HPP
