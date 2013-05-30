// Copyright (C) 2013 Rémi Bèges - Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Basic.hpp>
#include <Nazara/Math/Config.hpp>
#include <Nazara/Math/EulerAngles.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <cstring>
#include <limits>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)

template<typename T>
NzQuaternion<T>::NzQuaternion(T W, T X, T Y, T Z)
{
	Set(W, X, Y, Z);
}

template<typename T>
NzQuaternion<T>::NzQuaternion(const T quat[4])
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
NzQuaternion<T>& NzQuaternion<T>::ComputeW()
{
	T t = F(1.0) - SquaredMagnitude();

	if (t < F(0.0))
		w = F(0.0);
	else
		w = -std::sqrt(t);

	return *this;
}

template<typename T>
NzQuaternion<T>& NzQuaternion<T>::Conjugate()
{
	x = -x;
	y = -y;
	z = -z;

	return *this;
}

template<typename T>
T NzQuaternion<T>::DotProduct(const NzQuaternion& quat) const
{
	return w*quat.w + x*quat.x + y*quat.y + z*quat.z;
}

template<typename T>
NzQuaternion<T> NzQuaternion<T>::GetConjugate() const
{
	NzQuaternion<T> quat(*this);
	quat.Conjugate();

	return quat;
}

template<typename T>
NzQuaternion<T> NzQuaternion<T>::GetInverse() const
{
	NzQuaternion<T> quat(*this);
	quat.Inverse();

	return quat;
}

template<typename T>
NzQuaternion<T> NzQuaternion<T>::GetNormal(T* length) const
{
	NzQuaternion<T> quat(*this);
	quat.Normalize(length);

	return quat;
}

template<typename T>
NzQuaternion<T>& NzQuaternion<T>::Inverse()
{
	T norm = SquaredMagnitude();
	if (norm > F(0.0))
	{
		T invNorm = F(1.0) / norm;

		w *= invNorm;
		x *= -invNorm;
		y *= -invNorm;
		z *= -invNorm;
	}

	return *this;
}

template<typename T>
NzQuaternion<T>& NzQuaternion<T>::MakeIdentity()
{
	return Set(F(1.0), F(0.0), F(0.0), F(0.0));
}

template<typename T>
NzQuaternion<T>& NzQuaternion<T>::MakeRotationBetween(const NzVector3<T>& from, const NzVector3<T>& to)
{
	T dot = from.DotProduct(to);
	if (NzNumberEquals(dot, F(-1.0)))
	{
		NzVector3<T> cross = NzVector3<T>::CrossProduct(NzVector3<T>::UnitX(), from);
		if (NzNumberEquals(cross.GetLength(), F(0.0)))
			cross = NzVector3<T>::CrossProduct(NzVector3<T>::UnitY(), from);

		return Set(F(180.0), cross);
	}
	else if (NzNumberEquals(dot, F(1.0)))
		return MakeIdentity();
	else
	{
		NzVector3<T> a = from.CrossProduct(to);
		x = a.x;
		y = a.y;
		z = a.z;
		w = T(1.0) + dot;

		return Normalize();
	}
}

template<typename T>
NzQuaternion<T>& NzQuaternion<T>::MakeZero()
{
	return Set(F(0.0), F(0.0), F(0.0), F(0.0));
}

template<typename T>
T NzQuaternion<T>::Magnitude() const
{
	return std::sqrt(SquaredMagnitude());
}

template<typename T>
NzQuaternion<T>& NzQuaternion<T>::Normalize(T* length)
{
	T norm = std::sqrt(SquaredMagnitude());
	T invNorm = F(1.0) / norm;

	w *= invNorm;
	x *= invNorm;
	y *= invNorm;
	z *= invNorm;

	if (length)
		*length = norm;

	return *this;
}

template<typename T>
NzQuaternion<T>& NzQuaternion<T>::Set(T W, T X, T Y, T Z)
{
	w = W;
	x = X;
	y = Y;
	z = Z;

	return *this;
}

