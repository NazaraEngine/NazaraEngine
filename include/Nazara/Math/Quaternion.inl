// Copyright (C) 2015 Rémi Bèges - Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <Nazara/Math/Config.hpp>
#include <Nazara/Math/EulerAngles.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <cstring>
#include <limits>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)

namespace Nz
{
	template<typename T>
	Quaternion<T>::Quaternion(T W, T X, T Y, T Z)
	{
		Set(W, X, Y, Z);
	}

	template<typename T>
	Quaternion<T>::Quaternion(const T quat[4])
	{
		Set(quat);
	}

	template<typename T>
	Quaternion<T>::Quaternion(T angle, const Vector3<T>& axis)
	{
		Set(angle, axis);
	}

	template<typename T>
	Quaternion<T>::Quaternion(const EulerAngles<T>& angles)
	{
		Set(angles);
	}
	/*
	template<typename T>
	Quaternion<T>::Quaternion(const Matrix3<T>& mat)
	{
		Set(mat);
	}
	*/
	template<typename T>
	template<typename U>
	Quaternion<T>::Quaternion(const Quaternion<U>& quat)
	{
		Set(quat);
	}

	template<typename T>
	Quaternion<T>& Quaternion<T>::ComputeW()
	{
		T t = F(1.0) - SquaredMagnitude();

		if (t < F(0.0))
			w = F(0.0);
		else
			w = -std::sqrt(t);

		return *this;
	}

	template<typename T>
	Quaternion<T>& Quaternion<T>::Conjugate()
	{
		x = -x;
		y = -y;
		z = -z;

		return *this;
	}

	template<typename T>
	T Quaternion<T>::DotProduct(const Quaternion& quat) const
	{
		return w*quat.w + x*quat.x + y*quat.y + z*quat.z;
	}

	template<typename T>
	Quaternion<T> Quaternion<T>::GetConjugate() const
	{
		Quaternion<T> quat(*this);
		quat.Conjugate();

		return quat;
	}

	template<typename T>
	Quaternion<T> Quaternion<T>::GetInverse() const
	{
		Quaternion<T> quat(*this);
		quat.Inverse();

		return quat;
	}

	template<typename T>
	Quaternion<T> Quaternion<T>::GetNormal(T* length) const
	{
		Quaternion<T> quat(*this);
		quat.Normalize(length);

		return quat;
	}

	template<typename T>
	Quaternion<T>& Quaternion<T>::Inverse()
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
	Quaternion<T>& Quaternion<T>::MakeIdentity()
	{
		return Set(F(1.0), F(0.0), F(0.0), F(0.0));
	}

	template<typename T>
	Quaternion<T>& Quaternion<T>::MakeRotationBetween(const Vector3<T>& from, const Vector3<T>& to)
	{
		T dot = from.DotProduct(to);
		if (NumberEquals(dot, F(-1.0)))
		{
			Vector3<T> cross = Vector3<T>::CrossProduct(Vector3<T>::UnitX(), from);
			if (NumberEquals(cross.GetLength(), F(0.0)))
				cross = Vector3<T>::CrossProduct(Vector3<T>::UnitY(), from);

			return Set(F(180.0), cross);
		}
		else if (NumberEquals(dot, F(1.0)))
			return MakeIdentity();
		else
		{
			Vector3<T> a = from.CrossProduct(to);
			x = a.x;
			y = a.y;
			z = a.z;
			w = T(1.0) + dot;

			return Normalize();
		}
	}

	template<typename T>
	Quaternion<T>& Quaternion<T>::MakeZero()
	{
		return Set(F(0.0), F(0.0), F(0.0), F(0.0));
	}

	template<typename T>
	T Quaternion<T>::Magnitude() const
	{
		return std::sqrt(SquaredMagnitude());
	}

