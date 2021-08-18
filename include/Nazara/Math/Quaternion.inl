// Copyright (C) 2017 Rémi Bèges - Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <Nazara/Math/Config.hpp>
#include <Nazara/Math/EulerAngles.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <cstring>
#include <limits>
#include <sstream>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup math
	* \class Nz::Quaternion
	* \brief Math class that represents an element of the quaternions
	*
	* \remark The quaternion is meant to be 'unit' to represent rotations in a three dimensional space
	*/

	/*!
	* \brief Constructs a Quaternion object from its components
	*
	* \param W W component
	* \param X X component
	* \param Y Y component
	* \param Z Z component
	*/

	template<typename T>
	Quaternion<T>::Quaternion(T W, T X, T Y, T Z)
	{
		Set(W, X, Y, Z);
	}

	/*!
	* \brief Constructs a Quaternion object from an angle
	*
	* \param angle Angle representing a 2D rotation
	*/
	template<typename T>
	template<AngleUnit Unit>
	Quaternion<T>::Quaternion(const Angle<Unit, T>& angle)
	{
		Set(angle);
	}

	/*!
	* \brief Constructs a Quaternion object from a EulerAngles
	*
	* \param angles Easier representation of rotation of space
	*
	* \see EulerAngles
	*/
	template<typename T>
	Quaternion<T>::Quaternion(const EulerAngles<T>& angles)
	{
		Set(angles);
	}

	/*!
	* \brief Constructs a Quaternion object from an angle and a direction
	*
	* \param angle Angle to rotate along the axis
	* \param axis Vector3 which represents a direction, no need to be normalized
	*/

	template<typename T>
	Quaternion<T>::Quaternion(RadianAngle<T> angle, const Vector3<T>& axis)
	{
		Set(angle, axis);
	}

	/*!
	* \brief Constructs a Quaternion object from an array of four elements
	*
	* \param quat[4] quat[0] is W component, quat[1] is X component, quat[2] is Y component and quat[3] is Z component
	*/

	template<typename T>
	Quaternion<T>::Quaternion(const T quat[4])
	{
		Set(quat);
	}

	/*
	template<typename T>
	Quaternion<T>::Quaternion(const Matrix3<T>& mat)
	{
		Set(mat);
	}
	*/

	/*!
	* \brief Constructs a Quaternion object from another type of Quaternion
	*
	* \param quat Quaternion of type U to convert to type T
	*/

	template<typename T>
	template<typename U>
	Quaternion<T>::Quaternion(const Quaternion<U>& quat)
	{
		Set(quat);
	}

	/*!
	* \brief Computes the w component of the quaternion to make it unit
	* \return A reference to this quaternion
	*/

	template<typename T>
	Quaternion<T>& Quaternion<T>::ComputeW()
	{
		T t = T(1.0) - SquaredMagnitude();

		if (t < T(0.0))
			w = T(0.0);
		else
			w = -std::sqrt(t);

		return *this;
	}

	/*!
	* \brief Returns the rotational conjugate of this quaternion
	* \return A reference to this quaternion
	*
	* The conjugate of a quaternion represents the same rotation in the opposite direction about the rotational axis
	*
	* \see GetConjugate
	*/

	template<typename T>
	Quaternion<T>& Quaternion<T>::Conjugate()
	{
		x = -x;
		y = -y;
		z = -z;

		return *this;
	}

	/*!
	* \brief Calculates the dot (scalar) product with two quaternions
	* \return The value of the dot product
	*
	* \param quat The other quaternion to calculate the dot product with
	*/

	template<typename T>
	T Quaternion<T>::DotProduct(const Quaternion& quat) const
	{
		return w * quat.w + x * quat.x + y * quat.y + z * quat.z;
	}

	/*!
	* \brief Gets the rotational conjugate of this quaternion
	* \return A new quaternion which is the conjugate of this quaternion
	*
	* The conjugate of a quaternion represents the same rotation in the opposite direction about the rotational axis
	*
	* \see Conjugate
	*/

	template<typename T>
	Quaternion<T> Quaternion<T>::GetConjugate() const
	{
		Quaternion<T> quat(*this);
		quat.Conjugate();

		return quat;
	}

	/*!
	* \brief Gets the inverse of this quaternion
	* \return A new quaternion which is the inverse of this quaternion
	*
	* \remark If this quaternion is (0, 0, 0, 0), then it returns (0, 0, 0, 0)
	*
	* \see Inverse
	*/

	template<typename T>
	Quaternion<T> Quaternion<T>::GetInverse() const
	{
		Quaternion<T> quat(*this);
		quat.Inverse();

		return quat;
	}

	/*!
	* \brief Gets the normalization of this quaternion
	* \return A new quaternion which is the normalization of this quaternion
	*
	* \param length Optional argument to obtain the length's ratio of the quaternion and the unit-length
	*
	* \remark If this quaternion is (0, 0, 0, 0), then it returns (0, 0, 0, 0) and length is 0
	*
	* \see Normalize
	*/

	template<typename T>
	Quaternion<T> Quaternion<T>::GetNormal(T* length) const
	{
		Quaternion<T> quat(*this);
		quat.Normalize(length);

		return quat;
	}

	/*!
	* \brief Inverts this quaternion
	* \return A reference to this quaternion which is now inverted
	*
	* \remark If this quaternion is (0, 0, 0, 0), then it returns (0, 0, 0, 0)
	*
	* \see GetInverse
	*/

	template<typename T>
	Quaternion<T>& Quaternion<T>::Inverse()
	{
		T norm = SquaredMagnitude();
		if (norm > T(0.0))
		{
			T invNorm = T(1.0) / std::sqrt(norm);

			w *= invNorm;
			x *= -invNorm;
			y *= -invNorm;
			z *= -invNorm;
		}

		return *this;
	}

	/*!
	* \brief Makes the quaternion (1, 0, 0, 0)
	* \return A reference to this vector with components (1, 0, 0, 0)
	*
	* \see Unit
	*/

	template<typename T>
	Quaternion<T>& Quaternion<T>::MakeIdentity()
	{
		return Set(T(1.0), T(0.0), T(0.0), T(0.0));
	}

	/*!
	* \brief Makes this quaternion to the rotation required to rotate direction Vector3 from to direction Vector3 to
	* \return A reference to this vector which is the rotation needed
	*
	* \param from Initial vector
	* \param to Target vector
	*
	* \remark Vectors are not required to be normalized
	*
	* \see RotationBetween
	*/

	template<typename T>
	Quaternion<T>& Quaternion<T>::MakeRotationBetween(const Vector3<T>& from, const Vector3<T>& to)
	{
		// Based on: http://lolengine.net/blog/2013/09/18/beautiful-maths-quaternion-from-vectors
		T norm = std::sqrt(from.GetSquaredLength() * to.GetSquaredLength());
		Vector3<T> crossProduct = from.CrossProduct(to);
		Set(norm + from.DotProduct(to), crossProduct.x, crossProduct.y, crossProduct.z);
		return Normalize();
	}

	/*!
	* \brief Makes the quaternion (0, 0, 0, 0)
	* \return A reference to this vector with components (0, 0, 0, 0)
	*
	* \see Zero
	*/

	template<typename T>
	Quaternion<T>& Quaternion<T>::MakeZero()
	{
		return Set(T(0.0), T(0.0), T(0.0), T(0.0));
	}

	/*!
	* \brief Calculates the magnitude (length) of the quaternion
	* \return The magnitude
	*
	* \see SquaredMagnitude
	*/

	template<typename T>
	T Quaternion<T>::Magnitude() const
	{
		return std::sqrt(SquaredMagnitude());
	}

	/*!
	* \brief Normalizes the current quaternion
	* \return A reference to this quaternion which is now normalized
	*
	* \param length Optional argument to obtain the length's ratio of the quaternion and the unit-length
	*
	* \remark If the quaternion is (0, 0, 0, 0), then it returns (0, 0, 0, 0) and length is 0
	*
	* \see GetNormal
	*/

	template<typename T>
	Quaternion<T>& Quaternion<T>::Normalize(T* length)
	{
		T norm = std::sqrt(SquaredMagnitude());
		if (norm > T(0.0))
		{
			T invNorm = T(1.0) / norm;
			w *= invNorm;
			x *= invNorm;
			y *= invNorm;
			z *= invNorm;
		}

		if (length)
			*length = norm;

		return *this;
	}

	/*!
	* \brief Sets the components of the quaternion
	* \return A reference to this quaternion
	*
	* \param W W component
	* \param X X component
	* \param Y Y component
	* \param Z Z component
	*/

	template<typename T>
	Quaternion<T>& Quaternion<T>::Set(T W, T X, T Y, T Z)
	{
		w = W;
		x = X;
		y = Y;
		z = Z;

		return *this;
	}

	/*!
	* \brief Sets this quaternion from a 2D rotation specified by an Angle
	* \return A reference to this quaternion
	*
	* \param angle 2D angle
	*
	* \see Angle
	*/
	template<typename T>
	template<AngleUnit Unit>
	Quaternion<T>& Quaternion<T>::Set(const Angle<Unit, T>& angle)
	{
		return Set(angle.ToQuaternion());
	}

	/*!
	* \brief Sets this quaternion from rotation specified by Euler angle
	* \return A reference to this quaternion
	*
	* \param angles Easier representation of rotation of space
	*
	* \see EulerAngles
	*/
	template<typename T>
	Quaternion<T>& Quaternion<T>::Set(const EulerAngles<T>& angles)
	{
		return Set(angles.ToQuaternion());
	}

	/*!
	* \brief Sets this quaternion from rotation specified by axis and angle
	* \return A reference to this quaternion
	*
	* \param angle Angle to rotate along the axis
	* \param axis Vector3 which represents a direction, no need to be normalized
	*/

	template<typename T>
	Quaternion<T>& Quaternion<T>::Set(RadianAngle<T> angle, const Vector3<T>& axis)
	{
		angle /= T(2.0);

		Vector3<T> normalizedAxis = axis.GetNormal();

		auto sincos = angle.GetSinCos();

		w = sincos.second;
		x = normalizedAxis.x * sincos.first;
		y = normalizedAxis.y * sincos.first;
		z = normalizedAxis.z * sincos.first;

		return Normalize();
	}

	/*!
	* \brief Sets the components of the quaternion from an array of four elements
	* \return A reference to this quaternion
	*
	* \param quat[4] quat[0] is W component, quat[1] is X component, quat[2] is Y component and quat[3] is Z component
	*/

	template<typename T>
	Quaternion<T>& Quaternion<T>::Set(const T quat[4])
	{
		w = quat[0];
		x = quat[1];
		y = quat[2];
		z = quat[3];

		return *this;
	}

	/*!
	* \brief Sets the components of the quaternion from another type of Quaternion
	* \return A reference to this quaternion
	*
	* \param quat Quaternion of type U to convert its components
	*/

	template<typename T>
	template<typename U>
	Quaternion<T>& Quaternion<T>::Set(const Quaternion<U>& quat)
	{
		w = T(quat.w);
		x = T(quat.x);
		y = T(quat.y);
		z = T(quat.z);

		return *this;
	}

	/*!
	* \brief Calculates the squared magnitude (length) of the quaternion
	* \return The squared magnitude
	*
	* \see Magnitude
	*/

	template<typename T>
	T Quaternion<T>::SquaredMagnitude() const
	{
		return w * w + x * x + y * y + z * z;
	}

	/*!
	* \brief Returns the "roll angle" of this quaternion
	* \return Roll rotation
	*
	* \remark This function only has sense when quaternion only represents a "roll rotation"
	*/
	template<typename T>
	RadianAngle<T> Quaternion<T>::To2DAngle() const
	{
		T siny_cosp = T(2.0) * (w * z + x * y);
		T cosy_cosp = T(1.0) - T(2.0) * (y * y + z * z);

		return std::atan2(siny_cosp, cosy_cosp);
	}

	/*!
	* \brief Converts this quaternion to Euler angles representation
	* \return EulerAngles which is the representation of this rotation
	*
	* \remark Rotation are "left-handed"
	*/
	template<typename T>
	EulerAngles<T> Quaternion<T>::ToEulerAngles() const
	{
		T test = x * y + z * w;
		if (test > T(0.499))
			// singularity at north pole
			return EulerAngles<T>(DegreeAngle<T>(T(0.0)), RadianAngle<T>(T(2.0) * std::atan2(x, w)), DegreeAngle<T>(T(90.0)));

		if (test < T(-0.499))
			// singularity at south pole
			return EulerAngles<T>(DegreeAngle<T>(T(0.0)), RadianAngle<T>(T(-2.0) * std::atan2(x, w)), DegreeAngle<T>(T(-90.0)));

		return EulerAngles<T>(RadianAngle<T>(std::atan2(T(2.0) * x * w - T(2.0) * y * z, T(1.0) - T(2.0) * x * x - T(2.0) * z * z)),
		                      RadianAngle<T>(std::atan2(T(2.0) * y * w - T(2.0) * x * z, T(1.0) - T(2.0) * y * y - T(2.0) * z * z)),
		                      RadianAngle<T>(std::asin(T(2.0) * test)));
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object: "Quaternion(w | x, y, z)"
	*/

	template<typename T>
	std::string Quaternion<T>::ToString() const
	{
		std::ostringstream ss;
		ss << *this;

		return ss.str();
	}

	/*!
	* \brief Adds the components of the quaternion with other quaternion
	* \return A quaternion where components are the sum of this quaternion and the other one
	*
	* \param quat The other quaternion to add components with
	*/

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

	/*!
	* \brief Multiplies of the quaternion with other quaternion
	* \return A quaternion which is the product of those two according to operator* in quaternions
	*
	* \param quat The other quaternion to multiply with
	*/

	template<typename T>
	Quaternion<T> Quaternion<T>::operator*(const Quaternion& quat) const
	{
		Quaternion result;
		result.w = w * quat.w - x * quat.x - y * quat.y - z * quat.z;
		result.x = w * quat.x + x * quat.w + y * quat.z - z * quat.y;
		result.y = w * quat.y + y * quat.w + z * quat.x - x * quat.z;
		result.z = w * quat.z + z * quat.w + x * quat.y - y * quat.x;

		return result;
	}

	/*!
	* \brief Apply the quaternion to the Vector3
	* \return A Vector3f which is the vector rotated by this quaternion
	*
	* \param vec The vector to multiply with
	*/

	template<typename T>
	Vector3<T> Quaternion<T>::operator*(const Vector3<T>& vec) const
	{
		Vector3<T> quatVec(x, y, z);
		Vector3<T> uv = quatVec.CrossProduct(vec);
		Vector3<T> uuv = quatVec.CrossProduct(uv);
		uv *= T(2.0) * w;
		uuv *= T(2.0);

		return vec + uv + uuv;
	}

	/*!
	* \brief Multiplies the components of the quaternion with a scalar
	* \return A quaternion where components are the product of this quaternion and the scalar
	*
	* \param scale The scalar to multiply components with
	*/

	template<typename T>
	Quaternion<T> Quaternion<T>::operator*(T scale) const
	{
		return Quaternion(w * scale,
		                  x * scale,
		                  y * scale,
		                  z * scale);
	}

	/*!
	* \brief Divides the quaternion with other quaternion
	* \return A quaternion which is the quotient of those two according to operator* in quaternions
	*
	* \param quat The other quaternion to divide with
	*/

	template<typename T>
	Quaternion<T> Quaternion<T>::operator/(const Quaternion& quat) const
	{
		return quat.GetConjugate() * (*this);
	}

	/*!
	* \brief Adds the components of the quaternion with other quaternion
	* \return A reference to this quaternion where components are the sum of this quaternion and the other one
	*
	* \param quat The other quaternion to add components with
	*/

	template<typename T>
	Quaternion<T>& Quaternion<T>::operator+=(const Quaternion& quat)
	{
		return operator=(operator+(quat));
	}

	/*!
	* \brief Multiplies of the quaternion with other quaternion
	* \return A reference to this quaternion which is the product of those two according to operator* in quaternions
	*
	* \param quat The other quaternion to multiply with
	*/

	template<typename T>
	Quaternion<T>& Quaternion<T>::operator*=(const Quaternion& quat)
	{
		return operator=(operator*(quat));
	}

	/*!
	* \brief Multiplies the components of the quaternion with a scalar
	* \return A reference to this quaternion where components are the product of this quaternion and the scalar
	*
	* \param scale The scalar to multiply components with
	*/

	template<typename T>
	Quaternion<T>& Quaternion<T>::operator*=(T scale)
	{
		return operator=(operator*(scale));
	}

	/*!
	* \brief Divides the quaternion with other quaternion
	* \return A reference to this quaternion which is the quotient of those two according to operator* in quaternions
	*
	* \param quat The other quaternion to divide with
	*/

	template<typename T>
	Quaternion<T>& Quaternion<T>::operator/=(const Quaternion& quat)
	{
		return operator=(operator/(quat));
	}

	/*!
	* \brief Compares the quaternion to other one
	* \return true if the quaternions are the same
	*
	* \param quat Other quaternion to compare with
	*/

	template<typename T>
	bool Quaternion<T>::operator==(const Quaternion& quat) const
	{
		return NumberEquals(w, quat.w) &&
		       NumberEquals(x, quat.x) &&
		       NumberEquals(y, quat.y) &&
		       NumberEquals(z, quat.z);
	}

	/*!
	* \brief Compares the quaternion to other one
	* \return false if the quaternions are the same
	*
	* \param quat Other quaternion to compare with
	*/

	template<typename T>
	bool Quaternion<T>::operator!=(const Quaternion& quat) const
	{
		return !operator==(quat);
	}

	/*!
	* \brief Shorthand for the quaternion (1, 0, 0, 0)
	* \return A quaternion with components (1, 0, 0, 0)
	*
	* \see MakeIdentity
	*/

	template<typename T>
	Quaternion<T> Quaternion<T>::Identity()
	{
		Quaternion quaternion;
		quaternion.MakeIdentity();

		return quaternion;
	}

	/*!
	* \brief Interpolates the quaternion to other one with a factor of interpolation
	* \return A new quaternion which is the interpolation of two quaternions
	*
	* \param from Initial quaternion
	* \param to Target quaternion
	* \param interpolation Factor of interpolation
	*
	* \remark interpolation is meant to be between 0 and 1, other values are potentially undefined behavior
	* \remark With NAZARA_DEBUG, a NazaraError is thrown and Zero() is returned
	*
	* \see Lerp, Slerp
	*/

	template<typename T>
	Quaternion<T> Quaternion<T>::Lerp(const Quaternion& from, const Quaternion& to, T interpolation)
	{
		#ifdef NAZARA_DEBUG
		if (interpolation < T(0.0) || interpolation > T(1.0))
		{
			NazaraError("Interpolation must be in range [0..1] (Got " + NumberToString(interpolation) + ')');
			return Zero();
		}
		#endif

		Quaternion interpolated;
		interpolated.w = Nz::Lerp(from.w, to.w, interpolation);
		interpolated.x = Nz::Lerp(from.x, to.x, interpolation);
		interpolated.y = Nz::Lerp(from.y, to.y, interpolation);
		interpolated.z = Nz::Lerp(from.z, to.z, interpolation);

		return interpolated;
	}

	/*!
	* \brief Gives the normalized quaternion
	* \return A normalized quaternion from the quat
	*
	* \param quat Quaternion to normalize
	* \param length Optional argument to obtain the length's ratio of the vector and the unit-length
	*
	* \see GetNormal
	*/

	template<typename T>
	Quaternion<T> Quaternion<T>::Normalize(const Quaternion& quat, T* length)
	{
		return quat.GetNormal(length);
	}

	/*!
	* \brief Gets the rotation required to rotate direction Vector3 from to direction Vector3 to
	* \return A quaternion which is the rotation needed between those two Vector3
	*
	* \param from Initial vector
	* \param to Target vector
	*
	* \see MakeRotationBetween
	*/

	template<typename T>
	Quaternion<T> Quaternion<T>::RotationBetween(const Vector3<T>& from, const Vector3<T>& to)
	{
		Quaternion quaternion;
		quaternion.MakeRotationBetween(from, to);

		return quaternion;
	}

	/*!
	* \brief Interpolates spherically the quaternion to other one with a factor of interpolation
	* \return A new quaternion which is the interpolation of two quaternions
	*
	* \param from Initial quaternion
	* \param to Target quaternion
	* \param interpolation Factor of interpolation
	*
	* \remark interpolation is meant to be between 0 and 1, other values are potentially undefined behavior
	* \remark With NAZARA_DEBUG, a NazaraError is thrown and Zero() is returned
	*
	* \see Lerp
	*/

	template<typename T>
	Quaternion<T> Quaternion<T>::Slerp(const Quaternion& from, const Quaternion& to, T interpolation)
	{
		#ifdef NAZARA_DEBUG
		if (interpolation < T(0.0) || interpolation > T(1.0))
		{
			NazaraError("Interpolation must be in range [0..1] (Got " + NumberToString(interpolation) + ')');
			return Zero();
		}
		#endif

		Quaternion q;

		T cosOmega = from.DotProduct(to);
		if (cosOmega < T(0.0))
		{
			// We invert everything
			q.Set(-to.w, -to.x, -to.y, -to.z);
			cosOmega = -cosOmega;
		}
		else
			q.Set(to);

		T k0, k1;
		if (cosOmega > T(0.9999))
		{
			// Linear interpolation to avoid division by zero
			k0 = T(1.0) - interpolation;
			k1 = interpolation;
		}
		else
		{
			T sinOmega = std::sqrt(T(1.0) - cosOmega*cosOmega);
			T omega = std::atan2(sinOmega, cosOmega);

			// To avoid two divisions
			sinOmega = T(1.0)/sinOmega;

			k0 = std::sin((T(1.0) - interpolation) * omega) * sinOmega;
			k1 = std::sin(interpolation*omega) * sinOmega;
		}

		Quaternion result(k0 * from.w, k0 * from.x, k0 * from.y, k0 * from.z);
		return result += q * k1;
	}

	/*!
	* \brief Shorthand for the quaternion (0, 0, 0, 0)
	* \return A quaternion with components (0, 0, 0, 0)
	*
	* \see MakeZero
	*/

	template<typename T>
	Quaternion<T> Quaternion<T>::Zero()
	{
		Quaternion quaternion;
		quaternion.MakeZero();

		return quaternion;
	}


	/*!
	* \brief Serializes a Quaternion
	* \return true if successfully serialized
	*
	* \param context Serialization context
	* \param quat Input Quaternion
	*/
	template<typename T>
	bool Serialize(SerializationContext& context, const Quaternion<T>& quat, TypeTag<Quaternion<T>>)
	{
		if (!Serialize(context, quat.x))
			return false;

		if (!Serialize(context, quat.y))
			return false;

		if (!Serialize(context, quat.z))
			return false;

		if (!Serialize(context, quat.w))
			return false;

		return true;
	}

	/*!
	* \brief Unserializes a Quaternion
	* \return true if successfully unserialized
	*
	* \param context Serialization context
	* \param quat Output Quaternion
	*/
	template<typename T>
	bool Unserialize(SerializationContext& context, Quaternion<T>* quat, TypeTag<Quaternion<T>>)
	{
		if (!Unserialize(context, &quat->x))
			return false;

		if (!Unserialize(context, &quat->y))
			return false;

		if (!Unserialize(context, &quat->z))
			return false;

		if (!Unserialize(context, &quat->w))
			return false;

		return true;
	}
}

/*!
* \brief Output operator
* \return The stream
*
* \param out The stream
* \param quat The quaternion to output
*/

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nz::Quaternion<T>& quat)
{
	return out << "Quaternion(" << quat.w << " | " << quat.x << ", " << quat.y << ", " << quat.z << ')';
}

#include <Nazara/Core/DebugOff.hpp>
