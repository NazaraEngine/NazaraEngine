// Copyright (C) 2025 Rémi Bèges - Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_MATH_QUATERNION_HPP
#define NAZARA_MATH_QUATERNION_HPP

#include <Nazara/Math/Angle.hpp>
#include <Nazara/Math/EulerAngles.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <string>

namespace Nz
{
	struct SerializationContext;

	template<typename T> class Quaternion
	{
		public:
			constexpr Quaternion() = default;
			constexpr Quaternion(T W, T X, T Y, T Z);
			template<AngleUnit Unit> Quaternion(const Angle<Unit, T>& angle);
			Quaternion(const EulerAngles<T>& angles);
			constexpr Quaternion(RadianAngle<T> angle, const Vector3<T>& axis);
			constexpr Quaternion(const T quat[4]);
			template<typename U> constexpr explicit Quaternion(const Quaternion<U>& quat);

			RadianAngle<T> AngleBetween(const Quaternion& vec) const;
			constexpr bool ApproxEqual(const Quaternion& quat, T maxDifference = std::numeric_limits<T>::epsilon()) const;

			Quaternion& ComputeW();
			constexpr Quaternion& Conjugate();

			constexpr T DotProduct(const Quaternion& vec) const;

			constexpr Quaternion GetConjugate() const;
			Quaternion GetInverse() const;
			Quaternion GetNormal(T* length = nullptr) const;

			Quaternion& Inverse();

			T Magnitude() const;

			Quaternion& Normalize(T* length = nullptr);

			constexpr T SquaredMagnitude() const;

			RadianAngle<T> To2DAngle() const;
			EulerAngles<T> ToEulerAngles() const;
			std::string ToString() const;

			constexpr Quaternion operator+(const Quaternion& quat) const;
			constexpr Quaternion operator*(const Quaternion& quat) const;
			constexpr Vector3<T> operator*(const Vector3<T>& vec) const;
			constexpr Quaternion operator*(T scale) const;
			constexpr Quaternion operator/(const Quaternion& quat) const;

			constexpr Quaternion& operator+=(const Quaternion& quat);
			constexpr Quaternion& operator*=(const Quaternion& quat);
			constexpr Quaternion& operator*=(T scale);
			constexpr Quaternion& operator/=(const Quaternion& quat);

			constexpr bool operator==(const Quaternion& quat) const;
			constexpr bool operator!=(const Quaternion& quat) const;
			constexpr bool operator<(const Quaternion& quat) const;
			constexpr bool operator<=(const Quaternion& quat) const;
			constexpr bool operator>(const Quaternion& quat) const;
			constexpr bool operator>=(const Quaternion& quat) const;

			static RadianAngle<T> AngleBetween(const Quaternion& lhs, const Quaternion& rhs);
			static constexpr bool ApproxEqual(const Quaternion& lhs, const Quaternion& rhs, T maxDifference = std::numeric_limits<T>::epsilon());
			static constexpr Quaternion Identity();
			static constexpr Quaternion Lerp(const Quaternion& from, const Quaternion& to, T interpolation);
			static Quaternion LookAt(const Vector3<T>& forward, const Vector3<T>& up);
			static Quaternion Normalize(const Quaternion& quat, T* length = nullptr);
			static Quaternion RotationBetween(const Vector3<T>& from, const Vector3<T>& to);
			static Quaternion RotateTowards(const Quaternion& from, const Quaternion& to, RadianAngle<T> maxRotation);
			static Quaternion Mirror(Quaternion quat, const Vector3<T>& axis);
			static Quaternion Slerp(const Quaternion& from, const Quaternion& to, T interpolation);
			static constexpr Quaternion Zero();

			T w, x, y, z;
	};

	using Quaterniond = Quaternion<double>;
	using Quaternionf = Quaternion<float>;

	template<typename T> bool Serialize(SerializationContext& context, const Quaternion<T>& quat, TypeTag<Quaternion<T>>);
	template<typename T> bool Deserialize(SerializationContext& context, Quaternion<T>* quat, TypeTag<Quaternion<T>>);

	template<typename T> std::ostream& operator<<(std::ostream& out, const Nz::Quaternion<T>& quat);
}

#include <Nazara/Math/Quaternion.inl>

#endif // NAZARA_MATH_QUATERNION_HPP
