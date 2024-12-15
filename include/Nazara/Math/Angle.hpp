// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_MATH_ANGLE_HPP
#define NAZARA_MATH_ANGLE_HPP

#include <Nazara/Math/Enums.hpp>
#include <NazaraUtils/TypeTag.hpp>
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
			template<AngleUnit FromUnit> constexpr Angle(const Angle<FromUnit, T>& angle);

			constexpr bool ApproxEqual(const Angle& angle) const;
			constexpr bool ApproxEqual(const Angle& angle, T maxDifference) const;

			T GetCos() const;
			T GetSin() const;
			std::pair<T, T> GetSinCos() const;
			T GetTan() const;

			constexpr Angle& Normalize();

			template<AngleUnit ToUnit> T To() const;
			template<AngleUnit ToUnit> Angle<ToUnit, T> ToAngle() const;
			constexpr T ToDegrees() const;
			constexpr Angle<AngleUnit::Degree, T> ToDegreeAngle() const;
			EulerAngles<T> ToEulerAngles() const;
			Quaternion<T> ToQuaternion() const;
			constexpr T ToRadians() const;
			constexpr Angle<AngleUnit::Radian, T> ToRadianAngle() const;
			std::string ToString() const;
			constexpr T ToTurns() const;
			constexpr Angle<AngleUnit::Turn, T> ToTurnAngle() const;

			constexpr Angle operator+() const;
			constexpr Angle operator-() const;

			constexpr Angle operator+(Angle other) const;
			constexpr Angle operator-(Angle other) const;
			constexpr Angle operator*(T scalar) const;
			constexpr Angle operator/(T divider) const;

			constexpr Angle& operator+=(Angle other);
			constexpr Angle& operator-=(Angle other);
			constexpr Angle& operator*=(T scalar);
			constexpr Angle& operator/=(T divider);

			constexpr bool operator==(Angle other) const;
			constexpr bool operator!=(Angle other) const;
			constexpr bool operator<(Angle other) const;
			constexpr bool operator<=(Angle other) const;
			constexpr bool operator>(Angle other) const;
			constexpr bool operator>=(Angle other) const;

			static constexpr bool ApproxEqual(const Angle& lhs, const Angle& rhs);
			static constexpr bool ApproxEqual(const Angle& lhs, const Angle& rhs, T maxDifference);
			static constexpr Angle Clamp(Angle angle, Angle min, Angle max);
			template<AngleUnit FromUnit> static constexpr Angle From(T value);
			static constexpr Angle FromDegrees(T degrees);
			static constexpr Angle FromRadians(T radians);
			static constexpr Angle FromTurns(T turn);
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

	template<typename T>
	using TurnAngle = Angle<AngleUnit::Turn, T>;

	using TurnAngled = TurnAngle<double>;
	using TurnAnglef = TurnAngle<float>;

	template<AngleUnit Unit, typename T> Angle<Unit, T> operator*(T scale, Angle<Unit, T> angle);

	template<AngleUnit Unit, typename T> Angle<Unit, T> operator/(T divider, Angle<Unit, T> angle);

	template<AngleUnit Unit, typename T> std::ostream& operator<<(std::ostream& out, Angle<Unit, T> angle);

	template<AngleUnit Unit, typename T> bool Serialize(SerializationContext& context, Angle<Unit, T> angle, TypeTag<Angle<Unit, T>>);
	template<AngleUnit Unit, typename T> bool Deserialize(SerializationContext& context, Angle<Unit, T>* angle, TypeTag<Angle<Unit, T>>);
}

#include <Nazara/Math/Angle.inl>

#endif // NAZARA_MATH_ANGLE_HPP
