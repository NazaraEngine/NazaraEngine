// Copyright (C) 2012 Rémi Bèges - Jérôme Leclercq
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
		NzQuaternion();
		NzQuaternion(T W, T X, T Y, T Z);
		NzQuaternion(T quat[4]);
		NzQuaternion(T angle, const NzVector3<T>& axis);
		NzQuaternion(const NzEulerAngles<T>& angles);
		//NzQuaternion(const NzMatrix3<T>& mat);
		template<typename U> explicit NzQuaternion(const NzQuaternion<U>& quat);
		NzQuaternion(const NzQuaternion& quat) = default;
		~NzQuaternion() = default;

		T DotProduct(const NzQuaternion& vec) const;

		NzQuaternion GetConjugate() const;
		NzQuaternion GetInverse() const;
		NzQuaternion GetNormal() const;

		void Inverse();

		void MakeIdentity();
		void MakeZero();

		T Magnitude() const;

		T Normalize();

		void Set(T W, T X, T Y, T Z);
		void Set(T quat[4]);
		void Set(T angle, const NzVector3<T>& normalizedAxis);
		void Set(const NzEulerAngles<T>& angles);
		//void Set(const NzMatrix3<T>& mat);
		void Set(const NzQuaternion& quat);
		template<typename U> void Set(const NzQuaternion<U>& quat);

		T SquaredMagnitude() const;

		NzEulerAngles<T> ToEulerAngles() const;
		//NzMatrix3<T> ToRotationMatrix() const;
		NzString ToString() const;

		operator NzString() const;

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
		bool operator<(const NzQuaternion& quat) const;
		bool operator<=(const NzQuaternion& quat) const;
		bool operator>(const NzQuaternion& quat) const;
		bool operator>=(const NzQuaternion& quat) const;

		static NzQuaternion Identity();
		static NzQuaternion Slerp(const NzQuaternion& quatA, const NzQuaternion& quatB, T interp);
		static NzQuaternion Zero();

		T w, x, y, z;
};

template<typename T> std::ostream& operator<<(std::ostream& out, const NzQuaternion<T>& quat);

typedef NzQuaternion<double> NzQuaterniond;
typedef NzQuaternion<float> NzQuaternionf;

#include <Nazara/Math/Quaternion.inl>

#endif // NAZARA_QUATERNION_HPP
