// Copyright (C) 2014 Rémi Bèges - Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_QUATERNION_HPP
#define NAZARA_QUATERNION_HPP

#include <Nazara/Core/String.hpp>

template<typename T> class NzEulerAngles;
template<typename T> class NzVector3;

template<typename T> class NzQuaternion
{
	public:
		NzQuaternion() = default;
		NzQuaternion(T W, T X, T Y, T Z);
		NzQuaternion(const T quat[4]);
		NzQuaternion(T angle, const NzVector3<T>& axis);
		NzQuaternion(const NzEulerAngles<T>& angles);
		//NzQuaternion(const NzMatrix3<T>& mat);
		template<typename U> explicit NzQuaternion(const NzQuaternion<U>& quat);
		NzQuaternion(const NzQuaternion& quat) = default;
		~NzQuaternion() = default;

		NzQuaternion& ComputeW();
		NzQuaternion& Conjugate();

		T DotProduct(const NzQuaternion& vec) const;

		NzQuaternion GetConjugate() const;
		NzQuaternion GetInverse() const;
		NzQuaternion GetNormal(T* length = nullptr) const;

		NzQuaternion& Inverse();

		NzQuaternion& MakeIdentity();
		NzQuaternion& MakeRotationBetween(const NzVector3<T>& from, const NzVector3<T>& to);
		NzQuaternion& MakeZero();

		T Magnitude() const;

		NzQuaternion& Normalize(T* length = nullptr);

		NzQuaternion& Set(T W, T X, T Y, T Z);
		NzQuaternion& Set(const T quat[4]);
		NzQuaternion& Set(T angle, const NzVector3<T>& normalizedAxis);
		NzQuaternion& Set(const NzEulerAngles<T>& angles);
		//NzQuaternion& Set(const NzMatrix3<T>& mat);
		NzQuaternion& Set(const NzQuaternion& quat);
		template<typename U> NzQuaternion& Set(const NzQuaternion<U>& quat);

		T SquaredMagnitude() const;

		NzEulerAngles<T> ToEulerAngles() const;
		//NzMatrix3<T> ToRotationMatrix() const;
		NzString ToString() const;

		NzQuaternion& operator=(const NzQuaternion& quat);

		NzQuaternion operator+(const NzQuaternion& quat) const;
		NzQuaternion operator*(const NzQuaternion& quat) const;
		NzVector3<T> operator*(const NzVector3<T>& vec) const;
		NzQuaternion operator*(T scale) const;
		NzQuaternion operator/(const NzQuaternion& quat) const;

		NzQuaternion& operator+=(const NzQuaternion& quat);
		NzQuaternion& operator*=(const NzQuaternion& quat);
		NzQuaternion& operator*=(T scale);
		NzQuaternion& operator/=(const NzQuaternion& quat);

		bool operator==(const NzQuaternion& quat) const;
		bool operator!=(const NzQuaternion& quat) const;

		static NzQuaternion Identity();
		static NzQuaternion Lerp(const NzQuaternion& from, const NzQuaternion& to, T interpolation);
		static NzQuaternion RotationBetween(const NzVector3<T>& from, const NzVector3<T>& to);
		static NzQuaternion Slerp(const NzQuaternion& from, const NzQuaternion& to, T interpolation);
		static NzQuaternion Zero();

		T w, x, y, z;
};

template<typename T> std::ostream& operator<<(std::ostream& out, const NzQuaternion<T>& quat);

typedef NzQuaternion<double> NzQuaterniond;
typedef NzQuaternion<float> NzQuaternionf;

#include <Nazara/Math/Quaternion.inl>

#endif // NAZARA_QUATERNION_HPP
