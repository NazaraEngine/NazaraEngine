// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Math/Basic.hpp>
#include <Nazara/Math/Config.hpp>
#include <Nazara/Math/EulerAngles.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)

template<typename T>
NzMatrix4<T>::NzMatrix4(T r11, T r12, T r13, T r14,
                        T r21, T r22, T r23, T r24,
                        T r31, T r32, T r33, T r34,
                        T r41, T r42, T r43, T r44)
{
	Set(r11, r12, r13, r14,
	    r21, r22, r23, r24,
	    r31, r32, r33, r34,
	    r41, r42, r43, r44);
}

template<typename T>
NzMatrix4<T>::NzMatrix4(const T matrix[16])
{
	Set(matrix);
}

template<typename T>
template<typename U>
NzMatrix4<T>::NzMatrix4(const NzMatrix4<U>& matrix)
{
	Set(matrix);
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::ApplyRotation(const NzQuaternion<T>& rotation)
{
	return Concatenate(NzMatrix4<T>::Rotate(rotation));
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::ApplyScale(const NzVector3<T>& scale)
{
	m11 *= scale.x;
	m12 *= scale.x;
	m13 *= scale.x;

	m21 *= scale.y;
	m22 *= scale.y;
	m23 *= scale.y;

	m31 *= scale.z;
	m32 *= scale.z;
	m33 *= scale.z;

	return *this;
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::ApplyTranslation(const NzVector3<T>& translation)
{
	m41 += translation.x;
	m42 += translation.y;
	m43 += translation.z;

	return *this;
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::Concatenate(const NzMatrix4& matrix)
{
	#if NAZARA_MATH_MATRIX4_CHECK_AFFINE
	if (IsAffine() && matrix.IsAffine())
		return ConcatenateAffine(matrix);
	#endif

	return Set(m11*matrix.m11 + m12*matrix.m21 + m13*matrix.m31 + m14*matrix.m41,
	           m11*matrix.m12 + m12*matrix.m22 + m13*matrix.m32 + m14*matrix.m42,
	           m11*matrix.m13 + m12*matrix.m23 + m13*matrix.m33 + m14*matrix.m43,
	           m11*matrix.m14 + m12*matrix.m24 + m13*matrix.m34 + m14*matrix.m44,

	           m21*matrix.m11 + m22*matrix.m21 + m23*matrix.m31 + m24*matrix.m41,
	           m21*matrix.m12 + m22*matrix.m22 + m23*matrix.m32 + m24*matrix.m42,
	           m21*matrix.m13 + m22*matrix.m23 + m23*matrix.m33 + m24*matrix.m43,
	           m21*matrix.m14 + m22*matrix.m24 + m23*matrix.m34 + m24*matrix.m44,

	           m31*matrix.m11 + m32*matrix.m21 + m33*matrix.m31 + m34*matrix.m41,
	           m31*matrix.m12 + m32*matrix.m22 + m33*matrix.m32 + m34*matrix.m42,
	           m31*matrix.m13 + m32*matrix.m23 + m33*matrix.m33 + m34*matrix.m43,
	           m31*matrix.m14 + m32*matrix.m24 + m33*matrix.m34 + m34*matrix.m44,

	           m41*matrix.m11 + m42*matrix.m21 + m43*matrix.m31 + m44*matrix.m41,
	           m41*matrix.m12 + m42*matrix.m22 + m43*matrix.m32 + m44*matrix.m42,
	           m41*matrix.m13 + m42*matrix.m23 + m43*matrix.m33 + m44*matrix.m43,
	           m41*matrix.m14 + m42*matrix.m24 + m43*matrix.m34 + m44*matrix.m44);
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::ConcatenateAffine(const NzMatrix4& matrix)
{
	#ifdef NAZARA_DEBUG
	if (!IsAffine())
	{
		NazaraWarning("First matrix not affine");
		return Concatenate(matrix);
	}

	if (!matrix.IsAffine())
	{
		NazaraWarning("Second matrix not affine");
		return Concatenate(matrix);
	}
	#endif

	return Set(m11*matrix.m11 + m12*matrix.m21 + m13*matrix.m31,
	           m11*matrix.m12 + m12*matrix.m22 + m13*matrix.m32,
	           m11*matrix.m13 + m12*matrix.m23 + m13*matrix.m33,
	           F(0.0),

	           m21*matrix.m11 + m22*matrix.m21 + m23*matrix.m31,
	           m21*matrix.m12 + m22*matrix.m22 + m23*matrix.m32,
	           m21*matrix.m13 + m22*matrix.m23 + m23*matrix.m33,
	           F(0.0),

	           m31*matrix.m11 + m32*matrix.m21 + m33*matrix.m31,
	           m31*matrix.m12 + m32*matrix.m22 + m33*matrix.m32,
	           m31*matrix.m13 + m32*matrix.m23 + m33*matrix.m33,
	           F(0.0),

	           m41*matrix.m11 + m42*matrix.m21 + m43*matrix.m31 + matrix.m41,
	           m41*matrix.m12 + m42*matrix.m22 + m43*matrix.m32 + matrix.m42,
	           m41*matrix.m13 + m42*matrix.m23 + m43*matrix.m33 + matrix.m43,
	           F(1.0));
}

template<typename T>
T NzMatrix4<T>::GetDeterminant() const
{
	T A = m22*(m33*m44 - m43*m34) - m32*(m23*m44 - m43*m24) + m42*(m23*m34 - m33*m24);
	T B = m12*(m33*m44 - m43*m34) - m32*(m13*m44 - m43*m14) + m42*(m13*m34 - m33*m14);
	T C = m12*(m23*m44 - m43*m24) - m22*(m13*m44 - m43*m14) + m42*(m13*m24 - m23*m14);
	T D = m12*(m23*m34 - m33*m24) - m22*(m13*m34 - m33*m14) + m32*(m13*m24 - m23*m14);

	return m11*A - m21*B + m31*C - m41*D;
}

template<typename T>
T NzMatrix4<T>::GetDeterminantAffine() const
{
	T A = m22*m33 - m32*m23;
	T B = m12*m33 - m32*m13;
	T C = m12*m23 - m22*m13;

	return m11*A - m21*B + m31*C;
}

template<typename T>
bool NzMatrix4<T>::GetInverse(NzMatrix4* dest) const
{
	///DOC: Il est possible d'appeler cette méthode avec la même matrice en argument qu'en appelant
	#ifdef NAZARA_DEBUG
	if (!dest)
	{
		NazaraError("Destination matrix must be valid");
		return false;
	}
	#endif

	T det = GetDeterminant();
	if (!NzNumberEquals(det, F(0.0)))
	{
		// http://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
		T inv[16];
		inv[0] = m22 * m33 * m44 -
				 m22 * m34 * m43 -
				 m32 * m23 * m44 +
				 m32 * m24 * m43 +
				 m42 * m23 * m34 -
				 m42 * m24 * m33;

		inv[1] = -m12 * m33 * m44 +
				  m12 * m34 * m43 +
				  m32 * m13 * m44 -
				  m32 * m14 * m43 -
				  m42 * m13 * m34 +
				  m42 * m14 * m33;

		inv[2] = m12 * m23 * m44 -
				 m12 * m24 * m43 -
				 m22 * m13 * m44 +
				 m22 * m14 * m43 +
				 m42 * m13 * m24 -
				 m42 * m14 * m23;

		inv[3] = -m12 * m23 * m34 +
				  m12 * m24 * m33 +
				  m22 * m13 * m34 -
				  m22 * m14 * m33 -
				  m32 * m13 * m24 +
				  m32 * m14 * m23;

		inv[4] = -m21 * m33 * m44 +
				  m21 * m34 * m43 +
				  m31 * m23 * m44 -
				  m31 * m24 * m43 -
				  m41 * m23 * m34 +
				  m41 * m24 * m33;

		inv[5] = m11 * m33 * m44 -
				 m11 * m34 * m43 -
				 m31 * m13 * m44 +
				 m31 * m14 * m43 +
				 m41 * m13 * m34 -
				 m41 * m14 * m33;

		inv[6] = -m11 * m23 * m44 +
				  m11 * m24 * m43 +
				  m21 * m13 * m44 -
				  m21 * m14 * m43 -
				  m41 * m13 * m24 +
				  m41 * m14 * m23;

		inv[7] = m11 * m23 * m34 -
				 m11 * m24 * m33 -
				 m21 * m13 * m34 +
				 m21 * m14 * m33 +
				 m31 * m13 * m24 -
				 m31 * m14 * m23;

		inv[8] = m21 * m32 * m44 -
				 m21 * m34 * m42 -
				 m31 * m22 * m44 +
				 m31 * m24 * m42 +
				 m41 * m22 * m34 -
				 m41 * m24 * m32;

		inv[9] = -m11 * m32 * m44 +
				  m11 * m34 * m42 +
				  m31 * m12 * m44 -
				  m31 * m14 * m42 -
				  m41 * m12 * m34 +
				  m41 * m14 * m32;

		inv[10] = m11 * m22 * m44 -
				  m11 * m24 * m42 -
				  m21 * m12 * m44 +
				  m21 * m14 * m42 +
				  m41 * m12 * m24 -
				  m41 * m14 * m22;

		inv[11] = -m11 * m22 * m34 +
				   m11 * m24 * m32 +
				   m21 * m12 * m34 -
				   m21 * m14 * m32 -
				   m31 * m12 * m24 +
				   m31 * m14 * m22;

		inv[12] = -m21 * m32 * m43 +
				   m21 * m33 * m42 +
				   m31 * m22 * m43 -
				   m31 * m23 * m42 -
				   m41 * m22 * m33 +
				   m41 * m23 * m32;

		inv[13] = m11 * m32 * m43 -
				  m11 * m33 * m42 -
				  m31 * m12 * m43 +
				  m31 * m13 * m42 +
				  m41 * m12 * m33 -
				  m41 * m13 * m32;

		inv[14] = -m11 * m22 * m43 +
				   m11 * m23 * m42 +
				   m21 * m12 * m43 -
				   m21 * m13 * m42 -
				   m41 * m12 * m23 +
				   m41 * m13 * m22;

		inv[15] = m11 * m22 * m33 -
				  m11 * m23 * m32 -
				  m21 * m12 * m33 +
				  m21 * m13 * m32 +
				  m31 * m12 * m23 -
				  m31 * m13 * m22;

		T invDet = F(1.0) / det;
		for (unsigned int i = 0; i < 16; ++i)
			inv[i] *= invDet;

		dest->Set(inv);
		return true;
	}
	else
		return false;
}

template<typename T>
bool NzMatrix4<T>::GetInverseAffine(NzMatrix4* dest) const
{
	///DOC: Il est possible d'appeler cette méthode avec la même matrice en argument qu'en appelant
	#if NAZARA_MATH_SAFE
	if (!IsAffine())
	{
		NazaraError("Matrix is not affine");
		return false;
	}

	if (!dest)
	{
		NazaraError("Destination matrix must be valid");
		return false;
	}
	#endif

	T det = GetDeterminantAffine();
	if (!NzNumberEquals(det, F(0.0)))
	{
		// http://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
		T inv[16];
		inv[0] = m22 * m33 -
				 m32 * m23;

		inv[1] = -m12 * m33 +
				  m32 * m13;

		inv[2] = m12 * m23 -
				 m22 * m13;

		inv[3] = F(0.0);

		inv[4] = -m21 * m33 +
				  m31 * m23;

		inv[5] = m11 * m33 -
				 m31 * m13;

		inv[6] = -m11 * m23 +
				  m21 * m13;

		inv[7] = F(0.0);

		inv[8] = m21 * m32 -
				 m31 * m22;

		inv[9] = -m11 * m32 +
				  m31 * m12;

		inv[10] = m11 * m22 -
				  m21 * m12;

		inv[11] = F(0.0);

		inv[12] = -m21 * m32 * m43 +
				   m21 * m33 * m42 +
				   m31 * m22 * m43 -
				   m31 * m23 * m42 -
				   m41 * m22 * m33 +
				   m41 * m23 * m32;

		inv[13] = m11 * m32 * m43 -
				  m11 * m33 * m42 -
				  m31 * m12 * m43 +
				  m31 * m13 * m42 +
				  m41 * m12 * m33 -
				  m41 * m13 * m32;

		inv[14] = -m11 * m22 * m43 +
				   m11 * m23 * m42 +
				   m21 * m12 * m43 -
				   m21 * m13 * m42 -
				   m41 * m12 * m23 +
				   m41 * m13 * m22;

		T invDet = F(1.0) / det;
		for (unsigned int i = 0; i < 16; ++i)
			inv[i] *= invDet;

		inv[15] = F(1.0);

		dest->Set(inv);
		return true;
	}
	else
		return false;
}

template<typename T>
NzQuaternion<T> NzMatrix4<T>::GetRotation() const
{
	// http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
	NzQuaternion<T> quat;

	T trace = m11 + m22 + m33;
	if (trace > F(0.0))
	{
		T s = F(0.5)/std::sqrt(trace + F(1.0));
		quat.w = F(0.25) / s;
		quat.x = (m23 - m32) * s;
		quat.y = (m31 - m13) * s;
		quat.z = (m12 - m21) * s;
	}
	else
	{
		if (m11 > m22 && m11 > m33)
		{
			T s = F(2.0) * std::sqrt(F(1.0) + m11 - m22 - m33);

			quat.w = (m23 - m32) / s;
			quat.x = F(0.25) * s;
			quat.y = (m21 + m12) / s;
			quat.z = (m31 + m13) / s;
		}
		else if (m22 > m33)
		{
			T s = F(2.0) * std::sqrt(F(1.0) + m22 - m11 - m33);

			quat.w = (m31 - m13) / s;
			quat.x = (m21 + m12) / s;
			quat.y = F(0.25) * s;
			quat.z = (m32 + m23) / s;
		}
		else
		{
			T s = F(2.0) * std::sqrt(F(1.0) + m33 - m11 - m22);

			quat.w = (m12 - m21) / s;
			quat.x = (m31 + m13) / s;
			quat.y = (m32 + m23) / s;
			quat.z = F(0.25) * s;
		}
	}

	return quat;
}

template<typename T>
NzVector3<T> NzMatrix4<T>::GetScale() const
{
	return NzVector3<T>(std::sqrt(m11*m11 + m21*m21 + m31*m31),
						std::sqrt(m12*m12 + m22*m22 + m32*m32),
						std::sqrt(m13*m13 + m23*m23 + m33*m33));
}

template<typename T>
NzVector3<T> NzMatrix4<T>::GetTranslation() const
{
	return NzVector3<T>(m41, m42, m43);
}

template<typename T>
void NzMatrix4<T>::GetTransposed(NzMatrix4* dest) const
{
	dest->Set(m11, m21, m31, m41,
	          m12, m22, m32, m42,
	          m13, m23, m33, m43,
	          m14, m24, m34, m44);
}

template<typename T>
bool NzMatrix4<T>::HasNegativeScale() const
{
	return GetDeterminant() < F(0.0);
}

template<typename T>
bool NzMatrix4<T>::HasScale() const
{
	T t = m11*m11 + m21*m21 + m31*m31;
	if (!NzNumberEquals(t, F(1.0)))
		return true;

	t = m12*m12 + m22*m22 + m32*m32;
	if (!NzNumberEquals(t, F(1.0)))
		return true;

	t = m13*m13 + m23*m23 + m33*m33;
	if (!NzNumberEquals(t, F(1.0)))
		return true;

	return false;
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::Inverse(bool* succeeded)
{
	bool result = GetInverse(this);
	if (succeeded)
		*succeeded = result;

	return *this;
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::InverseAffine(bool* succeeded)
{
	bool result = GetInverseAffine(this);
	if (succeeded)
		*succeeded = result;

	return *this;
}

template<typename T>
bool NzMatrix4<T>::IsAffine() const
{
	return NzNumberEquals(m14, F(0.0)) &&
	       NzNumberEquals(m24, F(0.0)) &&
	       NzNumberEquals(m34, F(0.0)) &&
	       NzNumberEquals(m44, F(1.0));
}

template<typename T>
bool NzMatrix4<T>::IsIdentity() const
{
	return (NzNumberEquals(m11, F(1.0)) && NzNumberEquals(m12, F(0.0)) && NzNumberEquals(m13, F(0.0)) && NzNumberEquals(m14, F(0.0)) &&
	        NzNumberEquals(m11, F(0.0)) && NzNumberEquals(m12, F(1.0)) && NzNumberEquals(m13, F(0.0)) && NzNumberEquals(m14, F(0.0)) &&
	        NzNumberEquals(m11, F(0.0)) && NzNumberEquals(m12, F(0.0)) && NzNumberEquals(m13, F(1.0)) && NzNumberEquals(m14, F(0.0)) &&
	        NzNumberEquals(m11, F(0.0)) && NzNumberEquals(m12, F(0.0)) && NzNumberEquals(m13, F(0.0)) && NzNumberEquals(m14, F(1.0)));
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::MakeIdentity()
{
	Set(F(1.0), F(0.0), F(0.0), F(0.0),
		F(0.0), F(1.0), F(0.0), F(0.0),
		F(0.0), F(0.0), F(1.0), F(0.0),
		F(0.0), F(0.0), F(0.0), F(1.0));

	return *this;
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::MakeOrtho(T left, T right, T top, T bottom, T zNear, T zFar)
{
	// http://msdn.microsoft.com/en-us/library/windows/desktop/bb204942(v=vs.85).aspx
	Set(F(2.0) / (right - left), F(0.0), F(0.0), F(0.0),
	    F(0.0), F(2.0) / (top - bottom), F(0.0), F(0.0),
	    F(0.0), F(0.0), F(1.0) / (zNear - zFar), F(0.0),
	    (left + right) / (left - right), (top + bottom) / (bottom - top), zNear/(zNear - zFar), F(1.0));

	return *this;
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::MakeLookAt(const NzVector3<T>& eye, const NzVector3<T>& target, const NzVector3<T>& up)
{
	NzVector3<T> f = NzVector3<T>::Normalize(target - eye);
	NzVector3<T> u(up.GetNormal());
	NzVector3<T> s = NzVector3<T>::Normalize(f.CrossProduct(u));
	u = s.CrossProduct(f);

	Set(s.x, u.x, -f.x, T(0.0),
	    s.y, u.y, -f.y, T(0.0),
	    s.z, u.z, -f.z, T(0.0),
		-s.DotProduct(eye), -u.DotProduct(eye), f.DotProduct(eye), T(1.0));

	return *this;
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::MakePerspective(T angle, T ratio, T zNear, T zFar)
{
	// http://msdn.microsoft.com/en-us/library/windows/desktop/bb204945(v=vs.85).aspx
	#if NAZARA_MATH_ANGLE_RADIAN
	angle /= F(2.0);
	#else
	angle = NzDegreeToRadian(angle/F(2.0));
	#endif

	T yScale = F(1.0) / std::tan(angle);

	Set(yScale / ratio, F(0.0), F(0.0), F(0.0),
	    F(0.0), yScale, F(0.0), F(0.0),
	    F(0.0), F(0.0), zFar / (zNear-zFar), F(-1.0),
	    F(0.0), F(0.0), (zNear*zFar) / (zNear-zFar), F(0.0));

	return *this;
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::MakeRotation(const NzQuaternion<T>& rotation)
{
	SetRotation(rotation);

	// On complète la matrice
	m14 = F(0.0);
	m24 = F(0.0);
	m34 = F(0.0);
	m41 = F(0.0);
	m42 = F(0.0);
	m43 = F(0.0);
	m44 = F(1.0);

	return *this;
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::MakeScale(const NzVector3<T>& scale)
{
	Set(scale.x, F(0.0),  F(0.0),  F(0.0),
	    F(0.0),  scale.y, F(0.0),  F(0.0),
	    F(0.0),  F(0.0),  scale.z, F(0.0),
	    F(0.0),  F(0.0),  F(0.0),  F(1.0));

	return *this;
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::MakeTranslation(const NzVector3<T>& translation)
{
	Set(F(1.0), F(0.0), F(0.0), F(0.0),
	    F(0.0), F(1.0), F(0.0), F(0.0),
	    F(0.0), F(0.0), F(1.0), F(0.0),
	    translation.x, translation.y, translation.z, F(1.0));

	return *this;
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::MakeTransform(const NzVector3<T>& translation, const NzQuaternion<T>& rotation)
{
	// La rotation et la translation peuvent être appliquées directement
	SetRotation(rotation);
	SetTranslation(translation);

	// On complète la matrice (les transformations sont affines)
	m14 = F(0.0);
	m24 = F(0.0);
	m34 = F(0.0);
	m44 = F(1.0);

	// Pas besoin de mettre à jour l'identité (Déjà fait par Set*)

	return *this;
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::MakeTransform(const NzVector3<T>& translation, const NzQuaternion<T>& rotation, const NzVector3<T>& scale)
{
	MakeTransform(translation, rotation);

	// Ensuite on fait une mise à l'échelle des valeurs déjà présentes
	return ApplyScale(scale);
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::MakeViewMatrix(const NzVector3<T>& translation, const NzQuaternion<T>& rotation)
{
	// Une matrice de vue doit appliquer une transformation opposée à la matrice "monde"
	NzQuaternion<T> invRot = rotation.GetConjugate(); // Inverse de la rotation

	return MakeTransform(-(invRot*translation), invRot);
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::MakeZero()
{
	Set(F(0.0), F(0.0), F(0.0), F(0.0),
	    F(0.0), F(0.0), F(0.0), F(0.0),
	    F(0.0), F(0.0), F(0.0), F(0.0),
	    F(0.0), F(0.0), F(0.0), F(0.0));

	return *this;
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::Set(T r11, T r12, T r13, T r14,
                                T r21, T r22, T r23, T r24,
                                T r31, T r32, T r33, T r34,
                                T r41, T r42, T r43, T r44)
{
	m11 = r11;
	m12 = r12;
	m13 = r13;
	m14 = r14;
	m21 = r21;
	m22 = r22;
	m23 = r23;
	m24 = r24;
	m31 = r31;
	m32 = r32;
	m33 = r33;
	m34 = r34;
	m41 = r41;
	m42 = r42;
	m43 = r43;
	m44 = r44;

	return *this;
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::Set(const T matrix[16])
{
	// Ici nous sommes certains de la continuité des éléments en mémoire
	std::memcpy(&m11, matrix, 16*sizeof(T));

	return *this;
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::Set(const NzMatrix4& matrix)
{
	// Le membre isIdentity est copié en même temps que les valeurs
	std::memcpy(this, &matrix, sizeof(NzMatrix4));

	return *this;
}

template<typename T>
template<typename U>
NzMatrix4<T>& NzMatrix4<T>::Set(const NzMatrix4<U>& matrix)
{
	Set(F(matrix[ 0]), F(matrix[ 1]), F(matrix[ 2]), F(matrix[ 3]),
	    F(matrix[ 4]), F(matrix[ 5]), F(matrix[ 6]), F(matrix[ 7]),
	    F(matrix[ 8]), F(matrix[ 9]), F(matrix[10]), F(matrix[11]),
	    F(matrix[12]), F(matrix[13]), F(matrix[14]), F(matrix[15]));

	return *this;
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::SetRotation(const NzQuaternion<T>& rotation)
{
	T tx  = rotation.x + rotation.x;
	T ty  = rotation.y + rotation.y;
	T tz  = rotation.z + rotation.z;
	T twx = tx * rotation.w;
	T twy = ty * rotation.w;
	T twz = tz * rotation.w;
	T txx = tx * rotation.x;
	T txy = ty * rotation.x;
	T txz = tz * rotation.x;
	T tyy = ty * rotation.y;
	T tyz = tz * rotation.y;
	T tzz = tz * rotation.z;

	m11 = F(1.0) - (tyy + tzz);
	m12 = txy + twz;
	m13 = txz - twy;

	m21 = txy - twz;
	m22 = F(1.0) - (txx + tzz);
	m23 = tyz + twx;

	m31 = txz + twy;
	m32 = tyz - twx;
	m33 = F(1.0) - (txx + tyy);

	return *this;
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::SetScale(const NzVector3<T>& scale)
{
	m11 = scale.x;
	m22 = scale.y;
	m33 = scale.z;

	return *this;
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::SetTranslation(const NzVector3<T>& translation)
{
	m41 = translation.x;
	m42 = translation.y;
	m43 = translation.z;

	return *this;
}

template<typename T>
NzString NzMatrix4<T>::ToString() const
{
	NzStringStream ss;
	return ss << "Matrix4(" << m11 << ", " << m12 << ", " << m13 << ", " << m14 << ",\n"
	    	  << "        " << m21 << ", " << m22 << ", " << m23 << ", " << m24 << ",\n"
	    	  << "        " << m31 << ", " << m32 << ", " << m33 << ", " << m34 << ",\n"
	    	  << "        " << m41 << ", " << m42 << ", " << m43 << ", " << m44 << ')';
}

template<typename T>
NzVector2<T> NzMatrix4<T>::Transform(const NzVector2<T>& vector, T z, T w) const
{
	return NzVector2<T>(m11*vector.x + m21*vector.y + m31*z + m41*w,
	                    m12*vector.x + m22*vector.y + m32*z + m42*w);
}

template<typename T>
NzVector3<T> NzMatrix4<T>::Transform(const NzVector3<T>& vector, T w) const
{
	return NzVector3<T>(m11*vector.x + m21*vector.y + m31*vector.z + m41*w,
	                    m12*vector.x + m22*vector.y + m32*vector.z + m42*w,
	                    m13*vector.x + m23*vector.y + m33*vector.z + m43*w);
}

template<typename T>
NzVector4<T> NzMatrix4<T>::Transform(const NzVector4<T>& vector) const
{
	return NzVector4<T>(m11*vector.x + m21*vector.y + m31*vector.z + m41*vector.w,
	                    m12*vector.x + m22*vector.y + m32*vector.z + m42*vector.w,
	                    m13*vector.x + m23*vector.y + m33*vector.z + m43*vector.w,
	                    m14*vector.x + m24*vector.y + m34*vector.z + m44*vector.w);
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::Transpose()
{
	// N'affecte pas l'identité (La transposée d'une matrice identité est la matrice identité elle-même)
	std::swap(m12, m21);
	std::swap(m13, m31);
	std::swap(m14, m41);
	std::swap(m23, m32);
	std::swap(m24, m42);
	std::swap(m34, m43);

	return *this;
}

template<typename T>
NzMatrix4<T>::operator T*()
{
	return &m11;
}

template<typename T>
NzMatrix4<T>::operator const T*() const
{
	return &m11;
}

template<typename T>
T& NzMatrix4<T>::operator()(unsigned int x, unsigned int y)
{
	#if NAZARA_MATH_SAFE
	if (x > 3 || y > 3)
	{
		NzStringStream ss;
		ss << "Index out of range: (" << x << ", " << y << ") > (3,3)";

		throw std::out_of_range(ss.ToString());
	}
	#endif

	return (&m11)[y*4+x];
}

template<typename T>
T NzMatrix4<T>::operator()(unsigned int x, unsigned int y) const
{
	#if NAZARA_MATH_SAFE
	if (x > 3 || y > 3)
	{
		NzStringStream ss;
		ss << "Index out of range: (" << x << ", " << y << ") > (3,3)";

		NazaraError(ss);
		throw std::out_of_range(ss.ToString());
	}
	#endif

	return (&m11)[y*4+x];
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::operator*(const NzMatrix4& matrix) const
{
	NzMatrix4 result(*this);
	return result.Concatenate(matrix);
}

template<typename T>
NzVector2<T> NzMatrix4<T>::operator*(const NzVector2<T>& vector) const
{
	return Transform(vector);
}

template<typename T>
NzVector3<T> NzMatrix4<T>::operator*(const NzVector3<T>& vector) const
{
	return Transform(vector);
}

template<typename T>
NzVector4<T> NzMatrix4<T>::operator*(const NzVector4<T>& vector) const
{
	return Transform(vector);
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::operator*(T scalar) const
{
	NzMatrix4 mat;
	for (unsigned int i = 0; i < 16; ++i)
		mat[i] = (&m11)[i] * scalar;

	return mat;
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::operator*=(const NzMatrix4& matrix)
{
	Concatenate(matrix);

	return *this;
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::operator*=(T scalar)
{
	for (unsigned int i = 0; i < 16; ++i)
		(&m11)[i] *= scalar;

	return *this;
}

template<typename T>
bool NzMatrix4<T>::operator==(const NzMatrix4& mat) const
{
	for (unsigned int i = 0; i < 16; ++i)
		if (!NzNumberEquals((&m11)[i], (&mat.m11)[i]))
			return false;

	return true;
}

template<typename T>
bool NzMatrix4<T>::operator!=(const NzMatrix4& mat) const
{
	return !operator==(mat);
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::Concatenate(const NzMatrix4& left, const NzMatrix4& right)
{
	NzMatrix4 matrix(left); // Copie de la matrice de gauche
	matrix.Concatenate(right); // Concaténation avec la matrice de droite

	return matrix; // Et on renvoie la matrice
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::ConcatenateAffine(const NzMatrix4& left, const NzMatrix4& right)
{
	NzMatrix4 matrix(left); // Copie de la matrice de gauche
	matrix.ConcatenateAffine(right); // Concaténation (affine) avec la matrice de droite

	return matrix; // Et on renvoie la matrice
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::Identity()
{
	NzMatrix4 matrix;
	matrix.MakeIdentity();

	return matrix;
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::LookAt(const NzVector3<T>& eye, const NzVector3<T>& target, const NzVector3<T>& up)
{
	NzMatrix4 matrix;
	matrix.MakeLookAt(eye, target, up);

	return matrix;
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::Ortho(T left, T right, T top, T bottom, T zNear, T zFar)
{
	NzMatrix4 matrix;
	matrix.MakeOrtho(left, right, top, bottom, zNear, zFar);

	return matrix;
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::Perspective(T angle, T ratio, T zNear, T zFar)
{
	NzMatrix4 matrix;
	matrix.MakePerspective(angle, ratio, zNear, zFar);

	return matrix;
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::Rotate(const NzQuaternion<T>& rotation)
{
	NzMatrix4 matrix;
	matrix.MakeRotation(rotation);

	return matrix;
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::Scale(const NzVector3<T>& scale)
{
	NzMatrix4 matrix;
	matrix.MakeScale(scale);

	return matrix;
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::Translate(const NzVector3<T>& translation)
{
	NzMatrix4 mat;
	mat.MakeTranslation(translation);

	return mat;
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::Transform(const NzVector3<T>& translation, const NzQuaternion<T>& rotation)
{
	NzMatrix4 mat;
	mat.MakeTransform(translation, rotation);

	return mat;
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::Transform(const NzVector3<T>& translation, const NzQuaternion<T>& rotation, const NzVector3<T>& scale)
{
	NzMatrix4 mat;
	mat.MakeTransform(translation, rotation, scale);

	return mat;
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::ViewMatrix(const NzVector3<T>& translation, const NzQuaternion<T>& rotation)
{
	NzMatrix4 mat;
	mat.MakeViewMatrix(translation, rotation);

	return mat;
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::Zero()
{
	NzMatrix4 matrix;
	matrix.MakeZero();

	return matrix;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzMatrix4<T>& matrix)
{
	return out << matrix.ToString();
}

template<typename T>
NzMatrix4<T> operator*(T scale, const NzMatrix4<T>& matrix)
{
	return matrix * scale;
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
