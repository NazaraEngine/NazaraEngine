// Copyright (C) 2012 Rémi Begues
//                    Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Basic.hpp>
#include <Nazara/Math/EulerAngles.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Core/Debug.hpp>

template<typename T>
NzQuaternion<T>::NzQuaternion()
{
}

template<typename T>
NzQuaternion<T>::NzQuaternion(T W, T X, T Y, T Z)
{
	Set(W, X, Y, Z);
}

template<typename T>
NzQuaternion<T>::NzQuaternion(T quat[4])
{
	Set(quat);
}

template<typename T>
NzQuaternion<T>::NzQuaternion(T angle, const NzVector3<T>& axis)
{
	Set(angle, axis);
}

template<typename T>
NzQuaternion<T>::NzQuaternion(const NzEulerAngles<T>& angles)
{
	Set(angles);
}
/*
template<typename T>
NzQuaternion<T>::NzQuaternion(const NzMatrix3<T>& mat)
{
	Set(mat);
}
*/
template<typename T>
template<typename U>
NzQuaternion<T>::NzQuaternion(const NzQuaternion<U>& quat)
{
	Set(quat);
}

template<typename T>
NzQuaternion<T> NzQuaternion<T>::GetConjugate() const
{
	return NzQuaternion(w, -x, -y, -z);
}

template<typename T>
NzQuaternion<T> NzQuaternion<T>::GetNormalized() const
{
	NzQuaternion<T> quat(*this);
	quat.Normalize();

	return quat;
}

template<typename T>
double NzQuaternion<T>::Magnitude() const
{
	return std::sqrt(SquaredMagnitude());
}

template<typename T>
double NzQuaternion<T>::Normalize()
{
	double length = Magnitude();

	if (length != 0.0)
	{
		w /= length;
		x /= length;
		y /= length;
		z /= length;
	}

	return length;
}

template<typename T>
T NzQuaternion<T>::SquaredMagnitude() const
{
	return w * w + x * x + y * y + z * z;
}

template<typename T>
void NzQuaternion<T>::Set(T W, T X, T Y, T Z)
{
	w = W;
	x = X;
	y = Y;
	z = Z;
}

template<typename T>
void NzQuaternion<T>::Set(T quat[4])
{
	w = quat[0];
	x = quat[1];
	y = quat[2];
	z = quat[3];
}

template<typename T>
void NzQuaternion<T>::Set(T angle, const NzVector3<T>& normalizedAxis)
{
	#if !NAZARA_MATH_ANGLE_RADIAN
	angle = NzDegreeToRadian(angle);
	#endif

	angle /= 2;

	auto sinAngle = std::sin(angle);

	w = std::cos(angle);
	x = normalizedAxis.x * sinAngle;
	y = normalizedAxis.y * sinAngle;
	z = normalizedAxis.z * sinAngle;
}

template<typename T>
void NzQuaternion<T>::Set(const NzEulerAngles<T>& angles)
{
	Set(angles.ToQuaternion());
}

template<typename T>
template<typename U>
void NzQuaternion<T>::Set(const NzQuaternion<U>& quat)
{
	w = static_cast<T>(quat.w);
	x = static_cast<T>(quat.x);
	y = static_cast<T>(quat.y);
	z = static_cast<T>(quat.z);
}

template<typename T>
void NzQuaternion<T>::Set(const NzQuaternion& quat)
{
	w = quat.w;
	x = quat.x;
	y = quat.y;
	z = quat.z;
}

template<typename T>
void NzQuaternion<T>::SetIdentity()
{
	Set(1.0, 0.0, 0.0, 0.0);
}

template<typename T>
void NzQuaternion<T>::SetZero()
{
	Set(0.0, 0.0, 0.0, 0.0);
}

