// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_ANGLE_HPP
#define NAZARA_ANGLE_HPP

#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <Nazara/Math/Enums.hpp>
#include <ostream>
#include <type_traits>
#include <utility>

namespace Nz
{
	struct SerializationContext;

	template<AngleUnit Unit, typename T>
	class Angle
	{
		public:
			Angle() = default;
			Angle(T value);
			template<AngleUnit U = Unit, typename = std::enable_if_t<U == AngleUnit::Degree>> explicit Angle(const Angle<AngleUnit::Radian, T>& value) { Set(value); }
			template<AngleUnit U = Unit, typename = std::enable_if_t<U == AngleUnit::Radian>> explicit Angle(const Angle<AngleUnit::Degree, T>& value) { Set(value); }
			template<typename U> explicit Angle(const Angle<Unit, U>& Angle);
			Angle(const Angle&) = default;
			~Angle() = default;

			T GetCos() const;
			T GetSin() const;
			std::pair<T, T> GetSinCos() const;
			T GetTan() const;

			Angle& MakeZero();

			void Normalize();

			template<AngleUnit U = Unit, typename = std::enable_if_t<U == AngleUnit::Degree>> Angle& Set(const Angle<AngleUnit::Radian, T>& Angle);
			template<AngleUnit U = Unit, typename = std::enable_if_t<U == AngleUnit::Radian>> Angle& Set(const Angle<AngleUnit::Degree, T>& Angle);
			Angle& Set(const Angle& Angle);
			template<typename U> Angle& Set(const Angle<Unit, U>& Angle);

			Angle<AngleUnit::Degree, T> ToDegrees() const;
			Angle<AngleUnit::Radian, T> ToRadians() const;
			String ToString() const;

			Angle& operator=(const Angle&) = default;

			Angle operator+(const Angle& other) const;
			Angle operator-(const Angle& other) const;

			Angle& operator+=(const Angle& other);
			Angle& operator-=(const Angle& other);
			Angle& operator*=(T scalar);
			Angle& operator/=(T divider);

			bool operator==(const Angle& other) const;
			bool operator!=(const Angle& other) const;

			static Angle Zero();

			T angle;
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

#endif // NAZARA_ANGLE_HPP
