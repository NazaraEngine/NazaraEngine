// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Basic.hpp>
#include <Nazara/Math/Config.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)

template<typename T>
NzEulerAngles<T>::NzEulerAngles(T P, T Y, T R)
{
	Set(P, Y, R);
}

template<typename T>
NzEulerAngles<T>::NzEulerAngles(const T angles[3])
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
void NzEulerAngles<T>::MakeZero()
{
	Set(F(0.0), F(0.0), F(0.0));
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
void NzEulerAngles<T>::Set(const T angles[3])
{
	pitch = angles[0];
	yaw = angles[1];
	roll = angles[2];
}

template<typename T>
void NzEulerAngles<T>::Set(const NzEulerAngles& angles)
{
	std::memcpy(this, &angles, sizeof(NzEulerAngles));
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
NzQuaternion<T> NzEulerAngles<T>::ToQuaternion() const
{
	NzQuaternion<T> rotX(pitch, NzVector3<T>::UnitX());
	NzQuaternion<T> rotY(yaw, NzVector3<T>::UnitY());
	NzQuaternion<T> rotZ(roll, NzVector3<T>::UnitZ());

	return rotY * rotX * rotZ;
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
NzEulerAngles<T> NzEulerAngles<T>::Zero()
{
	NzEulerAngles angles;
	angles.MakeZero();

	return angles;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzEulerAngles<T>& angles)
{
	return out << angles.ToString();
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