	template<typename T>
	Quaternion<T>& Quaternion<T>::Normalize(T* length)
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
	Quaternion<T>& Quaternion<T>::Set(T W, T X, T Y, T Z)
	{
		w = W;
		x = X;
		y = Y;
		z = Z;

		return *this;
	}

	template<typename T>
	Quaternion<T>& Quaternion<T>::Set(const T quat[4])
	{
		w = quat[0];
		x = quat[1];
		y = quat[2];
		z = quat[3];

		return *this;
	}

	template<typename T>
	Quaternion<T>& Quaternion<T>::Set(T angle, const Vector3<T>& axis)
	{
		#if !NAZARA_MATH_ANGLE_RADIAN
		angle = DegreeToRadian(angle);
		#endif

		angle /= F(2.0);

		Vector3<T> normalizedAxis = axis.GetNormal();

		T sinAngle = std::sin(angle);

		w = std::cos(angle);
		x = normalizedAxis.x * sinAngle;
		y = normalizedAxis.y * sinAngle;
		z = normalizedAxis.z * sinAngle;

		return Normalize();
	}

	template<typename T>
	Quaternion<T>& Quaternion<T>::Set(const EulerAngles<T>& angles)
	{
		return Set(angles.ToQuaternion());
	}

	template<typename T>
	template<typename U>
	Quaternion<T>& Quaternion<T>::Set(const Quaternion<U>& quat)
	{
		w = F(quat.w);
		x = F(quat.x);
		y = F(quat.y);
		z = F(quat.z);

		return *this;
	}

	template<typename T>
	Quaternion<T>& Quaternion<T>::Set(const Quaternion& quat)
	{
		std::memcpy(this, &quat, sizeof(Quaternion));

		return *this;
	}

	template<typename T>
	T Quaternion<T>::SquaredMagnitude() const
	{
		return w*w + x*x + y*y + z*z;
	}

	template<typename T>
	EulerAngles<T> Quaternion<T>::ToEulerAngles() const
	{
		T test = x*y + z*w;
		if (test > F(0.499))
			// singularity at north pole
			return EulerAngles<T>(FromDegrees(F(90.0)), FromRadians(F(2.0) * std::atan2(x, w)), F(0.0));

		if (test < F(-0.499))
			return EulerAngles<T>(FromDegrees(F(-90.0)), FromRadians(F(-2.0) * std::atan2(x, w)), F(0.0));

		return EulerAngles<T>(FromRadians(std::atan2(F(2.0)*x*w - F(2.0)*y*z, F(1.0) - F(2.0)*x*x - F(2.0)*z*z)),
								FromRadians(std::atan2(F(2.0)*y*w - F(2.0)*x*z, F(1.0) - F(2.0)*y*y - F(2.0)*z*z)),
								FromRadians(std::asin(F(2.0)*test)));
	}

	template<typename T>
	String Quaternion<T>::ToString() const
	{
		StringStream ss;

		return ss << "Quaternion(" << w << " | " << x << ", " << y << ", " << z << ')';
	}

	template<typename T>
	Quaternion<T>& Quaternion<T>::operator=(const Quaternion& quat)
	{
		return Set(quat);
	}

	template<typename T>
	Quaternion<T> Quaternion<T>::operator+(const Quaternion& quat) const
	{
		Quaternion result;
		result.w = w + quat.w;
		result.x = x + quat.x;
		result.y = y + quat.y;
		result.z = z + quat.z;

		return result;
	}

	template<typename T>
	Quaternion<T> Quaternion<T>::operator*(const Quaternion& quat) const
	{
		Quaternion result;
		result.w = w*quat.w - x*quat.x - y*quat.y - z*quat.z;
		result.x = w*quat.x + x*quat.w + y*quat.z - z*quat.y;
		result.y = w*quat.y + y*quat.w + z*quat.x - x*quat.z;
		result.z = w*quat.z + z*quat.w + x*quat.y - y*quat.x;

		return result;
	}

