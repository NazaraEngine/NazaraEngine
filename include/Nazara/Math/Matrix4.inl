// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
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

#define NAZARA_CLASS_MATRIX4
#include <Nazara/Math/ThreadSafety.hpp>
#include <Nazara/Core/Debug.hpp>

template<typename T>
NzMatrix4<T>::NzMatrix4() :
m_sharedMatrix(nullptr)
{
}

template<typename T>
NzMatrix4<T>::NzMatrix4(T r11, T r12, T r13, T r14,
						T r21, T r22, T r23, T r24,
						T r31, T r32, T r33, T r34,
						T r41, T r42, T r43, T r44) :
m_sharedMatrix(nullptr)
{
	Set(r11, r12, r13, r14,
		r21, r22, r23, r24,
		r31, r32, r33, r34,
		r41, r42, r43, r44);
}

template<typename T>
NzMatrix4<T>::NzMatrix4(T matrix[16]) :
m_sharedMatrix(nullptr)
{
	Set(matrix);
}

template<typename T>
template<typename U>
NzMatrix4<T>::NzMatrix4(const NzMatrix4<U>& matrix) :
m_sharedMatrix(nullptr)
{
	Set(matrix);
}

template<typename T>
NzMatrix4<T>::NzMatrix4(const NzMatrix4& matrix) :
m_sharedMatrix(nullptr)
{
	Set(matrix);
}

template<typename T>
NzMatrix4<T>::NzMatrix4(NzMatrix4&& matrix) :
m_sharedMatrix(nullptr)
{
	Set(matrix);
}

template<typename T>
NzMatrix4<T>::~NzMatrix4()
{
	ReleaseMatrix();
}

