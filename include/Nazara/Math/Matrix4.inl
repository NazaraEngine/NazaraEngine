// Copyright (C) 2012 Jérôme Leclercq
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
#include <cmath>
#include <cstring>
#include <limits>
#include <stdexcept>
//#include <Nazara/Core/Debug.hpp>
///FIXME: Le MLT détecte de faux-leaks ici (Problème lié aux inline ?)

#define F(a) static_cast<T>(a)

template<typename T>
NzMatrix4<T>::NzMatrix4()
{
}

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
NzMatrix4<T>::NzMatrix4(const NzMatrix4& matrix)
{
	Set(matrix);
}

template<typename T>
NzMatrix4<T>::NzMatrix4(NzMatrix4&& matrix) noexcept
{
	Set(matrix);
}

template<typename T>
NzMatrix4<T>::~NzMatrix4()
{
	ReleaseMatrix();
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::Concatenate(const NzMatrix4& matrix) const
{
	return Concatenate(*this, matrix);
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::ConcatenateAffine(const NzMatrix4& matrix) const
{
	return ConcatenateAffine(*this, matrix);
}

template<typename T>
T NzMatrix4<T>::GetDeterminant() const
{
	#if NAZARA_MATH_SAFE
	if (!IsDefined())
	{
		NazaraError("Matrix not defined");
		return F(0.0);
	}
	#endif

	T A = m_sharedMatrix->m22*(m_sharedMatrix->m33*m_sharedMatrix->m44 - m_sharedMatrix->m43*m_sharedMatrix->m34) - m_sharedMatrix->m32*(m_sharedMatrix->m23*m_sharedMatrix->m44 - m_sharedMatrix->m43*m_sharedMatrix->m24) + m_sharedMatrix->m42*(m_sharedMatrix->m23*m_sharedMatrix->m34 - m_sharedMatrix->m33*m_sharedMatrix->m24);
	T B = m_sharedMatrix->m12*(m_sharedMatrix->m33*m_sharedMatrix->m44 - m_sharedMatrix->m43*m_sharedMatrix->m34) - m_sharedMatrix->m32*(m_sharedMatrix->m13*m_sharedMatrix->m44 - m_sharedMatrix->m43*m_sharedMatrix->m14) + m_sharedMatrix->m42*(m_sharedMatrix->m13*m_sharedMatrix->m34 - m_sharedMatrix->m33*m_sharedMatrix->m14);
	T C = m_sharedMatrix->m12*(m_sharedMatrix->m23*m_sharedMatrix->m44 - m_sharedMatrix->m43*m_sharedMatrix->m24) - m_sharedMatrix->m22*(m_sharedMatrix->m13*m_sharedMatrix->m44 - m_sharedMatrix->m43*m_sharedMatrix->m14) + m_sharedMatrix->m42*(m_sharedMatrix->m13*m_sharedMatrix->m24 - m_sharedMatrix->m23*m_sharedMatrix->m14);
	T D = m_sharedMatrix->m12*(m_sharedMatrix->m23*m_sharedMatrix->m34 - m_sharedMatrix->m33*m_sharedMatrix->m24) - m_sharedMatrix->m22*(m_sharedMatrix->m13*m_sharedMatrix->m34 - m_sharedMatrix->m33*m_sharedMatrix->m14) + m_sharedMatrix->m32*(m_sharedMatrix->m13*m_sharedMatrix->m24 - m_sharedMatrix->m23*m_sharedMatrix->m14);

	return m_sharedMatrix->m11*A - m_sharedMatrix->m21*B + m_sharedMatrix->m31*C - m_sharedMatrix->m41*D;
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::GetInverse() const
{
	#if NAZARA_MATH_SAFE
	if (!IsDefined())
	{
		NazaraError("Matrix not defined");
		return NzMatrix4();
	}
	#endif

	T det = GetDeterminant();
	if (!NzNumberEquals(det, F(0.0)))
	{
		return NzMatrix4((m_sharedMatrix->m22*(m_sharedMatrix->m33*m_sharedMatrix->m44 - m_sharedMatrix->m34*m_sharedMatrix->m43) - m_sharedMatrix->m32*(m_sharedMatrix->m23*m_sharedMatrix->m44 - m_sharedMatrix->m43*m_sharedMatrix->m24) + m_sharedMatrix->m42*(m_sharedMatrix->m23*m_sharedMatrix->m34 - m_sharedMatrix->m33*m_sharedMatrix->m24)) / det,
						-(m_sharedMatrix->m12*(m_sharedMatrix->m33*m_sharedMatrix->m44 - m_sharedMatrix->m43*m_sharedMatrix->m34) - m_sharedMatrix->m32*(m_sharedMatrix->m13*m_sharedMatrix->m44 - m_sharedMatrix->m43*m_sharedMatrix->m14) + m_sharedMatrix->m42*(m_sharedMatrix->m13*m_sharedMatrix->m34 - m_sharedMatrix->m33*m_sharedMatrix->m14)) / det,
		                 (m_sharedMatrix->m12*(m_sharedMatrix->m23*m_sharedMatrix->m44 - m_sharedMatrix->m43*m_sharedMatrix->m24) - m_sharedMatrix->m22*(m_sharedMatrix->m13*m_sharedMatrix->m44 - m_sharedMatrix->m43*m_sharedMatrix->m14) + m_sharedMatrix->m42*(m_sharedMatrix->m13*m_sharedMatrix->m24 - m_sharedMatrix->m23*m_sharedMatrix->m14)) / det,
		                -(m_sharedMatrix->m12*(m_sharedMatrix->m23*m_sharedMatrix->m34 - m_sharedMatrix->m33*m_sharedMatrix->m24) - m_sharedMatrix->m22*(m_sharedMatrix->m13*m_sharedMatrix->m34 - m_sharedMatrix->m33*m_sharedMatrix->m14) + m_sharedMatrix->m32*(m_sharedMatrix->m13*m_sharedMatrix->m24 - m_sharedMatrix->m23*m_sharedMatrix->m14)) / det,

		                -(m_sharedMatrix->m21*(m_sharedMatrix->m33*m_sharedMatrix->m44 - m_sharedMatrix->m34*m_sharedMatrix->m43) - m_sharedMatrix->m23*(m_sharedMatrix->m31*m_sharedMatrix->m44 - m_sharedMatrix->m34*m_sharedMatrix->m41) + m_sharedMatrix->m24*(m_sharedMatrix->m31*m_sharedMatrix->m43 - m_sharedMatrix->m33*m_sharedMatrix->m41)) / det,
		                 (m_sharedMatrix->m11*(m_sharedMatrix->m33*m_sharedMatrix->m44 - m_sharedMatrix->m34*m_sharedMatrix->m43) - m_sharedMatrix->m13*(m_sharedMatrix->m31*m_sharedMatrix->m44 - m_sharedMatrix->m34*m_sharedMatrix->m41) + m_sharedMatrix->m14*(m_sharedMatrix->m31*m_sharedMatrix->m43 - m_sharedMatrix->m33*m_sharedMatrix->m41)) / det,
		                -(m_sharedMatrix->m11*(m_sharedMatrix->m23*m_sharedMatrix->m44 - m_sharedMatrix->m24*m_sharedMatrix->m43) - m_sharedMatrix->m13*(m_sharedMatrix->m21*m_sharedMatrix->m44 - m_sharedMatrix->m24*m_sharedMatrix->m41) + m_sharedMatrix->m14*(m_sharedMatrix->m21*m_sharedMatrix->m43 - m_sharedMatrix->m23*m_sharedMatrix->m41)) / det,
		                 (m_sharedMatrix->m11*(m_sharedMatrix->m23*m_sharedMatrix->m34 - m_sharedMatrix->m24*m_sharedMatrix->m33) - m_sharedMatrix->m13*(m_sharedMatrix->m21*m_sharedMatrix->m34 - m_sharedMatrix->m24*m_sharedMatrix->m31) + m_sharedMatrix->m14*(m_sharedMatrix->m21*m_sharedMatrix->m33 - m_sharedMatrix->m23*m_sharedMatrix->m31)) / det,

		                 (m_sharedMatrix->m21*(m_sharedMatrix->m32*m_sharedMatrix->m44 - m_sharedMatrix->m34*m_sharedMatrix->m42) - m_sharedMatrix->m22*(m_sharedMatrix->m31*m_sharedMatrix->m44 - m_sharedMatrix->m34*m_sharedMatrix->m41) + m_sharedMatrix->m24*(m_sharedMatrix->m31*m_sharedMatrix->m42 - m_sharedMatrix->m32*m_sharedMatrix->m41)) / det,
		                -(m_sharedMatrix->m11*(m_sharedMatrix->m32*m_sharedMatrix->m44 - m_sharedMatrix->m34*m_sharedMatrix->m42) - m_sharedMatrix->m12*(m_sharedMatrix->m31*m_sharedMatrix->m44 - m_sharedMatrix->m34*m_sharedMatrix->m41) + m_sharedMatrix->m14*(m_sharedMatrix->m31*m_sharedMatrix->m42 - m_sharedMatrix->m32*m_sharedMatrix->m41)) / det,
		                 (m_sharedMatrix->m11*(m_sharedMatrix->m22*m_sharedMatrix->m44 - m_sharedMatrix->m24*m_sharedMatrix->m42) - m_sharedMatrix->m12*(m_sharedMatrix->m21*m_sharedMatrix->m44 - m_sharedMatrix->m24*m_sharedMatrix->m41) + m_sharedMatrix->m14*(m_sharedMatrix->m21*m_sharedMatrix->m42 - m_sharedMatrix->m22*m_sharedMatrix->m41)) / det,
		                -(m_sharedMatrix->m11*(m_sharedMatrix->m22*m_sharedMatrix->m34 - m_sharedMatrix->m24*m_sharedMatrix->m32) - m_sharedMatrix->m12*(m_sharedMatrix->m21*m_sharedMatrix->m34 - m_sharedMatrix->m24*m_sharedMatrix->m31) + m_sharedMatrix->m14*(m_sharedMatrix->m21*m_sharedMatrix->m32 - m_sharedMatrix->m22*m_sharedMatrix->m31)) / det,

		                -(m_sharedMatrix->m21*(m_sharedMatrix->m32*m_sharedMatrix->m43 - m_sharedMatrix->m33*m_sharedMatrix->m42) - m_sharedMatrix->m22*(m_sharedMatrix->m31*m_sharedMatrix->m43 - m_sharedMatrix->m33*m_sharedMatrix->m41) + m_sharedMatrix->m23*(m_sharedMatrix->m31*m_sharedMatrix->m42 - m_sharedMatrix->m32*m_sharedMatrix->m41)) / det,
						 (m_sharedMatrix->m11*(m_sharedMatrix->m32*m_sharedMatrix->m43 - m_sharedMatrix->m33*m_sharedMatrix->m42) - m_sharedMatrix->m12*(m_sharedMatrix->m31*m_sharedMatrix->m43 - m_sharedMatrix->m33*m_sharedMatrix->m41) + m_sharedMatrix->m13*(m_sharedMatrix->m31*m_sharedMatrix->m42 - m_sharedMatrix->m32*m_sharedMatrix->m41)) / det,
		                -(m_sharedMatrix->m11*(m_sharedMatrix->m22*m_sharedMatrix->m43 - m_sharedMatrix->m23*m_sharedMatrix->m42) - m_sharedMatrix->m12*(m_sharedMatrix->m21*m_sharedMatrix->m43 - m_sharedMatrix->m23*m_sharedMatrix->m41) + m_sharedMatrix->m13*(m_sharedMatrix->m21*m_sharedMatrix->m42 - m_sharedMatrix->m22*m_sharedMatrix->m41)) / det,
						 (m_sharedMatrix->m11*(m_sharedMatrix->m22*m_sharedMatrix->m33 - m_sharedMatrix->m23*m_sharedMatrix->m32) - m_sharedMatrix->m12*(m_sharedMatrix->m21*m_sharedMatrix->m33 - m_sharedMatrix->m23*m_sharedMatrix->m31) + m_sharedMatrix->m13*(m_sharedMatrix->m21*m_sharedMatrix->m32 - m_sharedMatrix->m22*m_sharedMatrix->m31)) / det);
	}
	else
	{
		NazaraError("Matrix has no inverse");

		return Identity();
	}
}

template<typename T>
NzVector3<T> NzMatrix4<T>::GetScale() const
{
	#if NAZARA_MATH_SAFE
	if (!IsDefined())
	{
		NazaraError("Matrix not defined");
		return NzVector3<T>();
	}
	#endif

	return NzVector3<T>(std::sqrt(m_sharedMatrix->m11*m_sharedMatrix->m11 + m_sharedMatrix->m21*m_sharedMatrix->m21 + m_sharedMatrix->m31*m_sharedMatrix->m31),
						std::sqrt(m_sharedMatrix->m12*m_sharedMatrix->m12 + m_sharedMatrix->m22*m_sharedMatrix->m22 + m_sharedMatrix->m32*m_sharedMatrix->m32),
						std::sqrt(m_sharedMatrix->m13*m_sharedMatrix->m13 + m_sharedMatrix->m23*m_sharedMatrix->m23 + m_sharedMatrix->m33*m_sharedMatrix->m33));
}

template<typename T>
NzVector3<T> NzMatrix4<T>::GetTranslation() const
{
	#if NAZARA_MATH_SAFE
	if (!IsDefined())
	{
		NazaraError("Matrix not defined");
		return NzVector3<T>();
	}
	#endif

	return NzVector3<T>(m_sharedMatrix->m41, m_sharedMatrix->m42, m_sharedMatrix->m43);
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::GetTransposed() const
{
	#if NAZARA_MATH_SAFE
	if (!IsDefined())
	{
		NazaraError("Matrix not defined");
		return NzMatrix4();
	}
	#endif

	return NzMatrix4(m_sharedMatrix->m11, m_sharedMatrix->m21, m_sharedMatrix->m31, m_sharedMatrix->m41,
	                 m_sharedMatrix->m12, m_sharedMatrix->m22, m_sharedMatrix->m32, m_sharedMatrix->m42,
	                 m_sharedMatrix->m13, m_sharedMatrix->m23, m_sharedMatrix->m33, m_sharedMatrix->m43,
	                 m_sharedMatrix->m14, m_sharedMatrix->m24, m_sharedMatrix->m34, m_sharedMatrix->m44);
}

template<typename T>
bool NzMatrix4<T>::HasNegativeScale() const
{
	return GetDeterminant() < F(0.0);
}

template<typename T>
bool NzMatrix4<T>::HasScale() const
{
	#if NAZARA_MATH_SAFE
	if (!IsDefined())
	{
		NazaraError("Matrix not defined");
		return false;
	}
	#endif

	T t = m_sharedMatrix->m11*m_sharedMatrix->m11 + m_sharedMatrix->m21*m_sharedMatrix->m21 + m_sharedMatrix->m31*m_sharedMatrix->m31;
	if (!NzNumberEquals(t, F(1.0)))
		return true;

	t = m_sharedMatrix->m12*m_sharedMatrix->m12 + m_sharedMatrix->m22*m_sharedMatrix->m22 + m_sharedMatrix->m32*m_sharedMatrix->m32;
	if (!NzNumberEquals(t, F(1.0)))
		return true;

	t = m_sharedMatrix->m13*m_sharedMatrix->m13 + m_sharedMatrix->m23*m_sharedMatrix->m23 + m_sharedMatrix->m33*m_sharedMatrix->m33;
	if (!NzNumberEquals(t, F(1.0)))
		return true;

	return false;
}

template<typename T>
bool NzMatrix4<T>::IsAffine() const
{
	#if NAZARA_MATH_SAFE
	if (!IsDefined())
	{
		NazaraError("Matrix not defined");
		return false;
	}
	#endif

	return NzNumberEquals(m_sharedMatrix->m14, F(0.0)) &&
	       NzNumberEquals(m_sharedMatrix->m24, F(0.0)) &&
	       NzNumberEquals(m_sharedMatrix->m34, F(0.0)) &&
	       NzNumberEquals(m_sharedMatrix->m44, F(1.0));
}

template<typename T>
bool NzMatrix4<T>::IsDefined() const
{
	return m_sharedMatrix != nullptr;
}

template<typename T>
void NzMatrix4<T>::MakeIdentity()
{
	Set(F(1.0), F(0.0), F(0.0), F(0.0),
		F(0.0), F(1.0), F(0.0), F(0.0),
		F(0.0), F(0.0), F(1.0), F(0.0),
		F(0.0), F(0.0), F(0.0), F(1.0));
}

template<typename T>
void NzMatrix4<T>::MakeOrtho(T left, T top, T width, T height, T zNear, T zFar)
{
	// http://msdn.microsoft.com/en-us/library/windows/desktop/bb204941(v=vs.85).aspx
	Set(F(2.0)/(width-left), F(0.0), F(0.0), -(width+left)/(width-left),
	    F(0.0), F(2.0)/(top-height), F(0.0), -(top+height)/(top-height),
	    F(0.0), F(0.0), F(-2.0)/(zFar-zNear), -(zFar+zNear)/(zFar-zNear),
	    F(0.0), F(0.0), F(0.0), F(1.0));
}

template<typename T>
void NzMatrix4<T>::MakeLookAt(const NzVector3<T>& eye, const NzVector3<T>& target, const NzVector3<T>& up)
{
	NzVector3<T> f = NzVector3<T>::Normalize(target - eye);
	NzVector3<T> u(up.GetNormal());
	NzVector3<T> s = NzVector3<T>::Normalize(f.CrossProduct(u));
	u = s.CrossProduct(f);

	Set(s.x, u.x, -f.x, T(0.0),
	    s.y, u.y, -f.y, T(0.0),
	    s.z, u.z, -f.z, T(0.0),
		-s.DotProduct(eye), -u.DotProduct(eye), f.DotProduct(eye), T(1.0));
}

template<typename T>
void NzMatrix4<T>::MakePerspective(T angle, T ratio, T zNear, T zFar)
{
	// http://msdn.microsoft.com/en-us/library/windows/desktop/bb204944(v=vs.85).aspx
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
}

template<typename T>
void NzMatrix4<T>::MakeRotation(const NzQuaternion<T>& rotation)
{
	// http://www.flipcode.com/documents/matrfaq.html#Q54
/*
        |       2     2                                |
        | 1 - 2Y  - 2Z    2XY + 2ZW      2XZ - 2YW     |
        |                                              |
        |                       2     2                |
    M = | 2XY - 2ZW       1 - 2X  - 2Z   2YZ + 2XW     |
        |                                              |
        |                                      2     2 |
        | 2XZ + 2YW       2YZ - 2XW      1 - 2X  - 2Y  |
        |                                              |
*/
	///FIXME: À corriger (Rotation quaternino != rotation matricielle)
	Set(F(1.0) - F(2.0)*rotation.y*rotation.y - F(2.0)*rotation.z*rotation.z,
		F(2.0)*rotation.x*rotation.y + F(2.0)*rotation.z*rotation.w,
		F(2.0)*rotation.x*rotation.z - F(2.0)*rotation.y*rotation.w,
		F(0.0),

		F(2.0)*rotation.x*rotation.y - F(2.0)*rotation.z*rotation.w,
		F(1.0) - F(2.0)*rotation.x*rotation.x - F(2.0)*rotation.z*rotation.z,
		F(2.0)*rotation.y*rotation.z + F(2.0)*rotation.x*rotation.w,
		F(0.0),

		F(2.0)*rotation.x*rotation.z + F(2.0)*rotation.y*rotation.w,
		F(2.0)*rotation.y*rotation.z - F(2.0)*rotation.x*rotation.w,
		F(1.0) - F(2.0)*rotation.x*rotation.x - F(2.0)*rotation.y*rotation.y,
		F(0.0),

		F(0.0),
		F(0.0),
		F(0.0),
		F(1.0));
}

template<typename T>
void NzMatrix4<T>::MakeScale(const NzVector3<T>& scale)
{
	Set(scale.x, F(0.0),  F(0.0),  F(0.0),
	    F(0.0),  scale.y, F(0.0),  F(0.0),
	    F(0.0),  F(0.0),  scale.z, F(0.0),
	    F(0.0),  F(0.0),  F(0.0),  F(1.0));
}

template<typename T>
void NzMatrix4<T>::MakeTranslation(const NzVector3<T>& translation)
{
	Set(F(1.0), F(0.0), F(0.0), F(0.0),
	    F(0.0), F(1.0), F(0.0), F(0.0),
	    F(0.0), F(0.0), F(1.0), F(0.0),
	    translation.x, translation.y, translation.z, F(1.0));
}

template<typename T>
void NzMatrix4<T>::MakeZero()
{
	Set(F(0.0), F(0.0), F(0.0), F(0.0),
	    F(0.0), F(0.0), F(0.0), F(0.0),
	    F(0.0), F(0.0), F(0.0), F(0.0),
	    F(0.0), F(0.0), F(0.0), F(0.0));
}

template<typename T>
void NzMatrix4<T>::Set(T r11, T r12, T r13, T r14,
					   T r21, T r22, T r23, T r24,
					   T r31, T r32, T r33, T r34,
					   T r41, T r42, T r43, T r44)
{
	EnsureOwnership();

	m_sharedMatrix->m11 = r11;
	m_sharedMatrix->m12 = r12;
	m_sharedMatrix->m13 = r13;
	m_sharedMatrix->m14 = r14;
	m_sharedMatrix->m21 = r21;
	m_sharedMatrix->m22 = r22;
	m_sharedMatrix->m23 = r23;
	m_sharedMatrix->m24 = r24;
	m_sharedMatrix->m31 = r31;
	m_sharedMatrix->m32 = r32;
	m_sharedMatrix->m33 = r33;
	m_sharedMatrix->m34 = r34;
	m_sharedMatrix->m41 = r41;
	m_sharedMatrix->m42 = r42;
	m_sharedMatrix->m43 = r43;
	m_sharedMatrix->m44 = r44;
}

template<typename T>
void NzMatrix4<T>::Set(const T matrix[16])
{
	EnsureOwnership();

	// Ici nous sommes certains de la continuité des éléments en mémoire
	std::memcpy(&m_sharedMatrix->m11, matrix, 16*sizeof(T));
}

template<typename T>
void NzMatrix4<T>::Set(const NzMatrix4& matrix)
{
	ReleaseMatrix();

	m_sharedMatrix = matrix.m_sharedMatrix;
	if (m_sharedMatrix)
	{
		NazaraMutexLock(m_sharedMatrix->mutex);
		m_sharedMatrix->refCount++;
		NazaraMutexUnlock(m_sharedMatrix->mutex);
	}
}

template<typename T>
void NzMatrix4<T>::Set(NzMatrix4&& matrix)
{
	std::swap(m_sharedMatrix, matrix.m_sharedMatrix);
}

template<typename T>
template<typename U>
void NzMatrix4<T>::Set(const NzMatrix4<U>& matrix)
{
	Set(F(matrix.m_sharedMatrix->m11), F(matrix.m_sharedMatrix->m12), F(matrix.m_sharedMatrix->m13), F(matrix.m_sharedMatrix->m14),
		F(matrix.m_sharedMatrix->m21), F(matrix.m_sharedMatrix->m22), F(matrix.m_sharedMatrix->m23), F(matrix.m_sharedMatrix->m24),
		F(matrix.m_sharedMatrix->m31), F(matrix.m_sharedMatrix->m32), F(matrix.m_sharedMatrix->m33), F(matrix.m_sharedMatrix->m34),
		F(matrix.m_sharedMatrix->m41), F(matrix.m_sharedMatrix->m42), F(matrix.m_sharedMatrix->m43), F(matrix.m_sharedMatrix->m44));
}

template<typename T>
void NzMatrix4<T>::SetRotation(const NzQuaternion<T>& rotation)
{
	// http://www.flipcode.com/documents/matrfaq.html#Q54
	EnsureOwnership();

	m_sharedMatrix->m11 = F(1.0) - F(2.0)*rotation.y*rotation.y - F(2.0)*rotation.z*rotation.z;
	m_sharedMatrix->m22 = F(1.0) - F(2.0)*rotation.x*rotation.x - F(2.0)*rotation.z*rotation.z;
	m_sharedMatrix->m33 = F(1.0) - F(2.0)*rotation.x*rotation.x - F(2.0)*rotation.y*rotation.y;

	m_sharedMatrix->m12 = F(2.0)*rotation.x*rotation.y - F(2.0)*rotation.z*rotation.w;
	m_sharedMatrix->m13 = F(2.0)*rotation.x*rotation.z + F(2.0)*rotation.y*rotation.w;
	m_sharedMatrix->m21 = F(2.0)*rotation.x*rotation.y + F(2.0)*rotation.z*rotation.w;
	m_sharedMatrix->m23 = F(2.0)*rotation.y*rotation.z - F(2.0)*rotation.x*rotation.w;
	m_sharedMatrix->m31 = F(2.0)*rotation.x*rotation.z - F(2.0)*rotation.y*rotation.w;
	m_sharedMatrix->m32 = F(2.0)*rotation.y*rotation.z + F(2.0)*rotation.x*rotation.w;
}

template<typename T>
void NzMatrix4<T>::SetScale(const NzVector3<T>& scale)
{
	EnsureOwnership();

	m_sharedMatrix->m11 = scale.x;
	m_sharedMatrix->m22 = scale.y;
	m_sharedMatrix->m33 = scale.z;
}

template<typename T>
void NzMatrix4<T>::SetTranslation(const NzVector3<T>& translation)
{
	EnsureOwnership();

	m_sharedMatrix->m41 = translation.x;
	m_sharedMatrix->m42 = translation.y;
	m_sharedMatrix->m43 = translation.z;
	m_sharedMatrix->m44 = F(1.0);
}

template<typename T>
NzString NzMatrix4<T>::ToString() const
{
	if (!m_sharedMatrix)
		return "Matrix4(undefined)";

	NzStringStream ss;
	return ss << "Matrix4(" << m_sharedMatrix->m11 << ", " << m_sharedMatrix->m12 << ", " << m_sharedMatrix->m13 << ", " << m_sharedMatrix->m14 << ",\n"
	    	  << "        " << m_sharedMatrix->m21 << ", " << m_sharedMatrix->m22 << ", " << m_sharedMatrix->m23 << ", " << m_sharedMatrix->m24 << ",\n"
	    	  << "        " << m_sharedMatrix->m31 << ", " << m_sharedMatrix->m32 << ", " << m_sharedMatrix->m33 << ", " << m_sharedMatrix->m34 << ",\n"
	    	  << "        " << m_sharedMatrix->m41 << ", " << m_sharedMatrix->m42 << ", " << m_sharedMatrix->m43 << ", " << m_sharedMatrix->m44 << ')';
}

template<typename T>
NzVector2<T> NzMatrix4<T>::Transform(const NzVector2<T>& vector, T z, T w) const
{
	#if NAZARA_MATH_SAFE
	if (!m_sharedMatrix)
	{
		NazaraError("Matrix not defined");
		return vector;
	}
	#endif

	return NzVector2<T>(m_sharedMatrix->m11*vector.x + m_sharedMatrix->m12*vector.y + m_sharedMatrix->m13*z + m_sharedMatrix->m14*w,
	                    m_sharedMatrix->m21*vector.x + m_sharedMatrix->m22*vector.y + m_sharedMatrix->m23*z + m_sharedMatrix->m24*w);
}

template<typename T>
NzVector3<T> NzMatrix4<T>::Transform(const NzVector3<T>& vector, T w) const
{
	#if NAZARA_MATH_SAFE
	if (!m_sharedMatrix)
	{
		NazaraError("Matrix not defined");
		return vector;
	}
	#endif

	return NzVector3<T>(m_sharedMatrix->m11*vector.x + m_sharedMatrix->m12*vector.y + m_sharedMatrix->m13*vector.z + m_sharedMatrix->m14*w,
	                    m_sharedMatrix->m21*vector.x + m_sharedMatrix->m22*vector.y + m_sharedMatrix->m23*vector.z + m_sharedMatrix->m24*w,
	                    m_sharedMatrix->m31*vector.x + m_sharedMatrix->m32*vector.y + m_sharedMatrix->m33*vector.z + m_sharedMatrix->m34*w);
}

template<typename T>
NzVector4<T> NzMatrix4<T>::Transform(const NzVector4<T>& vector) const
{
	#if NAZARA_MATH_SAFE
	if (!m_sharedMatrix)
	{
		NazaraError("Matrix not defined");
		return vector;
	}
	#endif

	return NzVector4<T>(m_sharedMatrix->m11*vector.x + m_sharedMatrix->m12*vector.y + m_sharedMatrix->m13*vector.z + m_sharedMatrix->m14*vector.w,
	                    m_sharedMatrix->m21*vector.x + m_sharedMatrix->m22*vector.y + m_sharedMatrix->m23*vector.z + m_sharedMatrix->m24*vector.w,
	                    m_sharedMatrix->m31*vector.x + m_sharedMatrix->m32*vector.y + m_sharedMatrix->m33*vector.z + m_sharedMatrix->m34*vector.w,
	                    m_sharedMatrix->m41*vector.x + m_sharedMatrix->m42*vector.y + m_sharedMatrix->m43*vector.z + m_sharedMatrix->m44*vector.w);
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::Transpose()
{
	#if NAZARA_MATH_SAFE
	if (!m_sharedMatrix)
	{
		NazaraError("Matrix not defined");
		return *this;
	}
	#endif

	std::swap(m_sharedMatrix->m12, m_sharedMatrix->m21);
	std::swap(m_sharedMatrix->m13, m_sharedMatrix->m31);
	std::swap(m_sharedMatrix->m14, m_sharedMatrix->m41);
	std::swap(m_sharedMatrix->m23, m_sharedMatrix->m32);
	std::swap(m_sharedMatrix->m24, m_sharedMatrix->m42);
	std::swap(m_sharedMatrix->m34, m_sharedMatrix->m43);

	return *this;
}

template<typename T>
NzMatrix4<T>::operator NzString() const
{
	return ToString();
}

template<typename T>
NzMatrix4<T>::operator T*()
{
	#if NAZARA_MATH_SAFE
	if (!m_sharedMatrix)
	{
		NazaraError("Matrix not defined");
		return nullptr;
	}
	#endif

	EnsureOwnership();

	return &m_sharedMatrix->m11;
}

template<typename T>
NzMatrix4<T>::operator const T*() const
{
	#if NAZARA_MATH_SAFE
	if (!m_sharedMatrix)
	{
		NazaraError("Matrix not defined");
		return nullptr;
	}
	#endif

	return &m_sharedMatrix->m11;
}

template<typename T>
T& NzMatrix4<T>::operator()(unsigned int x, unsigned int y)
{
	#if NAZARA_MATH_SAFE
	if (x > 3 || y > 3)
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Index out of range: (" << x << ", " << y << ") > (3,3)";

		throw std::out_of_range(ss.ToString());
	}
	#endif

	EnsureOwnership();

	return (&m_sharedMatrix->m11)[y*4+x];
}

template<typename T>
const T& NzMatrix4<T>::operator()(unsigned int x, unsigned int y) const
{
	#if NAZARA_MATH_SAFE
	if (!m_sharedMatrix)
	{
		NazaraError("Matrix not defined");
		throw std::runtime_error("Tried to access element of Matrix not defined");
	}

	if (x > 3 || y > 3)
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Index out of range: (" << x << ", " << y << ") > (3,3)";

		throw std::out_of_range(ss.ToString());
	}
	#endif

	return (&m_sharedMatrix->m11)[y*4+x];
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::operator=(const NzMatrix4& matrix)
{
	Set(matrix);

	return *this;
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::operator=(NzMatrix4&& matrix) noexcept
{
	Set(matrix);

	return *this;
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::operator*(const NzMatrix4& matrix) const
{
	return Concatenate(matrix);
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
	#if NAZARA_MATH_SAFE
	if (!m_sharedMatrix)
	{
		NazaraError("Matrix not defined");
		return *this;
	}
	#endif

	return NzMatrix4(m_sharedMatrix->m11 * scalar, m_sharedMatrix->m12 * scalar, m_sharedMatrix->m13 * scalar, m_sharedMatrix->m14 * scalar,
	                 m_sharedMatrix->m21 * scalar, m_sharedMatrix->m22 * scalar, m_sharedMatrix->m23 * scalar, m_sharedMatrix->m24 * scalar,
	                 m_sharedMatrix->m31 * scalar, m_sharedMatrix->m32 * scalar, m_sharedMatrix->m33 * scalar, m_sharedMatrix->m34 * scalar,
	                 m_sharedMatrix->m41 * scalar, m_sharedMatrix->m42 * scalar, m_sharedMatrix->m43 * scalar, m_sharedMatrix->m44 * scalar);
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::operator*=(const NzMatrix4& matrix)
{
	#if NAZARA_MATH_SAFE
	if (!m_sharedMatrix)
	{
		NazaraError("Matrix not defined");
		return *this;
	}
	#endif

	Set(Concatenate(*this, matrix));

	return *this;
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::operator*=(T scalar)
{
	#if NAZARA_MATH_SAFE
	if (!m_sharedMatrix)
	{
		NazaraError("Matrix not defined");
		return *this;
	}
	#endif

	m_sharedMatrix->m11 *= scalar;
	m_sharedMatrix->m12 *= scalar;
	m_sharedMatrix->m13 *= scalar;
	m_sharedMatrix->m14 *= scalar;
	m_sharedMatrix->m21 *= scalar;
	m_sharedMatrix->m22 *= scalar;
	m_sharedMatrix->m23 *= scalar;
	m_sharedMatrix->m24 *= scalar;
	m_sharedMatrix->m31 *= scalar;
	m_sharedMatrix->m32 *= scalar;
	m_sharedMatrix->m33 *= scalar;
	m_sharedMatrix->m34 *= scalar;
	m_sharedMatrix->m41 *= scalar;
	m_sharedMatrix->m42 *= scalar;
	m_sharedMatrix->m43 *= scalar;
	m_sharedMatrix->m44 *= scalar;

	return *this;
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::Concatenate(const NzMatrix4& m1, const NzMatrix4& m2)
{
	#if NAZARA_MATH_SAFE
	if (!m1.IsDefined())
	{
		NazaraError("First matrix not defined");
		return NzMatrix4();
	}

	if (!m2.IsDefined())
	{
		NazaraError("Second matrix not defined");
		return NzMatrix4();
	}
	#endif

	#if NAZARA_MATH_MATRIX4_CHECK_AFFINE
	if (m1.IsAffine() && m2.IsAffine())
		return ConcatenateAffine(m1, m2);
	#endif

	return NzMatrix4(m1.m_sharedMatrix->m11*m2.m_sharedMatrix->m11 + m1.m_sharedMatrix->m12*m2.m_sharedMatrix->m21 + m1.m_sharedMatrix->m13*m2.m_sharedMatrix->m31 + m1.m_sharedMatrix->m14*m2.m_sharedMatrix->m41,
	                 m1.m_sharedMatrix->m11*m2.m_sharedMatrix->m12 + m1.m_sharedMatrix->m12*m2.m_sharedMatrix->m22 + m1.m_sharedMatrix->m13*m2.m_sharedMatrix->m32 + m1.m_sharedMatrix->m14*m2.m_sharedMatrix->m42,
	                 m1.m_sharedMatrix->m11*m2.m_sharedMatrix->m13 + m1.m_sharedMatrix->m12*m2.m_sharedMatrix->m23 + m1.m_sharedMatrix->m13*m2.m_sharedMatrix->m33 + m1.m_sharedMatrix->m14*m2.m_sharedMatrix->m43,
	                 m1.m_sharedMatrix->m11*m2.m_sharedMatrix->m14 + m1.m_sharedMatrix->m12*m2.m_sharedMatrix->m24 + m1.m_sharedMatrix->m13*m2.m_sharedMatrix->m34 + m1.m_sharedMatrix->m14*m2.m_sharedMatrix->m44,

	                 m1.m_sharedMatrix->m21*m2.m_sharedMatrix->m11 + m1.m_sharedMatrix->m22*m2.m_sharedMatrix->m21 + m1.m_sharedMatrix->m23*m2.m_sharedMatrix->m31 + m1.m_sharedMatrix->m24*m2.m_sharedMatrix->m41,
	                 m1.m_sharedMatrix->m21*m2.m_sharedMatrix->m12 + m1.m_sharedMatrix->m22*m2.m_sharedMatrix->m22 + m1.m_sharedMatrix->m23*m2.m_sharedMatrix->m32 + m1.m_sharedMatrix->m24*m2.m_sharedMatrix->m42,
	                 m1.m_sharedMatrix->m21*m2.m_sharedMatrix->m13 + m1.m_sharedMatrix->m22*m2.m_sharedMatrix->m23 + m1.m_sharedMatrix->m23*m2.m_sharedMatrix->m33 + m1.m_sharedMatrix->m24*m2.m_sharedMatrix->m43,
	                 m1.m_sharedMatrix->m21*m2.m_sharedMatrix->m14 + m1.m_sharedMatrix->m22*m2.m_sharedMatrix->m24 + m1.m_sharedMatrix->m23*m2.m_sharedMatrix->m34 + m1.m_sharedMatrix->m24*m2.m_sharedMatrix->m44,

	                 m1.m_sharedMatrix->m31*m2.m_sharedMatrix->m11 + m1.m_sharedMatrix->m32*m2.m_sharedMatrix->m21 + m1.m_sharedMatrix->m33*m2.m_sharedMatrix->m31 + m1.m_sharedMatrix->m34*m2.m_sharedMatrix->m41,
	                 m1.m_sharedMatrix->m31*m2.m_sharedMatrix->m12 + m1.m_sharedMatrix->m32*m2.m_sharedMatrix->m22 + m1.m_sharedMatrix->m33*m2.m_sharedMatrix->m32 + m1.m_sharedMatrix->m34*m2.m_sharedMatrix->m42,
	                 m1.m_sharedMatrix->m31*m2.m_sharedMatrix->m13 + m1.m_sharedMatrix->m32*m2.m_sharedMatrix->m23 + m1.m_sharedMatrix->m33*m2.m_sharedMatrix->m33 + m1.m_sharedMatrix->m34*m2.m_sharedMatrix->m43,
	                 m1.m_sharedMatrix->m31*m2.m_sharedMatrix->m14 + m1.m_sharedMatrix->m32*m2.m_sharedMatrix->m24 + m1.m_sharedMatrix->m33*m2.m_sharedMatrix->m34 + m1.m_sharedMatrix->m34*m2.m_sharedMatrix->m44,

	                 m1.m_sharedMatrix->m41*m2.m_sharedMatrix->m11 + m1.m_sharedMatrix->m42*m2.m_sharedMatrix->m21 + m1.m_sharedMatrix->m43*m2.m_sharedMatrix->m31 + m1.m_sharedMatrix->m44*m2.m_sharedMatrix->m41,
	                 m1.m_sharedMatrix->m41*m2.m_sharedMatrix->m12 + m1.m_sharedMatrix->m42*m2.m_sharedMatrix->m22 + m1.m_sharedMatrix->m43*m2.m_sharedMatrix->m32 + m1.m_sharedMatrix->m44*m2.m_sharedMatrix->m42,
	                 m1.m_sharedMatrix->m41*m2.m_sharedMatrix->m13 + m1.m_sharedMatrix->m42*m2.m_sharedMatrix->m23 + m1.m_sharedMatrix->m43*m2.m_sharedMatrix->m33 + m1.m_sharedMatrix->m44*m2.m_sharedMatrix->m43,
	                 m1.m_sharedMatrix->m41*m2.m_sharedMatrix->m14 + m1.m_sharedMatrix->m42*m2.m_sharedMatrix->m24 + m1.m_sharedMatrix->m43*m2.m_sharedMatrix->m34 + m1.m_sharedMatrix->m44*m2.m_sharedMatrix->m44);
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::ConcatenateAffine(const NzMatrix4& m1, const NzMatrix4& m2)
{
	#if NAZARA_MATH_SAFE
	if (!m1.IsDefined())
	{
		NazaraError("First matrix not defined");
		return NzMatrix4();
	}

	if (!m2.IsDefined())
	{
		NazaraError("Second matrix not defined");
		return NzMatrix4();
	}
	#endif

	#ifdef NAZARA_DEBUG
	if (!m1.IsAffine())
	{
		NazaraError("First matrix not affine");
		return NzMatrix4();
	}

	if (!m2.IsAffine())
	{
		NazaraError("Second matrix not affine");
		return NzMatrix4();
	}
	#endif

	return NzMatrix4(m1.m_sharedMatrix->m11*m2.m_sharedMatrix->m11 + m1.m_sharedMatrix->m12*m2.m_sharedMatrix->m21 + m1.m_sharedMatrix->m13*m2.m_sharedMatrix->m31,
	                 m1.m_sharedMatrix->m11*m2.m_sharedMatrix->m12 + m1.m_sharedMatrix->m12*m2.m_sharedMatrix->m22 + m1.m_sharedMatrix->m13*m2.m_sharedMatrix->m32,
	                 m1.m_sharedMatrix->m11*m2.m_sharedMatrix->m13 + m1.m_sharedMatrix->m12*m2.m_sharedMatrix->m23 + m1.m_sharedMatrix->m13*m2.m_sharedMatrix->m33,
	                 F(0.0),

	                 m1.m_sharedMatrix->m21*m2.m_sharedMatrix->m11 + m1.m_sharedMatrix->m22*m2.m_sharedMatrix->m21 + m1.m_sharedMatrix->m23*m2.m_sharedMatrix->m31,
	                 m1.m_sharedMatrix->m21*m2.m_sharedMatrix->m12 + m1.m_sharedMatrix->m22*m2.m_sharedMatrix->m22 + m1.m_sharedMatrix->m23*m2.m_sharedMatrix->m32,
	                 m1.m_sharedMatrix->m21*m2.m_sharedMatrix->m13 + m1.m_sharedMatrix->m22*m2.m_sharedMatrix->m23 + m1.m_sharedMatrix->m23*m2.m_sharedMatrix->m33,
	                 F(0.0),

	                 m1.m_sharedMatrix->m31*m2.m_sharedMatrix->m11 + m1.m_sharedMatrix->m32*m2.m_sharedMatrix->m21 + m1.m_sharedMatrix->m33*m2.m_sharedMatrix->m31,
	                 m1.m_sharedMatrix->m31*m2.m_sharedMatrix->m12 + m1.m_sharedMatrix->m32*m2.m_sharedMatrix->m22 + m1.m_sharedMatrix->m33*m2.m_sharedMatrix->m32,
	                 m1.m_sharedMatrix->m31*m2.m_sharedMatrix->m13 + m1.m_sharedMatrix->m32*m2.m_sharedMatrix->m23 + m1.m_sharedMatrix->m33*m2.m_sharedMatrix->m33,
	                 F(0.0),

	                 m1.m_sharedMatrix->m41*m2.m_sharedMatrix->m11 + m1.m_sharedMatrix->m42*m2.m_sharedMatrix->m21 + m1.m_sharedMatrix->m43*m2.m_sharedMatrix->m31 + m2.m_sharedMatrix->m41,
	                 m1.m_sharedMatrix->m41*m2.m_sharedMatrix->m12 + m1.m_sharedMatrix->m42*m2.m_sharedMatrix->m22 + m1.m_sharedMatrix->m43*m2.m_sharedMatrix->m32 + m2.m_sharedMatrix->m42,
	                 m1.m_sharedMatrix->m41*m2.m_sharedMatrix->m13 + m1.m_sharedMatrix->m42*m2.m_sharedMatrix->m23 + m1.m_sharedMatrix->m43*m2.m_sharedMatrix->m33 + m2.m_sharedMatrix->m43,
	                 F(1.0));
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
NzMatrix4<T> NzMatrix4<T>::Ortho(T left, T top, T width, T height, T zNear, T zFar)
{
	NzMatrix4 matrix;
	matrix.MakeOrtho(left, top, width, height, zNear, zFar);

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

template<typename T>
void NzMatrix4<T>::EnsureOwnership()
{
	if (m_sharedMatrix)
	{
		NazaraLock(m_sharedMatrix->mutex);
		if (m_sharedMatrix->refCount > 1)
		{
			m_sharedMatrix->refCount--;

			SharedMatrix* sharedMatrix = new SharedMatrix;
			std::memcpy(&sharedMatrix->m11, &m_sharedMatrix->m11, 16*sizeof(T));
		}
	}
	else
		m_sharedMatrix = new SharedMatrix;
}

template<typename T>
void NzMatrix4<T>::ReleaseMatrix()
{
	if (!m_sharedMatrix)
		return;

	NazaraMutexLock(m_sharedMatrix->mutex);
	bool freeSharedMatrix = (--m_sharedMatrix->refCount == 0);
	NazaraMutexUnlock(m_sharedMatrix->mutex);

	if (freeSharedMatrix)
		delete m_sharedMatrix;

	m_sharedMatrix = nullptr;
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
