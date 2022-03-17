// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATH_ANGLE_HPP
#define NAZARA_MATH_ANGLE_HPP

#include <Nazara/Core/TypeTag.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <Nazara/Math/Enums.hpp>
#include <ostream>
#include <string>
#include <type_traits>
#include <utility>

namespace Nz
{
	struct SerializationContext;

	template<typename T> class EulerAngles;
	template<typename T> class Quaternion;

	template<AngleUnit Unit, typename T>
	class Angle
	{
		public:
			constexpr Angle() = default;
			constexpr Angle(T angle);
			template<typename U> constexpr explicit Angle(const Angle<Unit, U>& Angle);
			constexpr Angle(const Angle<AngleUnit::Degree, T>& angle);
			constexpr Angle(const Angle<AngleUnit::Radian, T>& angle);
			~Angle() = default;

			T GetCos() const;
			T GetSin() const;
			std::pair<T, T> GetSinCos() const;
			T GetTan() const;

			constexpr Angle& MakeZero();

			constexpr Angle& Normalize();

			constexpr Angle& Set(const Angle& ang);
			template<typename U> constexpr Angle& Set(const Angle<Unit, U>& ang);

			constexpr T ToDegrees() const;
			constexpr Angle<AngleUnit::Degree, T> ToDegreeAngle() const;
			EulerAngles<T> ToEulerAngles() const;
			Quaternion<T> ToQuaternion() const;
			constexpr T ToRadians() const;
			constexpr Angle<AngleUnit::Radian, T> ToRadianAngle() const;
			std::string ToString() const;

			constexpr Angle& operator=(const Angle&) = default;

			constexpr const Angle& operator+() const;
			constexpr Angle operator-() const;

			constexpr Angle operator+(const Angle& other) const;
			constexpr Angle operator-(const Angle& other) const;
			constexpr Angle operator*(T scalar) const;
			constexpr Angle operator/(T divider) const;

			constexpr Angle& operator+=(const Angle& other);
			constexpr Angle& operator-=(const Angle& other);
			constexpr Angle& operator*=(T scalar);
			constexpr Angle& operator/=(T divider);

			constexpr bool operator==(const Angle& other) const;
			constexpr bool operator!=(const Angle& other) const;

			static constexpr Angle FromDegrees(T ang);
			static constexpr Angle FromRadians(T ang);
			static constexpr Angle Zero();

			T value;
	};

	template<typename T>
	using DegreeAngle = Angle<AngleUnit::Degree, T>;

	using DegreeAngled = DegreeAngle<double>;
	using DegreeAnglef = DegreeAngle<float>;

	template<typename T>
	using RadianAngle = Angle<AngleUnit::Radian, T>;

	using RadianAngled = RadianAngle<double>;
	using RadianAnglef = RadianAngle<float>;

	template<AngleUnit Unit, typename T> bool Serialize(SerializationContext& context, const Angle<Unit, T>& angle, TypeTag<Angle<Unit, T>>);
	template<AngleUnit Unit, typename T> bool Unserialize(SerializationContext& context, Angle<Unit, T>* angle, TypeTag<Angle<Unit, T>>);
}

template<Nz::AngleUnit Unit, typename T>
Nz::Angle<Unit, T> operator*(T scale, const Nz::Angle<Unit, T>& angle);

template<Nz::AngleUnit Unit, typename T>
Nz::Angle<Unit, T> operator/(T divider, const Nz::Angle<Unit, T>& angle);

template<Nz::AngleUnit Unit, typename T>
std::ostream& operator<<(std::ostream& out, const Nz::Angle<Unit, T>& angle);

#include <Nazara/Math/Angle.inl>

#endif // NAZARA_MATH_ANGLE_HPP