template<typename T>
T NzMatrix4<T>::GetDeterminant() const
{
	#if NAZARA_MATH_SAFE
	if (!m_sharedMatrix)
	{
		NazaraError("Undefined matrix");
		return 0.0;
	}
	#endif

	T A = m_sharedMatrix->m22 * (m_sharedMatrix->m33 * m_sharedMatrix->m44 - m_sharedMatrix->m43 * m_sharedMatrix->m34) - m_sharedMatrix->m32 * (m_sharedMatrix->m23 * m_sharedMatrix->m44 - m_sharedMatrix->m43 * m_sharedMatrix->m24) + m_sharedMatrix->m42 * (m_sharedMatrix->m23 * m_sharedMatrix->m34 - m_sharedMatrix->m33 * m_sharedMatrix->m24);
	T B = m_sharedMatrix->m12 * (m_sharedMatrix->m33 * m_sharedMatrix->m44 - m_sharedMatrix->m43 * m_sharedMatrix->m34) - m_sharedMatrix->m32 * (m_sharedMatrix->m13 * m_sharedMatrix->m44 - m_sharedMatrix->m43 * m_sharedMatrix->m14) + m_sharedMatrix->m42 * (m_sharedMatrix->m13 * m_sharedMatrix->m34 - m_sharedMatrix->m33 * m_sharedMatrix->m14);
	T C = m_sharedMatrix->m12 * (m_sharedMatrix->m23 * m_sharedMatrix->m44 - m_sharedMatrix->m43 * m_sharedMatrix->m24) - m_sharedMatrix->m22 * (m_sharedMatrix->m13 * m_sharedMatrix->m44 - m_sharedMatrix->m43 * m_sharedMatrix->m14) + m_sharedMatrix->m42 * (m_sharedMatrix->m13 * m_sharedMatrix->m24 - m_sharedMatrix->m23 * m_sharedMatrix->m14);
	T D = m_sharedMatrix->m12 * (m_sharedMatrix->m23 * m_sharedMatrix->m34 - m_sharedMatrix->m33 * m_sharedMatrix->m24) - m_sharedMatrix->m22 * (m_sharedMatrix->m13 * m_sharedMatrix->m34 - m_sharedMatrix->m33 * m_sharedMatrix->m14) + m_sharedMatrix->m32 * (m_sharedMatrix->m13 * m_sharedMatrix->m24 - m_sharedMatrix->m23 * m_sharedMatrix->m14);

	return m_sharedMatrix->m11 * A - m_sharedMatrix->m21 * B + m_sharedMatrix->m31 * C - m_sharedMatrix->m41 * D;
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::GetInverse() const
{
	#if NAZARA_MATH_SAFE
	if (!m_sharedMatrix)
	{
		NazaraError("Undefined matrix");
		return NzMatrix4();
	}
	#endif

	NzMatrix4 matrix;
	T det = GetDeterminant();

	if (det != 0.0)
	{
		matrix(0, 0) =  (m_sharedMatrix->m22 * (m_sharedMatrix->m33 * m_sharedMatrix->m44 - m_sharedMatrix->m34 * m_sharedMatrix->m43) - m_sharedMatrix->m32 * (m_sharedMatrix->m23 * m_sharedMatrix->m44 - m_sharedMatrix->m43 * m_sharedMatrix->m24) + m_sharedMatrix->m42 * (m_sharedMatrix->m23 * m_sharedMatrix->m34 - m_sharedMatrix->m33 * m_sharedMatrix->m24)) / det;
		matrix(0, 1) = -(m_sharedMatrix->m12 * (m_sharedMatrix->m33 * m_sharedMatrix->m44 - m_sharedMatrix->m43 * m_sharedMatrix->m34) - m_sharedMatrix->m32 * (m_sharedMatrix->m13 * m_sharedMatrix->m44 - m_sharedMatrix->m43 * m_sharedMatrix->m14) + m_sharedMatrix->m42 * (m_sharedMatrix->m13 * m_sharedMatrix->m34 - m_sharedMatrix->m33 * m_sharedMatrix->m14)) / det;
		matrix(0, 2) =  (m_sharedMatrix->m12 * (m_sharedMatrix->m23 * m_sharedMatrix->m44 - m_sharedMatrix->m43 * m_sharedMatrix->m24) - m_sharedMatrix->m22 * (m_sharedMatrix->m13 * m_sharedMatrix->m44 - m_sharedMatrix->m43 * m_sharedMatrix->m14) + m_sharedMatrix->m42 * (m_sharedMatrix->m13 * m_sharedMatrix->m24 - m_sharedMatrix->m23 * m_sharedMatrix->m14)) / det;
		matrix(0, 3) = -(m_sharedMatrix->m12 * (m_sharedMatrix->m23 * m_sharedMatrix->m34 - m_sharedMatrix->m33 * m_sharedMatrix->m24) - m_sharedMatrix->m22 * (m_sharedMatrix->m13 * m_sharedMatrix->m34 - m_sharedMatrix->m33 * m_sharedMatrix->m14) + m_sharedMatrix->m32 * (m_sharedMatrix->m13 * m_sharedMatrix->m24 - m_sharedMatrix->m23 * m_sharedMatrix->m14)) / det;

		matrix(1, 0) = -(m_sharedMatrix->m21 * (m_sharedMatrix->m33 * m_sharedMatrix->m44 - m_sharedMatrix->m34 * m_sharedMatrix->m43) - m_sharedMatrix->m23 * (m_sharedMatrix->m31 * m_sharedMatrix->m44 - m_sharedMatrix->m34 * m_sharedMatrix->m41) + m_sharedMatrix->m24 * (m_sharedMatrix->m31 * m_sharedMatrix->m43 - m_sharedMatrix->m33 * m_sharedMatrix->m41)) / det;
		matrix(1, 1) =  (m_sharedMatrix->m11 * (m_sharedMatrix->m33 * m_sharedMatrix->m44 - m_sharedMatrix->m34 * m_sharedMatrix->m43) - m_sharedMatrix->m13 * (m_sharedMatrix->m31 * m_sharedMatrix->m44 - m_sharedMatrix->m34 * m_sharedMatrix->m41) + m_sharedMatrix->m14 * (m_sharedMatrix->m31 * m_sharedMatrix->m43 - m_sharedMatrix->m33 * m_sharedMatrix->m41)) / det;
		matrix(1, 2) = -(m_sharedMatrix->m11 * (m_sharedMatrix->m23 * m_sharedMatrix->m44 - m_sharedMatrix->m24 * m_sharedMatrix->m43) - m_sharedMatrix->m13 * (m_sharedMatrix->m21 * m_sharedMatrix->m44 - m_sharedMatrix->m24 * m_sharedMatrix->m41) + m_sharedMatrix->m14 * (m_sharedMatrix->m21 * m_sharedMatrix->m43 - m_sharedMatrix->m23 * m_sharedMatrix->m41)) / det;
		matrix(1, 3) =  (m_sharedMatrix->m11 * (m_sharedMatrix->m23 * m_sharedMatrix->m34 - m_sharedMatrix->m24 * m_sharedMatrix->m33) - m_sharedMatrix->m13 * (m_sharedMatrix->m21 * m_sharedMatrix->m34 - m_sharedMatrix->m24 * m_sharedMatrix->m31) + m_sharedMatrix->m14 * (m_sharedMatrix->m21 * m_sharedMatrix->m33 - m_sharedMatrix->m23 * m_sharedMatrix->m31)) / det;

		matrix(2, 0) =  (m_sharedMatrix->m21 * (m_sharedMatrix->m32 * m_sharedMatrix->m44 - m_sharedMatrix->m34 * m_sharedMatrix->m42) - m_sharedMatrix->m22 * (m_sharedMatrix->m31 * m_sharedMatrix->m44 - m_sharedMatrix->m34 * m_sharedMatrix->m41) + m_sharedMatrix->m24 * (m_sharedMatrix->m31 * m_sharedMatrix->m42 - m_sharedMatrix->m32 * m_sharedMatrix->m41)) / det;
		matrix(2, 1) = -(m_sharedMatrix->m11 * (m_sharedMatrix->m32 * m_sharedMatrix->m44 - m_sharedMatrix->m34 * m_sharedMatrix->m42) - m_sharedMatrix->m12 * (m_sharedMatrix->m31 * m_sharedMatrix->m44 - m_sharedMatrix->m34 * m_sharedMatrix->m41) + m_sharedMatrix->m14 * (m_sharedMatrix->m31 * m_sharedMatrix->m42 - m_sharedMatrix->m32 * m_sharedMatrix->m41)) / det;
		matrix(2, 2) =  (m_sharedMatrix->m11 * (m_sharedMatrix->m22 * m_sharedMatrix->m44 - m_sharedMatrix->m24 * m_sharedMatrix->m42) - m_sharedMatrix->m12 * (m_sharedMatrix->m21 * m_sharedMatrix->m44 - m_sharedMatrix->m24 * m_sharedMatrix->m41) + m_sharedMatrix->m14 * (m_sharedMatrix->m21 * m_sharedMatrix->m42 - m_sharedMatrix->m22 * m_sharedMatrix->m41)) / det;
		matrix(2, 3) = -(m_sharedMatrix->m11 * (m_sharedMatrix->m22 * m_sharedMatrix->m34 - m_sharedMatrix->m24 * m_sharedMatrix->m32) - m_sharedMatrix->m12 * (m_sharedMatrix->m21 * m_sharedMatrix->m34 - m_sharedMatrix->m24 * m_sharedMatrix->m31) + m_sharedMatrix->m14 * (m_sharedMatrix->m21 * m_sharedMatrix->m32 - m_sharedMatrix->m22 * m_sharedMatrix->m31)) / det;

		matrix(3, 0) = -(m_sharedMatrix->m21 * (m_sharedMatrix->m32 * m_sharedMatrix->m43 - m_sharedMatrix->m33 * m_sharedMatrix->m42) - m_sharedMatrix->m22 * (m_sharedMatrix->m31 * m_sharedMatrix->m43 - m_sharedMatrix->m33 * m_sharedMatrix->m41) + m_sharedMatrix->m23 * (m_sharedMatrix->m31 * m_sharedMatrix->m42 - m_sharedMatrix->m32 * m_sharedMatrix->m41)) / det;
		matrix(3, 1) =  (m_sharedMatrix->m11 * (m_sharedMatrix->m32 * m_sharedMatrix->m43 - m_sharedMatrix->m33 * m_sharedMatrix->m42) - m_sharedMatrix->m12 * (m_sharedMatrix->m31 * m_sharedMatrix->m43 - m_sharedMatrix->m33 * m_sharedMatrix->m41) + m_sharedMatrix->m13 * (m_sharedMatrix->m31 * m_sharedMatrix->m42 - m_sharedMatrix->m32 * m_sharedMatrix->m41)) / det;
		matrix(3, 2) = -(m_sharedMatrix->m11 * (m_sharedMatrix->m22 * m_sharedMatrix->m43 - m_sharedMatrix->m23 * m_sharedMatrix->m42) - m_sharedMatrix->m12 * (m_sharedMatrix->m21 * m_sharedMatrix->m43 - m_sharedMatrix->m23 * m_sharedMatrix->m41) + m_sharedMatrix->m13 * (m_sharedMatrix->m21 * m_sharedMatrix->m42 - m_sharedMatrix->m22 * m_sharedMatrix->m41)) / det;
		matrix(3, 3) =  (m_sharedMatrix->m11 * (m_sharedMatrix->m22 * m_sharedMatrix->m33 - m_sharedMatrix->m23 * m_sharedMatrix->m32) - m_sharedMatrix->m12 * (m_sharedMatrix->m21 * m_sharedMatrix->m33 - m_sharedMatrix->m23 * m_sharedMatrix->m31) + m_sharedMatrix->m13 * (m_sharedMatrix->m21 * m_sharedMatrix->m32 - m_sharedMatrix->m22 * m_sharedMatrix->m31)) / det;
	}

	return matrix;
}

template<typename T>
NzVector3<T> NzMatrix4<T>::GetScale() const
{
	#if NAZARA_MATH_SAFE
	if (!m_sharedMatrix)
	{
		NazaraError("Undefined matrix");
		return NzVector3<T>();
	}
	#endif

	return NzVector3<T>(std::sqrt(m_sharedMatrix->m11 * m_sharedMatrix->m11 + m_sharedMatrix->m21 * m_sharedMatrix->m21 + m_sharedMatrix->m31 * m_sharedMatrix->m31),
						std::sqrt(m_sharedMatrix->m12 * m_sharedMatrix->m12 + m_sharedMatrix->m22 * m_sharedMatrix->m22 + m_sharedMatrix->m32 * m_sharedMatrix->m32),
						std::sqrt(m_sharedMatrix->m13 * m_sharedMatrix->m13 + m_sharedMatrix->m23 * m_sharedMatrix->m23 + m_sharedMatrix->m33 * m_sharedMatrix->m33));
}

template<typename T>
NzVector3<T> NzMatrix4<T>::GetTranslation() const
{
	#if NAZARA_MATH_SAFE
	if (!m_sharedMatrix)
	{
		NazaraError("Undefined matrix");
		return NzVector3<T>();
	}
	#endif

	#if NAZARA_MATH_MATRIX_COLUMN_MAJOR
	return NzVector3<T>(m_sharedMatrix->m41, m_sharedMatrix->m42, m_sharedMatrix->m43);
	#else
	return NzVector3<T>(m_sharedMatrix->m14, m_sharedMatrix->m24, m_sharedMatrix->m34);
	#endif
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::GetTransposed() const
{
	#if NAZARA_MATH_SAFE
	if (!m_sharedMatrix)
	{
		NazaraError("Undefined matrix");
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
	return GetDeterminant() < 0.f;
}

template<typename T>
bool NzMatrix4<T>::HasScale() const
{
	#if NAZARA_MATH_SAFE
	if (!m_sharedMatrix)
	{
		NazaraError("Undefined matrix");
		return false;
	}
	#endif

	T t = m_sharedMatrix->m11 * m_sharedMatrix->m11 + m_sharedMatrix->m21 * m_sharedMatrix->m21 + m_sharedMatrix->m31 * m_sharedMatrix->m31;
	if (1.0 - t > std::numeric_limits<T>::epsilon())
		return true;

	t = m_sharedMatrix->m12 * m_sharedMatrix->m12 + m_sharedMatrix->m22 * m_sharedMatrix->m22 + m_sharedMatrix->m32 * m_sharedMatrix->m32;
	if (1.0 - t > std::numeric_limits<T>::epsilon())
		return true;

	t = m_sharedMatrix->m13 * m_sharedMatrix->m13 + m_sharedMatrix->m23 * m_sharedMatrix->m23 + m_sharedMatrix->m33 * m_sharedMatrix->m33;
	if (1.0 - t > std::numeric_limits<T>::epsilon())
		return true;

	return false;
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
void NzMatrix4<T>::Set(T matrix[16])
{
	EnsureOwnership();

	// Ici nous sommes certains de la continuité des éléments en mémoire
	std::memcpy(&m_sharedMatrix->m41, matrix, 16*sizeof(T));
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
	Set(static_cast<T>(matrix.m11), static_cast<T>(matrix.m12), static_cast<T>(matrix.m13), static_cast<T>(matrix.m14),
		static_cast<T>(matrix.m21), static_cast<T>(matrix.m22), static_cast<T>(matrix.m23), static_cast<T>(matrix.m24),
		static_cast<T>(matrix.m31), static_cast<T>(matrix.m32), static_cast<T>(matrix.m33), static_cast<T>(matrix.m34),
		static_cast<T>(matrix.m41), static_cast<T>(matrix.m42), static_cast<T>(matrix.m43), static_cast<T>(matrix.m44));
}

template<typename T>
void NzMatrix4<T>::SetIdentity()
{
	Set(1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0);
}

template<typename T>
void NzMatrix4<T>::SetOrtho(T left, T top, T width, T height, T zNear, T zFar)
{
	#if NAZARA_MATH_MATRIX_COLUMN_MAJOR
	Set(2.0/(width-left), 0.0, 0.0, -(width+left)/(width-left),
	    0.0, 2.0/(top-height), 0.0, -(top+height)/(top-height),
		0.0, 0.0, -2.0/(zFar-zNear), -(zFar+zNear)/(zFar-zNear),
		0.0, 0.0, 0.0, 1.0);
	#else
	Set(2.0/(width-left), 0.0, 0.0, 0.0,
	    0.0, 2.0/(top-height), 0.0, 0.0,
		0.0, 0.0, -2.0/(zFar-zNear), 0.0,
		-(width+left)/(width-left), -(top+height)/(top-height), -(zFar+zNear)/(zFar-zNear), 1.0);
	#endif
}

template<typename T>
void NzMatrix4<T>::SetLookAt(const NzVector3<T>& eye, const NzVector3<T>& center, const NzVector3<T>& up)
{
	NzVector3<T> f = center - eye;
	f.Normalize();

	NzVector3<T> u = up;
	u.Normalize();

	NzVector3<T> s = f.CrossProduct(u);
	s.Normalize();

	u = s.CrossProduct(f);

	Set(s.x, u.x, -f.x, 0.0,
		s.y, u.y, -f.y, 0.0,
		s.z, u.z, -f.z, 0.0,
		0.0, 0.0, 0.0, 1.0);

	operator*=(Translate(-eye));
}

template<typename T>
void NzMatrix4<T>::SetPerspective(T angle, T ratio, T zNear, T zFar)
{
	#if NAZARA_MATH_ANGLE_RADIAN
	angle /= 2;
	#else
	angle = NzDegreeToRadian(angle/2);
	#endif

	auto f = 1 / std::tan(angle);

	#if NAZARA_MATH_MATRIX_COLUMN_MAJOR
	Set(f / ratio, 0.0, 0.0, 0.0,
		0.0, f, 0.0, 0.0,
		0.0, 0.0, (zNear + zFar) / (zNear - zFar), -1.0,
		0.0, 0.0, (2 * zNear * zFar) / (zNear - zFar), 1.0);
	#else
	Set(f / ratio, 0.0, 0.0, 0.0,
		0.0, f, 0.0, 0.0,
		0.0, 0.0, (zNear + zFar) / (zNear - zFar), (2 * zNear * zFar) / (zNear - zFar),
		0.0, 0.0, -1.0, 1.0);
	#endif
}

template<typename T>
void NzMatrix4<T>::SetRotation(const NzQuaternion<T>& rotation)
{
	// http://www.flipcode.com/documents/matrfaq.html#Q54
    T xx = rotation.x * rotation.x;
    T xy = rotation.x * rotation.y;
    T xz = rotation.x * rotation.z;
    T xw = rotation.x * rotation.w;

    T yy = rotation.y * rotation.y;
    T yz = rotation.y * rotation.z;
    T yw = rotation.y * rotation.w;

    T zz = rotation.z * rotation.z;
    T zw = rotation.z * rotation.w;

	Set(1.0 - 2.0*(yy+zz), 2.0*(xy-zw),		  2.0*(xz+yw),		 0.0,
		2.0*(xz+zw),	   1.0 - 2.0*(xx+zz), 2.0*(yz-xw),		 0.0,
		2.0*(xz-yw),	   2.0*(yz+xw),		  1.0 - 2.0*(xx+yy), 0.0,
		0.0, 			   0.0,				  0.0,				 1.0);
}

template<typename T>
void NzMatrix4<T>::SetScale(const NzVector3<T>& vector)
{
	Set(vector.x, 0.0, 0.0, 0.0,
		0.0, vector.y, 0.0, 0.0,
		0.0, 0.0, vector.z, 0.0,
		0.0, 0.0, 0.0, 1.0);
}

template<typename T>
void NzMatrix4<T>::SetTranslation(const NzVector3<T>& translation)
{
	#if NAZARA_MATH_MATRIX_COLUMN_MAJOR
	Set(1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		translation.x, translation.y, translation.z, 1.0);
	#else
	Set(1.0, 0.0, 0.0, translation.x,
		0.0, 1.0, 0.0, translation.y,
		0.0, 0.0, 1.0, translation.z,
		0.0, 0.0, 0.0, 1.0);
	#endif
}

template<typename T>
void NzMatrix4<T>::SetZero()
{
	Set(0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0);
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
		NazaraError("Undefined matrix");
		return vector;
	}
	#endif

	return NzVector2<T>(m_sharedMatrix->m11 * vector.x + m_sharedMatrix->m12 * vector.y + m_sharedMatrix->m13 * z + m_sharedMatrix->m14 * w,
						m_sharedMatrix->m21 * vector.x + m_sharedMatrix->m22 * vector.y + m_sharedMatrix->m23 * z + m_sharedMatrix->m24 * w);
}

template<typename T>
NzVector3<T> NzMatrix4<T>::Transform(const NzVector3<T>& vector, T w) const
{
	#if NAZARA_MATH_SAFE
	if (!m_sharedMatrix)
	{
		NazaraError("Undefined matrix");
		return vector;
	}
	#endif

	return NzVector3<T>(m_sharedMatrix->m11 * vector.x + m_sharedMatrix->m12 * vector.y + m_sharedMatrix->m13 * vector.z + m_sharedMatrix->m14 * w,
						m_sharedMatrix->m21 * vector.x + m_sharedMatrix->m22 * vector.y + m_sharedMatrix->m23 * vector.z + m_sharedMatrix->m24 * w,
						m_sharedMatrix->m31 * vector.x + m_sharedMatrix->m32 * vector.y + m_sharedMatrix->m33 * vector.z + m_sharedMatrix->m34 * w);
}

template<typename T>
NzVector4<T> NzMatrix4<T>::Transform(const NzVector4<T>& vector) const
{
	#if NAZARA_MATH_SAFE
	if (!m_sharedMatrix)
	{
		NazaraError("Undefined matrix");
		return vector;
	}
	#endif

	return NzVector4<T>(m_sharedMatrix->m11 * vector.x + m_sharedMatrix->m12 * vector.y + m_sharedMatrix->m13 * vector.z + m_sharedMatrix->m14 * vector.w,
						m_sharedMatrix->m21 * vector.x + m_sharedMatrix->m22 * vector.y + m_sharedMatrix->m23 * vector.z + m_sharedMatrix->m24 * vector.w,
						m_sharedMatrix->m31 * vector.x + m_sharedMatrix->m32 * vector.y + m_sharedMatrix->m33 * vector.z + m_sharedMatrix->m34 * vector.w,
						m_sharedMatrix->m41 * vector.x + m_sharedMatrix->m42 * vector.y + m_sharedMatrix->m43 * vector.z + m_sharedMatrix->m44 * vector.w);
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::Transpose()
{
	#if NAZARA_MATH_SAFE
	if (!m_sharedMatrix)
	{
		NazaraError("Undefined matrix");
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
NzMatrix4<T>::operator T*()
{
	#if NAZARA_MATH_SAFE
	if (!m_sharedMatrix)
	{
		NazaraError("Undefined matrix");
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
		NazaraError("Undefined matrix");
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

	return (&m_sharedMatrix->m11)[x*4+y];
}

template<typename T>
const T& NzMatrix4<T>::operator()(unsigned int x, unsigned int y) const
{
	#if NAZARA_MATH_SAFE
	if (!m_sharedMatrix)
	{
		NazaraError("Undefined matrix");
		throw std::runtime_error("Tried to access element of undefined matrix");
	}

	if (x > 3 || y > 3)
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Index out of range: (" << x << ", " << y << ") > (3,3)";

		throw std::out_of_range(ss.ToString());
	}
	#endif

	return (&m_sharedMatrix->m11)[x*4+y];
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::operator=(const NzMatrix4& matrix)
{
	Set(matrix);

	return *this;
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::operator=(NzMatrix4&& matrix)
{
	Set(matrix);

	return *this;
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::operator*(const NzMatrix4& matrix) const
{
	#if NAZARA_MATH_SAFE
	if (!m_sharedMatrix)
	{
		NazaraError("Undefined matrix");
		return matrix;
	}
	#endif

	NzMatrix4 mat;
	for(int k = 0; k < 4; k++)
    {
        for(int j = 0; j < 4; j++)
        {
            for(int i = 0; i < 4; i++)
                mat(j, k) += (*this)(j, i) * matrix(i, k);
        }
    }

    return mat;
	/*return NzMatrix4(m11 * matrix.m11 + m21 * matrix.m12 + m31 * matrix.m13 + m41 * matrix.m14,
				     m12 * matrix.m11 + m22 * matrix.m12 + m32 * matrix.m13 + m42 * matrix.m14,
					 m13 * matrix.m11 + m23 * matrix.m12 + m33 * matrix.m13 + m43 * matrix.m14,
				     m14 * matrix.m11 + m24 * matrix.m12 + m34 * matrix.m13 + m44 * matrix.m14,
				     m11 * matrix.m21 + m21 * matrix.m22 + m31 * matrix.m23 + m41 * matrix.m24,
				     m12 * matrix.m21 + m22 * matrix.m22 + m32 * matrix.m23 + m42 * matrix.m24,
				     m13 * matrix.m21 + m23 * matrix.m22 + m33 * matrix.m23 + m43 * matrix.m24,
				     m14 * matrix.m21 + m24 * matrix.m22 + m34 * matrix.m23 + m44 * matrix.m24,
				     m11 * matrix.m31 + m21 * matrix.m32 + m31 * matrix.m33 + m41 * matrix.m34,
				     m12 * matrix.m31 + m22 * matrix.m32 + m32 * matrix.m33 + m42 * matrix.m34,
				     m13 * matrix.m31 + m23 * matrix.m32 + m33 * matrix.m33 + m43 * matrix.m34,
				     m14 * matrix.m31 + m24 * matrix.m32 + m34 * matrix.m33 + m44 * matrix.m34,
				     m11 * matrix.m41 + m21 * matrix.m42 + m31 * matrix.m43 + m41 * matrix.m44,
				     m12 * matrix.m41 + m22 * matrix.m42 + m32 * matrix.m43 + m42 * matrix.m44,
				     m13 * matrix.m41 + m23 * matrix.m42 + m33 * matrix.m43 + m43 * matrix.m44,
				     m14 * matrix.m41 + m24 * matrix.m42 + m34 * matrix.m43 + m44 * matrix.m44);*/

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
		NazaraError("Undefined matrix");
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
		NazaraError("Undefined matrix");
		return *this;
	}
	#endif

	// On calcule dans des variables temporaires
	T r11 = m_sharedMatrix->m11 * matrix.m_sharedMatrix->m11 + m_sharedMatrix->m21 * matrix.m_sharedMatrix->m12 + m_sharedMatrix->m31 * matrix.m_sharedMatrix->m13 + m_sharedMatrix->m41 * matrix.m_sharedMatrix->m14;
	T r12 = m_sharedMatrix->m12 * matrix.m_sharedMatrix->m11 + m_sharedMatrix->m22 * matrix.m_sharedMatrix->m12 + m_sharedMatrix->m32 * matrix.m_sharedMatrix->m13 + m_sharedMatrix->m42 * matrix.m_sharedMatrix->m14;
	T r13 = m_sharedMatrix->m13 * matrix.m_sharedMatrix->m11 + m_sharedMatrix->m23 * matrix.m_sharedMatrix->m12 + m_sharedMatrix->m33 * matrix.m_sharedMatrix->m13 + m_sharedMatrix->m43 * matrix.m_sharedMatrix->m14;
	T r14 = m_sharedMatrix->m14 * matrix.m_sharedMatrix->m11 + m_sharedMatrix->m24 * matrix.m_sharedMatrix->m12 + m_sharedMatrix->m34 * matrix.m_sharedMatrix->m13 + m_sharedMatrix->m44 * matrix.m_sharedMatrix->m14;
	T r21 = m_sharedMatrix->m11 * matrix.m_sharedMatrix->m21 + m_sharedMatrix->m21 * matrix.m_sharedMatrix->m22 + m_sharedMatrix->m31 * matrix.m_sharedMatrix->m23 + m_sharedMatrix->m41 * matrix.m_sharedMatrix->m24;
	T r22 = m_sharedMatrix->m12 * matrix.m_sharedMatrix->m21 + m_sharedMatrix->m22 * matrix.m_sharedMatrix->m22 + m_sharedMatrix->m32 * matrix.m_sharedMatrix->m23 + m_sharedMatrix->m42 * matrix.m_sharedMatrix->m24;
	T r23 = m_sharedMatrix->m13 * matrix.m_sharedMatrix->m21 + m_sharedMatrix->m23 * matrix.m_sharedMatrix->m22 + m_sharedMatrix->m33 * matrix.m_sharedMatrix->m23 + m_sharedMatrix->m43 * matrix.m_sharedMatrix->m24;
	T r24 = m_sharedMatrix->m14 * matrix.m_sharedMatrix->m21 + m_sharedMatrix->m24 * matrix.m_sharedMatrix->m22 + m_sharedMatrix->m34 * matrix.m_sharedMatrix->m23 + m_sharedMatrix->m44 * matrix.m_sharedMatrix->m24;
	T r31 = m_sharedMatrix->m11 * matrix.m_sharedMatrix->m31 + m_sharedMatrix->m21 * matrix.m_sharedMatrix->m32 + m_sharedMatrix->m31 * matrix.m_sharedMatrix->m33 + m_sharedMatrix->m41 * matrix.m_sharedMatrix->m34;
	T r32 = m_sharedMatrix->m12 * matrix.m_sharedMatrix->m31 + m_sharedMatrix->m22 * matrix.m_sharedMatrix->m32 + m_sharedMatrix->m32 * matrix.m_sharedMatrix->m33 + m_sharedMatrix->m42 * matrix.m_sharedMatrix->m34;
	T r33 = m_sharedMatrix->m13 * matrix.m_sharedMatrix->m31 + m_sharedMatrix->m23 * matrix.m_sharedMatrix->m32 + m_sharedMatrix->m33 * matrix.m_sharedMatrix->m33 + m_sharedMatrix->m43 * matrix.m_sharedMatrix->m34;
	T r34 = m_sharedMatrix->m14 * matrix.m_sharedMatrix->m31 + m_sharedMatrix->m24 * matrix.m_sharedMatrix->m32 + m_sharedMatrix->m34 * matrix.m_sharedMatrix->m33 + m_sharedMatrix->m44 * matrix.m_sharedMatrix->m34;
	T r41 = m_sharedMatrix->m11 * matrix.m_sharedMatrix->m41 + m_sharedMatrix->m21 * matrix.m_sharedMatrix->m42 + m_sharedMatrix->m31 * matrix.m_sharedMatrix->m43 + m_sharedMatrix->m41 * matrix.m_sharedMatrix->m44;
	T r42 = m_sharedMatrix->m12 * matrix.m_sharedMatrix->m41 + m_sharedMatrix->m22 * matrix.m_sharedMatrix->m42 + m_sharedMatrix->m32 * matrix.m_sharedMatrix->m43 + m_sharedMatrix->m42 * matrix.m_sharedMatrix->m44;
	T r43 = m_sharedMatrix->m13 * matrix.m_sharedMatrix->m41 + m_sharedMatrix->m23 * matrix.m_sharedMatrix->m42 + m_sharedMatrix->m33 * matrix.m_sharedMatrix->m43 + m_sharedMatrix->m43 * matrix.m_sharedMatrix->m44;
	T r44 = m_sharedMatrix->m14 * matrix.m_sharedMatrix->m41 + m_sharedMatrix->m24 * matrix.m_sharedMatrix->m42 + m_sharedMatrix->m34 * matrix.m_sharedMatrix->m43 + m_sharedMatrix->m44 * matrix.m_sharedMatrix->m44;

	// Et puis on affecte
	Set(r11, r12, r13, r14,
		r21, r22, r23, r24,
		r31, r32, r33, r34,
		r41, r42, r43, r44);

	return *this;
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::operator*=(T scalar)
{
	#if NAZARA_MATH_SAFE
	if (!m_sharedMatrix)
	{
		NazaraError("Undefined matrix");
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
NzMatrix4<T> NzMatrix4<T>::LookAt(const NzVector3<T>& eye, const NzVector3<T>& center, const NzVector3<T>& up)
{
	NzMatrix4 matrix;
	matrix.SetLookAt(eye, center, up);

	return matrix;
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::Ortho(T left, T top, T width, T height, T zNear, T zFar)
{
	NzMatrix4 matrix;
	matrix.SetOrtho(left, top, width, height, zNear, zFar);

	return matrix;
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::Perspective(T angle, T ratio, T zNear, T zFar)
{
	NzMatrix4 matrix;
	matrix.SetPerspective(angle, ratio, zNear, zFar);

	return matrix;
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::Rotate(const NzQuaternion<T>& rotation)
{
	NzMatrix4 matrix;
	matrix.SetRotation(rotation);

	return matrix;
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::Scale(const NzVector3<T>& scale)
{
	NzMatrix4 matrix;
	matrix.SetScale(scale);

	return matrix;
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::Translate(const NzVector3<T>& translation)
{
	NzMatrix4 mat;
	mat.SetTranslation(translation);

	return mat;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzMatrix4<T>& matrix)
{
	return out << matrix.ToString();
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
	m_sharedMatrix->refCount--;
	NazaraMutexUnlock(m_sharedMatrix->mutex);

	if (m_sharedMatrix->refCount == 0)
		delete m_sharedMatrix;

	m_sharedMatrix = nullptr;
}

#undef NAZARA_MATRIX4_INL

#include <Nazara/Core/DebugOff.hpp>