	template<typename T>
	Vector3<T> Quaternion<T>::operator*(const Vector3<T>& vec) const
	{
		Vector3f quatVec(x, y, z);
		Vector3f uv = quatVec.CrossProduct(vec);
		Vector3f uuv = quatVec.CrossProduct(uv);
		uv *= F(2.0) * w;
		uuv *= F(2.0);

		return vec + uv + uuv;
	}

	template<typename T>
	Quaternion<T> Quaternion<T>::operator*(T scale) const
	{
		return Quaternion(w * scale,
							x * scale,
							y * scale,
							z * scale);
	}

	template<typename T>
	Quaternion<T> Quaternion<T>::operator/(const Quaternion& quat) const
	{
		return quat.GetConjugate() * (*this);
	}

	template<typename T>
	Quaternion<T>& Quaternion<T>::operator+=(const Quaternion& quat)
	{
		return operator=(operator+(quat));
	}

	template<typename T>
	Quaternion<T>& Quaternion<T>::operator*=(const Quaternion& quat)
	{
		return operator=(operator*(quat));
	}

	template<typename T>
	Quaternion<T>& Quaternion<T>::operator*=(T scale)
	{
		return operator=(operator*(scale));
	}

	template<typename T>
	Quaternion<T>& Quaternion<T>::operator/=(const Quaternion& quat)
	{
		return operator=(operator/(quat));
	}

	template<typename T>
	bool Quaternion<T>::operator==(const Quaternion& quat) const
	{
		return NumberEquals(w, quat.w) &&
			   NumberEquals(x, quat.x) &&
			   NumberEquals(y, quat.y) &&
			   NumberEquals(z, quat.z);
	}

	template<typename T>
	bool Quaternion<T>::operator!=(const Quaternion& quat) const
	{
		return !operator==(quat);
	}

	template<typename T>
	Quaternion<T> Quaternion<T>::Identity()
	{
		Quaternion quaternion;
		quaternion.MakeIdentity();

		return quaternion;
	}

	template<typename T>
	Quaternion<T> Quaternion<T>::Lerp(const Quaternion& from, const Quaternion& to, T interpolation)
	{
		#ifdef NAZARA_DEBUG
		if (interpolation < F(0.0) || interpolation > F(1.0))
		{
			NazaraError("Interpolation must be in range [0..1] (Got " + String::Number(interpolation) + ')');
			return Zero();
		}
		#endif

		Quaternion interpolated;
		interpolated.w = Lerp(from.w, to.w, interpolation);
		interpolated.x = Lerp(from.x, to.x, interpolation);
		interpolated.y = Lerp(from.y, to.y, interpolation);
		interpolated.z = Lerp(from.z, to.z, interpolation);

		return interpolated;
	}

	template<typename T>
	Quaternion<T> Quaternion<T>::Normalize(const Quaternion& quat, T* length)
	{
		return quat.GetNormal(length);
	}

	template<typename T>
	Quaternion<T> Quaternion<T>::RotationBetween(const Vector3<T>& from, const Vector3<T>& to)
	{
		Quaternion quaternion;
		quaternion.MakeRotationBetween(from, to);

		return quaternion;
	}

	template<typename T>
	Quaternion<T> Quaternion<T>::Slerp(const Quaternion& from, const Quaternion& to, T interpolation)
	{
		#ifdef NAZARA_DEBUG
		if (interpolation < F(0.0) || interpolation > F(1.0))
		{
			NazaraError("Interpolation must be in range [0..1] (Got " + String::Number(interpolation) + ')');
			return Zero();
		}
		#endif

		Quaternion q;

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

		Quaternion result(k0 * from.w, k0 * from.x, k0 * from.y, k0 * from.z);
		return result += q*k1;
	}

	template<typename T>
	Quaternion<T> Quaternion<T>::Zero()
	{
		Quaternion quaternion;
		quaternion.MakeZero();

		return quaternion;
	}
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nz::Quaternion<T>& quat)
{
	return out << quat.ToString();
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
