// Copyright (C) 2022 Rémi Bèges - Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATH_QUATERNION_HPP
#define NAZARA_MATH_QUATERNION_HPP

#include <Nazara/Math/Angle.hpp>
#include <string>

namespace Nz
{
	struct SerializationContext;

	template<typename T> class EulerAngles;
	template<typename T> class Vector3;

	template<typename T> class Quaternion
	{
		public:
			Quaternion() = default;
			Quaternion(T W, T X, T Y, T Z);
			template<AngleUnit Unit> Quaternion(const Angle<Unit, T>& angle);
			Quaternion(const EulerAngles<T>& angles);
			Quaternion(RadianAngle<T> angle, const Vector3<T>& axis);
			Quaternion(const T quat[4]);
			//Quaternion(const Matrix3<T>& mat);
			template<typename U> explicit Quaternion(const Quaternion<U>& quat);
			Quaternion(const Quaternion& quat) = default;
			~Quaternion() = default;

			Quaternion& ComputeW();
			Quaternion& Conjugate();

			T DotProduct(const Quaternion& vec) const;

			Quaternion GetConjugate() const;
			Quaternion GetInverse() const;
			Quaternion GetNormal(T* length = nullptr) const;

			Quaternion& Inverse();

			Quaternion& MakeIdentity();
			Quaternion& MakeRotationBetween(const Vector3<T>& from, const Vector3<T>& to);
			Quaternion& MakeZero();

			T Magnitude() const;

			Quaternion& Normalize(T* length = nullptr);

			Quaternion& Set(T W, T X, T Y, T Z);
			template<AngleUnit Unit> Quaternion& Set(const Angle<Unit, T>& angle);
			Quaternion& Set(const EulerAngles<T>& angles);
			Quaternion& Set(RadianAngle<T> angle, const Vector3<T>& normalizedAxis);
			Quaternion& Set(const T quat[4]);
			//Quaternion& Set(const Matrix3<T>& mat);
			template<typename U> Quaternion& Set(const Quaternion<U>& quat);

			T SquaredMagnitude() const;

			RadianAngle<T> To2DAngle() const;
			EulerAngles<T> ToEulerAngles() const;
			//Matrix3<T> ToRotationMatrix() const;
			std::string ToString() const;

			Quaternion& operator=(const Quaternion& quat) = default;

			Quaternion operator+(const Quaternion& quat) const;
			Quaternion operator*(const Quaternion& quat) const;
			Vector3<T> operator*(const Vector3<T>& vec) const;
			Quaternion operator*(T scale) const;
			Quaternion operator/(const Quaternion& quat) const;

			Quaternion& operator+=(const Quaternion& quat);
			Quaternion& operator*=(const Quaternion& quat);
			Quaternion& operator*=(T scale);
			Quaternion& operator/=(const Quaternion& quat);

			bool operator==(const Quaternion& quat) const;
			bool operator!=(const Quaternion& quat) const;

			static Quaternion Identity();
			static Quaternion Lerp(const Quaternion& from, const Quaternion& to, T interpolation);
			static Quaternion LookAt(const Vector3<T>& forward, const Vector3<T>& up);
			static Quaternion Normalize(const Quaternion& quat, T* length = nullptr);
			static Quaternion RotationBetween(const Vector3<T>& from, const Vector3<T>& to);
			static Quaternion Mirror(Quaternion quat, const Vector3<T>& axis);
			static Quaternion Slerp(const Quaternion& from, const Quaternion& to, T interpolation);
			static Quaternion Zero();

			T w, x, y, z;
	};

	using Quaterniond = Quaternion<double>;
	using Quaternionf = Quaternion<float>;

	template<typename T> bool Serialize(SerializationContext& context, const Quaternion<T>& quat, TypeTag<Quaternion<T>>);
	template<typename T> bool Unserialize(SerializationContext& context, Quaternion<T>* quat, TypeTag<Quaternion<T>>);

	template<typename T> std::ostream& operator<<(std::ostream& out, const Nz::Quaternion<T>& quat);
}

#include <Nazara/Math/Quaternion.inl>

#endif // NAZARA_MATH_QUATERNION_HPP