template<typename T>
NzQuaternion<T>& NzQuaternion<T>::Set(const T quat[4])
{
	w = quat[0];
	x = quat[1];
	y = quat[2];
	z = quat[3];

	return *this;
}

template<typename T>
NzQuaternion<T>& NzQuaternion<T>::Set(T angle, const NzVector3<T>& axis)
{
	#if !NAZARA_MATH_ANGLE_RADIAN
	angle = NzDegreeToRadian(angle);
	#endif

	angle *= F(0.5);

	NzVector3<T> normalizedAxis = axis.GetNormal();

	T sinAngle = std::sin(angle);

	w = std::cos(angle);
	x = normalizedAxis.x * sinAngle;
	y = normalizedAxis.y * sinAngle;
	z = normalizedAxis.z * sinAngle;

	return Normalize();
}

template<typename T>
NzQuaternion<T>& NzQuaternion<T>::Set(const NzEulerAngles<T>& angles)
{
	return Set(angles.ToQuaternion());
}

template<typename T>
template<typename U>
NzQuaternion<T>& NzQuaternion<T>::Set(const NzQuaternion<U>& quat)
{
	w = static_cast<T>(quat.w);
	x = static_cast<T>(quat.x);
	y = static_cast<T>(quat.y);
	z = static_cast<T>(quat.z);

	return *this;
}

template<typename T>
NzQuaternion<T>& NzQuaternion<T>::Set(const NzQuaternion& quat)
{
	std::memcpy(this, &quat, sizeof(NzQuaternion));

	return *this;
}

template<typename T>
T NzQuaternion<T>::SquaredMagnitude() const
{
	return w*w + x*x + y*y + z*z;
}

template<typename T>
NzEulerAngles<T> NzQuaternion<T>::ToEulerAngles() const
{
	T test = x*y + z*w;
	if (test > F(0.499))
		// singularity at north pole
		return NzEulerAngles<T>(NzDegrees(F(90.0)), NzRadians(F(2.0) * std::atan2(x, w)), F(0.0));

	if (test < F(-0.499))
		return NzEulerAngles<T>(NzDegrees(F(-90.0)), NzRadians(F(-2.0) * std::atan2(x, w)), F(0.0));

	return NzEulerAngles<T>(NzRadians(std::atan2(F(2.0)*x*w - F(2.0)*y*z, F(1.0) - F(2.0)*x* - F(2.0)*z*z)),
							NzRadians(std::atan2(F(2.0)*y*w - F(2.0)*x*z, F(1.0) - F(2.0)*y*y - F(2.0)*z*z)),
							NzRadians(std::asin(F(2.0)*test)));
}

template<typename T>
NzString NzQuaternion<T>::ToString() const
{
	NzStringStream ss;

	return ss << "Quaternion(" << w << " | " << x << ", " << y << ", " << z << ')';
}

template<typename T>
NzQuaternion<T>& NzQuaternion<T>::operator=(const NzQuaternion& quat)
{
	return Set(quat);
}

template<typename T>
NzQuaternion<T> NzQuaternion<T>::operator+(const NzQuaternion& quat) const
{
	NzQuaternion result;
	result.w = w + quat.w;
	result.x = x + quat.x;
	result.y = y + quat.y;
	result.z = z + quat.z;

	return result;
}

template<typename T>
NzQuaternion<T> NzQuaternion<T>::operator*(const NzQuaternion& quat) const
{
	NzQuaternion result;
	result.w = w*quat.w - x*quat.x - y*quat.y - z*quat.z;
	result.x = w*quat.x + x*quat.w + y*quat.z - z*quat.y;
	result.y = w*quat.y + y*quat.w + z*quat.x - x*quat.z;
	result.z = w*quat.z + z*quat.w + x*quat.y - y*quat.x;

	return result;
}

