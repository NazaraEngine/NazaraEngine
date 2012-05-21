// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Basic.hpp>
#include <Nazara/Math/Config.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <cmath>
#include <Nazara/Core/Debug.hpp>

template<typename T>
NzEulerAngles<T>::NzEulerAngles()
{
}

template<typename T>
NzEulerAngles<T>::NzEulerAngles(T P, T Y, T R)
{
	Set(P, Y, R);
}

template<typename T>
NzEulerAngles<T>::NzEulerAngles(T angles[3])
{
	Set(angles);
}

template<typename T>
NzEulerAngles<T>::NzEulerAngles(const NzQuaternion<T>& quat)
{
	Set(quat);
}

template<typename T>
template<typename U>
NzEulerAngles<T>::NzEulerAngles(const NzEulerAngles<U>& angles)
{
	Set(angles);
}

template<typename T>
NzVector3<T> NzEulerAngles<T>::GetForward() const
{
	#if NAZARA_MATH_ANGLE_RADIAN
	return NzVector3<T>(std::cos(yaw), std::sin(roll), std::sin(yaw));
	#else
	return NzVector3<T>(std::cos(NzDegreeToRadian(yaw)), std::sin(NzDegreeToRadian(roll)), std::sin(NzDegreeToRadian(yaw)));
	#endif
}

template<typename T>
NzVector3<T> NzEulerAngles<T>::GetRight() const
{
	#if NAZARA_MATH_ANGLE_RADIAN
	return NzVector3<T>(std::sin(yaw), std::sin(pitch), std::cos(pitch));
	#else
	return NzVector3<T>(std::sin(NzDegreeToRadian(yaw)), std::sin(NzDegreeToRadian(pitch)), std::cos(NzDegreeToRadian(pitch)));
	#endif
}

template<typename T>
NzVector3<T> NzEulerAngles<T>::GetUp() const
{
	#if NAZARA_MATH_ANGLE_RADIAN
	return NzVector3<T>(std::sin(roll), std::cos(pitch), -std::sin(pitch));
	#else
	return NzVector3<T>(std::sin(NzDegreeToRadian(roll)), std::cos(NzDegreeToRadian(pitch)), -std::sin(NzDegreeToRadian(pitch)));
	#endif
}

template<typename T>
void NzEulerAngles<T>::Normalize()
{
	pitch = NzNormalizeAngle(pitch);
	yaw = NzNormalizeAngle(yaw);
	roll = NzNormalizeAngle(roll);
}

template<typename T>
void NzEulerAngles<T>::Set(T P, T Y, T R)
{
	pitch = P;
	yaw = Y;
	roll = R;
}

template<typename T>
void NzEulerAngles<T>::Set(T angles[3])
{
	pitch = angles[0];
	yaw = angles[1];
	roll = angles[2];
}

template<typename T>
void NzEulerAngles<T>::Set(const NzEulerAngles& angles)
{
	pitch = angles.pitch;
	yaw = angles.yaw;
	roll = angles.roll;
}

template<typename T>
void NzEulerAngles<T>::Set(const NzQuaternion<T>& quat)
{
	Set(quat.ToEulerAngles());
}

template<typename T>
template<typename U>
void NzEulerAngles<T>::Set(const NzEulerAngles<U>& angles)
{
	pitch = static_cast<T>(angles.pitch);
	yaw = static_cast<T>(angles.yaw);
	roll = static_cast<T>(angles.roll);
}

template<typename T>
void NzEulerAngles<T>::SetZero()
{
	Set(0.0, 0.0, 0.0);
}

template<typename T>
NzQuaternion<T> NzEulerAngles<T>::ToQuaternion() const
{
	NzQuaternion<T> Qx(pitch, NzVector3<T>(1.0, 0.0, 0.0));
	NzQuaternion<T> Qy(yaw, NzVector3<T>(0.0, 1.0, 0.0));
	NzQuaternion<T> Qz(roll, NzVector3<T>(0.0, 0.0, 1.0));

	return Qx * Qy * Qz;
}

template<typename T>
NzString NzEulerAngles<T>::ToString() const
{
	NzStringStream ss;

	return ss << "EulerAngles(" << pitch << ", " << yaw << ", " << roll << ')';
}

template<typename T>
NzEulerAngles<T> NzEulerAngles<T>::operator+(const NzEulerAngles& angles) const
{
	return NzEulerAngles(pitch + angles.pitch,
						 yaw + angles.yaw,
						 roll + angles.roll);
}

template<typename T>
NzEulerAngles<T> NzEulerAngles<T>::operator-(const NzEulerAngles& angles) const
{
	return NzEulerAngles(pitch - angles.pitch,
						 yaw - angles.yaw,
						 roll - angles.roll);
}

template<typename T>
NzEulerAngles<T> NzEulerAngles<T>::operator+=(const NzEulerAngles& angles)
{
	pitch += angles.pitch;
	yaw += angles.yaw;
	roll += angles.roll;

	return *this;
}

template<typename T>
NzEulerAngles<T> NzEulerAngles<T>::operator-=(const NzEulerAngles& angles)
{
	pitch -= angles.pitch;
	yaw -= angles.yaw;
	roll -= angles.roll;

	return *this;
}

template<typename T>
bool NzEulerAngles<T>::operator==(const NzEulerAngles& angles) const
{
	return NzNumberEquals(pitch, angles.pitch) &&
		   NzNumberEquals(yaw, angles.yaw) &&
		   NzNumberEquals(roll, angles.roll);
}

template<typename T>
bool NzEulerAngles<T>::operator!=(const NzEulerAngles& angles) const
{
	return !operator==(angles);
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzEulerAngles<T>& angles)
{
	return out << angles.ToString();
}

#include <Nazara/Core/DebugOff.hpp>
