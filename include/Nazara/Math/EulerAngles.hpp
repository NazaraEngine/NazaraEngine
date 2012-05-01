// Copyright (C) 2012 Rémi Begues
//                    Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_EULERANGLES_HPP
#define NAZARA_EULERANGLES_HPP

#include <Nazara/Core/String.hpp>

template<typename T> class NzQuaternion;
template<typename T> class NzVector3;

template<typename T> class NzEulerAngles
{
	public:
		NzEulerAngles();
		NzEulerAngles(T P, T Y, T R);
		NzEulerAngles(T angles[3]);
		//NzEulerAngles(const NzMatrix3<T>& mat);
		NzEulerAngles(const NzQuaternion<T>& quat);
		template<typename U> explicit NzEulerAngles(const NzEulerAngles<U>& angles);
		NzEulerAngles(const NzEulerAngles& angles) = default;
		~NzEulerAngles() = default;

		NzVector3<T> GetForward() const;
		NzVector3<T> GetRight() const;
		NzVector3<T> GetUp() const;

		void Normalize();

		void Set(T P, T Y, T R);
		void Set(T angles[3]);
		void Set(const NzEulerAngles<T>& angles);
		//void Set(const NzMatrix3<T>& mat);
		void Set(const NzQuaternion<T>& quat);
		template<typename U> void Set(const NzEulerAngles<U>& angles);
		void SetZero();

		//NzEulerAngles<T> ToEulerAngles() const;
		//NzMatrix3<T> ToRotationMatrix() const;
		NzQuaternion<T> ToQuaternion() const;
		NzString ToString() const;

		NzEulerAngles operator+(const NzEulerAngles& angles) const;
		NzEulerAngles operator-(const NzEulerAngles& angles) const;
		NzEulerAngles operator*(const NzEulerAngles& angles) const;
		NzEulerAngles operator/(const NzEulerAngles& angles) const;

		NzEulerAngles operator+=(const NzEulerAngles& angles);
		NzEulerAngles operator-=(const NzEulerAngles& angles);
		NzEulerAngles operator*=(const NzEulerAngles& angles);
		NzEulerAngles operator/=(const NzEulerAngles& angles);

		bool operator==(const NzEulerAngles& angles) const;
		bool operator!=(const NzEulerAngles& angles) const;

		T pitch, yaw, roll;
};

template<typename T> std::ostream& operator<<(std::ostream& out, const NzEulerAngles<T>& angles);

typedef NzEulerAngles<double> NzEulerAnglesd;
typedef NzEulerAngles<float> NzEulerAnglesf;

#include <Nazara/Math/EulerAngles.inl>

#endif // NAZARA_EULERANGLES_HPP