template<typename T>
NzVector3<T> NzQuaternion<T>::operator*(const NzVector3<T>& vec) const
{
	NzVector3f quatVec(x, y, z);
	NzVector3f uv = quatVec.CrossProduct(vec);
	NzVector3f uuv = quatVec.CrossProduct(uv);
	uv *= F(2.0) * w;
	uuv *= F(2.0);

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
NzQuaternion<T>& NzQuaternion<T>::operator+=(const NzQuaternion& quat)
{
	return operator=(operator+(quat));
}

template<typename T>
NzQuaternion<T>& NzQuaternion<T>::operator*=(const NzQuaternion& quat)
{
	return operator=(operator*(quat));
}

template<typename T>
NzQuaternion<T>& NzQuaternion<T>::operator*=(T scale)
{
	return operator=(operator*(scale));
}

template<typename T>
NzQuaternion<T>& NzQuaternion<T>::operator/=(const NzQuaternion& quat)
{
	return operator=(operator/(quat));
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
NzQuaternion<T> NzQuaternion<T>::Identity()
{
	NzQuaternion quaternion;
	quaternion.MakeIdentity();

	return quaternion;
}

template<typename T>
NzQuaternion<T> NzQuaternion<T>::Lerp(const NzQuaternion& from, const NzQuaternion& to, T interpolation)
{
	#ifdef NAZARA_DEBUG
	if (interpolation < F(0.0) || interpolation > F(1.0))
	{
		NazaraError("Interpolation must be in range [0..1] (Got " + NzString::Number(interpolation) + ')');
		return Zero();
	}
	#endif

	NzQuaternion interpolated;
	interpolated.w = NzLerp(from.w, to.w, interpolation);
	interpolated.x = NzLerp(from.x, to.x, interpolation);
	interpolated.y = NzLerp(from.y, to.y, interpolation);
	interpolated.z = NzLerp(from.z, to.z, interpolation);

	return interpolated;
}

template<typename T>
NzQuaternion<T> NzQuaternion<T>::RotationBetween(const NzVector3<T>& from, const NzVector3<T>& to)
{
	NzQuaternion quaternion;
	quaternion.MakeRotationBetween(from, to);

	return quaternion;
}

template<typename T>
NzQuaternion<T> NzQuaternion<T>::Slerp(const NzQuaternion& from, const NzQuaternion& to, T interpolation)
{
	#ifdef NAZARA_DEBUG
	if (interpolation < F(0.0) || interpolation > F(1.0))
	{
		NazaraError("Interpolation must be in range [0..1] (Got " + NzString::Number(interpolation) + ')');
		return Zero();
	}
	#endif

	NzQuaternion q;

	T cosOmega = from.DotProduct(to);
	if (cosOmega < F(0.0))
	{
		// On inverse tout
		q.Set(-to.w, -to.x, -to.y, -to.z);
		cosOmega = -cosOmega;
	}
	else
		q.Set(to);

	T k0, k1;
	if (cosOmega > F(0.9999))
	{
		// Interpolation linéaire pour éviter une division par zéro
        k0 = F(1.0) - interpolation;
        k1 = interpolation;
    }
    else
    {
        T sinOmega = std::sqrt(F(1.0) - cosOmega*cosOmega);
        T omega = std::atan2(sinOmega, cosOmega);

		// Pour éviter deux divisions
		sinOmega = F(1.0)/sinOmega;

        k0 = std::sin((F(1.0) - interpolation) * omega) * sinOmega;
        k1 = std::sin(interpolation*omega) * sinOmega;
    }

    NzQuaternion result(k0 * from.w, k0 * from.x, k0 * from.y, k0 * from.z);
    return result += q*k1;
}

template<typename T>
NzQuaternion<T> NzQuaternion<T>::Zero()
{
	NzQuaternion quaternion;
	quaternion.MakeZero();

	return quaternion;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzQuaternion<T>& quat)
{
	return out << quat.ToString();
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