template<typename T>
NzEulerAngles<T> NzQuaternion<T>::ToEulerAngles() const
{
	T test = x*y + z*w;
	if (test > 0.499)
		// singularity at north pole
		return NzEulerAngles<T>(NzDegrees(90.0), NzRadians(2.0 * std::atan2(x, w)), 0.0);

	if (test < -0.499)
		return NzEulerAngles<T>(NzDegrees(-90.0), NzRadians(-2.0 * std::atan2(x, w)), 0.0);

	T xx = x*x;
	T yy = y*y;
	T zz = z*z;

	return NzEulerAngles<T>(NzRadians(std::atan2(2.0*x*w - 2.0*y*z, 1.0 - 2.0*xx - 2.0*zz)),
							NzRadians(std::atan2(2.0*y*w - 2.0*x*z, 1.f - 2.0*yy - 2.0*zz)),
							NzRadians(std::asin(2.0*test)));
}

template<typename T>
NzString NzQuaternion<T>::ToString() const
{
	NzStringStream ss;

	return ss << "Quaternion(" << w << " | " << x << ", " << y << ", " << z << ')';
}

template<typename T>
NzQuaternion<T> NzQuaternion<T>::operator+(const NzQuaternion& quat) const
{
	return NzQuaternion(w + quat.w,
						x + quat.x,
						y + quat.y,
						z + quat.z);
}

template<typename T>
NzQuaternion<T> NzQuaternion<T>::operator*(const NzQuaternion& quat) const
{
	return NzQuaternion(w * quat.w - x * quat.x - y * quat.y - z * quat.z,
						w * quat.x + x * quat.w + y * quat.z - z * quat.y,
						w * quat.y + y * quat.w + z * quat.x - x * quat.z,
						w * quat.z + z * quat.w + x * quat.y - y * quat.x);
}

template<typename T>
NzVector3<T> NzQuaternion<T>::operator*(const NzVector3<T>& vec) const
{
	NzVector3<T> uv, uuv;
	NzVector3<T> qvec(x, y, z);
	uv = qvec.CrossProduct(vec);
	uuv = qvec.CrossProduct(uv);
	uv *= 2.0 * w;
	uuv *= 2.0;

	return vec + uv + uuv;
}

template<typename T>
NzQuaternion<T> NzQuaternion<T>::operator*(T scale) const
{
	return NzQuaternion(w * scale,
						x * scale,
						y * scale,
						z * scale);
}

template<typename T>
NzQuaternion<T> NzQuaternion<T>::operator/(const NzQuaternion& quat) const
{
	return GetConjugate(quat) * (*this);
}

template<typename T>
NzQuaternion<T> NzQuaternion<T>::operator+=(const NzQuaternion& quat)
{
	w += quat.w;
	x += quat.x;
	y += quat.y;
	z += quat.z;

	return *this;
}

template<typename T>
NzQuaternion<T> NzQuaternion<T>::operator*=(const NzQuaternion& quat)
{
    NzQuaternion q(*this);
	operator=(q * quat);

	return *this;
}

template<typename T>
NzQuaternion<T> NzQuaternion<T>::operator*=(T scale)
{
	w *= scale;
	x *= scale;
	y *= scale;
	z *= scale;

	return *this;
}

template<typename T>
NzQuaternion<T> NzQuaternion<T>::operator/=(const NzQuaternion& quat)
{
    NzQuaternion q(*this);
	operator=(q / quat);

	return *this;
}

template<typename T>
bool NzQuaternion<T>::operator==(const NzQuaternion& quat) const
{
	return NzNumberEquals(w, quat.w) &&
		   NzNumberEquals(x, quat.x) &&
		   NzNumberEquals(y, quat.y) &&
		   NzNumberEquals(z, quat.z);
}

template<typename T>
bool NzQuaternion<T>::operator!=(const NzQuaternion& quat) const
{
	return !operator==(quat);
}

template<typename T>
bool NzQuaternion<T>::operator<(const NzQuaternion& quat) const
{
	 return w < quat.w && x < quat.x && y < quat.y && z < quat.z;
}

template<typename T>
bool NzQuaternion<T>::operator<=(const NzQuaternion& quat) const
{
	return operator<(quat) || operator==(quat);
}

template<typename T>
bool NzQuaternion<T>::operator>(const NzQuaternion& quat) const
{
	return !operator<=(quat);
}

template<typename T>
bool NzQuaternion<T>::operator>=(const NzQuaternion& quat) const
{
	return !operator<(quat);
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzQuaternion<T>& quat)
{
	return out << quat.ToString();
}

#include <Nazara/Core/DebugOff.hpp>
