// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_MATH_EULERANGLES_HPP
#define NAZARA_MATH_EULERANGLES_HPP

#include <Nazara/Math/Angle.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <string>

namespace Nz
{
	struct SerializationContext;

	template<typename T>
	class EulerAngles
	{
		public:
			constexpr EulerAngles() = default;
			constexpr EulerAngles(DegreeAngle<T> P, DegreeAngle<T> Y, DegreeAngle<T> R);
			constexpr EulerAngles(const DegreeAngle<T> angles[3]);
			template<AngleUnit Unit> constexpr EulerAngles(const Angle<Unit, T>& angle);
			constexpr EulerAngles(const Quaternion<T>& quat);
			template<typename U> constexpr explicit EulerAngles(const EulerAngles<U>& angles);

			constexpr bool ApproxEqual(const EulerAngles& angles, T maxDifference = std::numeric_limits<T>::epsilon()) const;

			constexpr EulerAngles& Normalize();

			Quaternion<T> ToQuaternion() const;
			std::string ToString() const;

			constexpr EulerAngles operator+(const EulerAngles& angles) const;
			constexpr EulerAngles operator-(const EulerAngles& angles) const;

			constexpr EulerAngles& operator+=(const EulerAngles& angles);
			constexpr EulerAngles& operator-=(const EulerAngles& angles);

			constexpr bool operator==(const EulerAngles& angles) const;
			constexpr bool operator!=(const EulerAngles& angles) const;
			constexpr bool operator<(const EulerAngles& angles) const;
			constexpr bool operator<=(const EulerAngles& angles) const;
			constexpr bool operator>(const EulerAngles& angles) const;
			constexpr bool operator>=(const EulerAngles& angles) const;

			static constexpr bool ApproxEqual(const EulerAngles& lhs, const EulerAngles& rhs, T maxDifference = std::numeric_limits<T>::epsilon());
			static constexpr EulerAngles Zero();

			DegreeAngle<T> pitch, yaw, roll;
	};

	using EulerAnglesd = EulerAngles<double>;
	using EulerAnglesf = EulerAngles<float>;

	template<typename T> bool Serialize(SerializationContext& context, const EulerAngles<T>& eulerAngles, TypeTag<EulerAngles<T>>);
	template<typename T> bool Deserialize(SerializationContext& context, EulerAngles<T>* eulerAngles, TypeTag<EulerAngles<T>>);

	template<typename T> std::ostream& operator<<(std::ostream& out, const Nz::EulerAngles<T>& angles);
}


#include <Nazara/Math/EulerAngles.inl>

#endif // NAZARA_MATH_EULERANGLES_HPP
