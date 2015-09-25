// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <Nazara/Math/Config.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)

namespace Nz
{
	template<typename T>
	EulerAngles<T>::EulerAngles(T P, T Y, T R)
	{
		Set(P, Y, R);
	}

	template<typename T>
	EulerAngles<T>::EulerAngles(const T angles[3])
	{
		Set(angles);
	}

	template<typename T>
	EulerAngles<T>::EulerAngles(const Quaternion<T>& quat)
	{
		Set(quat);
	}

	template<typename T>
	template<typename U>
	EulerAngles<T>::EulerAngles(const EulerAngles<U>& angles)
	{
		Set(angles);
	}

	template<typename T>
	void EulerAngles<T>::MakeZero()
	{
		Set(F(0.0), F(0.0), F(0.0));
	}

	template<typename T>
	void EulerAngles<T>::Normalize()
	{
		pitch = NormalizeAngle(pitch);
		yaw = NormalizeAngle(yaw);
		roll = NormalizeAngle(roll);
	}

	template<typename T>
	void EulerAngles<T>::Set(T P, T Y, T R)
	{
		pitch = P;
		yaw = Y;
		roll = R;
	}

	template<typename T>
	void EulerAngles<T>::Set(const T angles[3])
	{
		pitch = angles[0];
		yaw = angles[1];
		roll = angles[2];
	}

	template<typename T>
	void EulerAngles<T>::Set(const EulerAngles& angles)
	{
		std::memcpy(this, &angles, sizeof(EulerAngles));
	}

	template<typename T>
	void EulerAngles<T>::Set(const Quaternion<T>& quat)
	{
		Set(quat.ToEulerAngles());
	}

	template<typename T>
	template<typename U>
	void EulerAngles<T>::Set(const EulerAngles<U>& angles)
	{
		pitch = F(angles.pitch);
		yaw = F(angles.yaw);
		roll = F(angles.roll);
	}

	template<typename T>
	Quaternion<T> EulerAngles<T>::ToQuaternion() const
	{
		T c1 = std::cos(ToRadians(yaw) / F(2.0));
		T c2 = std::cos(ToRadians(roll) / F(2.0));
		T c3 = std::cos(ToRadians(pitch) / F(2.0));

		T s1 = std::sin(ToRadians(yaw) / F(2.0));
		T s2 = std::sin(ToRadians(roll) / F(2.0));
		T s3 = std::sin(ToRadians(pitch) / F(2.0));

		return Quaternion<T>(c1 * c2 * c3 - s1 * s2 * s3,
							   s1 * s2 * c3 + c1 * c2 * s3,
							   s1 * c2 * c3 + c1 * s2 * s3,
							   c1 * s2 * c3 - s1 * c2 * s3);
	}

	template<typename T>
	String EulerAngles<T>::ToString() const
	{
		StringStream ss;

		return ss << "EulerAngles(" << pitch << ", " << yaw << ", " << roll << ')';
	}

	template<typename T>
	EulerAngles<T> EulerAngles<T>::operator+(const EulerAngles& angles) const
	{
		return EulerAngles(pitch + angles.pitch,
							 yaw + angles.yaw,
							 roll + angles.roll);
	}

	template<typename T>
	EulerAngles<T> EulerAngles<T>::operator-(const EulerAngles& angles) const
	{
		return EulerAngles(pitch - angles.pitch,
							 yaw - angles.yaw,
							 roll - angles.roll);
	}

	template<typename T>
	EulerAngles<T>& EulerAngles<T>::operator+=(const EulerAngles& angles)
	{
		pitch += angles.pitch;
		yaw += angles.yaw;
		roll += angles.roll;

		return *this;
	}

	template<typename T>
	EulerAngles<T>& EulerAngles<T>::operator-=(const EulerAngles& angles)
	{
		pitch -= angles.pitch;
		yaw -= angles.yaw;
		roll -= angles.roll;

		return *this;
	}

	template<typename T>
	bool EulerAngles<T>::operator==(const EulerAngles& angles) const
	{
		return NumberEquals(pitch, angles.pitch) &&
			   NumberEquals(yaw, angles.yaw) &&
			   NumberEquals(roll, angles.roll);
	}

	template<typename T>
	bool EulerAngles<T>::operator!=(const EulerAngles& angles) const
	{
		return !operator==(angles);
	}

	template<typename T>
	EulerAngles<T> EulerAngles<T>::Zero()
	{
		EulerAngles angles;
		angles.MakeZero();

		return angles;
	}
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nz::EulerAngles<T>& angles)
{
	return out << angles.ToString();
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
