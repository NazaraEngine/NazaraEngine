// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Config.hpp

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
			EulerAngles() = default;
			EulerAngles(DegreeAngle<T> P, DegreeAngle<T> Y, DegreeAngle<T> R);
			EulerAngles(const DegreeAngle<T> angles[3]);
			template<AngleUnit Unit> EulerAngles(const Angle<Unit, T>& angle);
			//EulerAngles(const Matrix3<T>& mat);
			EulerAngles(const Quaternion<T>& quat);
			template<typename U> explicit EulerAngles(const EulerAngles<U>& angles);
			EulerAngles(const EulerAngles& angles) = default;
			~EulerAngles() = default;

			void MakeZero();

			EulerAngles& Normalize();

			EulerAngles& Set(DegreeAngle<T> P, DegreeAngle<T> Y, DegreeAngle<T> R);
			EulerAngles& Set(const DegreeAngle<T> angles[3]);
			template<AngleUnit Unit> EulerAngles& Set(const Angle<Unit, T>& angles);
			//EulerAngles& Set(const Matrix3<T>& mat);
			EulerAngles& Set(const Quaternion<T>& quat);
			template<typename U> EulerAngles& Set(const EulerAngles<U>& angles);

			//Matrix3<T> ToRotationMatrix() const;
			Quaternion<T> ToQuaternion() const;
			std::string ToString() const;

			EulerAngles operator+(const EulerAngles& angles) const;
			EulerAngles operator-(const EulerAngles& angles) const;
			/*EulerAngles operator*(const EulerAngles& angles) const;
			EulerAngles operator/(const EulerAngles& angles) const;*/
			EulerAngles& operator=(const EulerAngles& other) = default;

			EulerAngles& operator+=(const EulerAngles& angles);
			EulerAngles& operator-=(const EulerAngles& angles);
			/*EulerAngles operator*=(const EulerAngles& angles);
			EulerAngles operator/=(const EulerAngles& angles);*/

			bool operator==(const EulerAngles& angles) const;
			bool operator!=(const EulerAngles& angles) const;

			static EulerAngles Zero();

			DegreeAngle<T> pitch, yaw, roll;
	};

	using EulerAnglesd = EulerAngles<double>;
	using EulerAnglesf = EulerAngles<float>;

	template<typename T> bool Serialize(SerializationContext& context, const EulerAngles<T>& eulerAngles, TypeTag<EulerAngles<T>>);
	template<typename T> bool Unserialize(SerializationContext& context, EulerAngles<T>* eulerAngles, TypeTag<EulerAngles<T>>);

	template<typename T> std::ostream& operator<<(std::ostream& out, const Nz::EulerAngles<T>& angles);
}


#include <Nazara/Math/EulerAngles.inl>

#endif // NAZARA_MATH_EULERANGLES_HPP
