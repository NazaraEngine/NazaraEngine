// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Basic.hpp>
#include <Nazara/Math/EulerAngles.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <cmath>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <Nazara/Core/Debug.hpp>

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
NzMatrix4<T>::NzMatrix4(T matrix[16])
{
	Set(matrix);
}

template<typename T>
template<typename U>
NzMatrix4<T>::NzMatrix4(const NzMatrix4<U>& mat)
{
	Set(mat);
}

template<typename T>
T NzMatrix4<T>::GetDeterminant() const
{
	T A = m22 * (m33 * m44 - m43 * m34) - m32 * (m23 * m44 - m43 * m24) + m42 * (m23 * m34 - m33 * m24);
	T B = m12 * (m33 * m44 - m43 * m34) - m32 * (m13 * m44 - m43 * m14) + m42 * (m13 * m34 - m33 * m14);
	T C = m12 * (m23 * m44 - m43 * m24) - m22 * (m13 * m44 - m43 * m14) + m42 * (m13 * m24 - m23 * m14);
	T D = m12 * (m23 * m34 - m33 * m24) - m22 * (m13 * m34 - m33 * m14) + m32 * (m13 * m24 - m23 * m14);

	return m11 * A - m21 * B + m31 * C - m41 * D;
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::GetInverse() const
{
	NzMatrix4 mat;
	T det = GetDeterminant();

	if (det != 0.0)
	{
		mat.m11 =  (m22 * (m33 * m44 - m34 * m43) - m32 * (m23 * m44 - m43 * m24) + m42 * (m23 * m34 - m33 * m24)) / det;
		mat.m12 = -(m12 * (m33 * m44 - m43 * m34) - m32 * (m13 * m44 - m43 * m14) + m42 * (m13 * m34 - m33 * m14)) / det;
		mat.m13 =  (m12 * (m23 * m44 - m43 * m24) - m22 * (m13 * m44 - m43 * m14) + m42 * (m13 * m24 - m23 * m14)) / det;
		mat.m14 = -(m12 * (m23 * m34 - m33 * m24) - m22 * (m13 * m34 - m33 * m14) + m32 * (m13 * m24 - m23 * m14)) / det;

		mat.m21 = -(m21 * (m33 * m44 - m34 * m43) - m23 * (m31 * m44 - m34 * m41) + m24 * (m31 * m43 - m33 * m41)) / det;
		mat.m22 =  (m11 * (m33 * m44 - m34 * m43) - m13 * (m31 * m44 - m34 * m41) + m14 * (m31 * m43 - m33 * m41)) / det;
		mat.m23 = -(m11 * (m23 * m44 - m24 * m43) - m13 * (m21 * m44 - m24 * m41) + m14 * (m21 * m43 - m23 * m41)) / det;
		mat.m24 =  (m11 * (m23 * m34 - m24 * m33) - m13 * (m21 * m34 - m24 * m31) + m14 * (m21 * m33 - m23 * m31)) / det;

		mat.m31 =  (m21 * (m32 * m44 - m34 * m42) - m22 * (m31 * m44 - m34 * m41) + m24 * (m31 * m42 - m32 * m41)) / det;
		mat.m32 = -(m11 * (m32 * m44 - m34 * m42) - m12 * (m31 * m44 - m34 * m41) + m14 * (m31 * m42 - m32 * m41)) / det;
		mat.m33 =  (m11 * (m22 * m44 - m24 * m42) - m12 * (m21 * m44 - m24 * m41) + m14 * (m21 * m42 - m22 * m41)) / det;
		mat.m34 = -(m11 * (m22 * m34 - m24 * m32) - m12 * (m21 * m34 - m24 * m31) + m14 * (m21 * m32 - m22 * m31)) / det;

		mat.m41 = -(m21 * (m32 * m43 - m33 * m42) - m22 * (m31 * m43 - m33 * m41) + m23 * (m31 * m42 - m32 * m41)) / det;
		mat.m42 =  (m11 * (m32 * m43 - m33 * m42) - m12 * (m31 * m43 - m33 * m41) + m13 * (m31 * m42 - m32 * m41)) / det;
		mat.m43 = -(m11 * (m22 * m43 - m23 * m42) - m12 * (m21 * m43 - m23 * m41) + m13 * (m21 * m42 - m22 * m41)) / det;
		mat.m44 =  (m11 * (m22 * m33 - m23 * m32) - m12 * (m21 * m33 - m23 * m31) + m13 * (m21 * m32 - m22 * m31)) / det;
	}

	return mat;
}

template<typename T>
NzVector3<T> NzMatrix4<T>::GetScale() const
{
	return NzVector3<T>(std::sqrt(m11 * m11 + m21 * m21 + m31 * m31),
						std::sqrt(m12 * m12 + m22 * m22 + m32 * m32),
						std::sqrt(m13 * m13 + m23 * m23 + m33 * m33));
}

template<typename T>
NzVector3<T> NzMatrix4<T>::GetTranslation() const
{
	#if NAZARA_MATH_MATRIX_COLUMN_MAJOR
	return NzVector3<T>(m41, m42, m43);
	#else
	return NzVector3<T>(m14, m24, m34);
	#endif
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::GetTransposed() const
{
	return NzMatrix4(m11, m21, m31, m41,
					 m12, m22, m32, m42,
					 m13, m23, m33, m43,
					 m14, m24, m34, m44);
}

template<typename T>
bool NzMatrix4<T>::HasNegativeScale() const
{
	return GetDeterminant() < 0.f;
}

template<typename T>
bool NzMatrix4<T>::HasScale() const
{
	T t = m11 * m11 + m21 * m21 + m31 * m31;
	if (1.0 - t > std::numeric_limits<T>::epsilon())
		return true;

	t = m12 * m12 + m22 * m22 + m32 * m32;
	if (1.0 - t > std::numeric_limits<T>::epsilon())
		return true;

	t = m13 * m13 + m23 * m23 + m33 * m33;
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
}

template<typename T> void NzMatrix4<T>::Set(T matrix[16])
{
	// Ici nous sommes certains de la continuité des éléments en mémoire
	std::memcpy(&m41, matrix, 16*sizeof(T));
}

template<typename T> void NzMatrix4<T>::Set(const NzMatrix4& mat)
{
	// Pareil
	std::memcpy(&m41, &mat.m41, 16*sizeof(T));
}

template<typename T>
template<typename U>
void NzMatrix4<T>::Set(const NzMatrix4<U>& mat)
{
	m11 = static_cast<T>(mat.m11);
	m12 = static_cast<T>(mat.m12);
	m13 = static_cast<T>(mat.m13);
	m14 = static_cast<T>(mat.m14);
	m21 = static_cast<T>(mat.m21);
	m22 = static_cast<T>(mat.m22);
	m23 = static_cast<T>(mat.m23);
	m24 = static_cast<T>(mat.m24);
	m31 = static_cast<T>(mat.m31);
	m32 = static_cast<T>(mat.m32);
	m33 = static_cast<T>(mat.m33);
	m34 = static_cast<T>(mat.m34);
	m41 = static_cast<T>(mat.m41);
	m42 = static_cast<T>(mat.m42);
	m43 = static_cast<T>(mat.m43);
	m44 = static_cast<T>(mat.m44);
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

	T f = 1 / std::tan(angle);

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
	NzStringStream ss;

	return ss << "Matrix4(" << m11 << ", " << m12 << ", " << m13 << ", " << m14 << ",\n"
			  << "        " << m21 << ", " << m22 << ", " << m23 << ", " << m24 << ",\n"
			  << "        " << m31 << ", " << m32 << ", " << m33 << ", " << m34 << ",\n"
			  << "        " << m41 << ", " << m42 << ", " << m43 << ", " << m44 << ')';
}

template<typename T>
NzVector2<T> NzMatrix4<T>::Transform(const NzVector2<T>& vector, T z, T w) const
{
	return NzVector2<T>(m11 * vector.x + m12 * vector.y + m13 * z + m14 * w,
						m21 * vector.x + m22 * vector.y + m23 * z + m24 * w);
}

template<typename T>
NzVector3<T> NzMatrix4<T>::Transform(const NzVector3<T>& vector, T w) const
{
	return NzVector3<T>(m11 * vector.x + m12 * vector.y + m13 * vector.z + m14 * w,
						m21 * vector.x + m22 * vector.y + m23 * vector.z + m24 * w,
						m31 * vector.x + m32 * vector.y + m33 * vector.z + m34 * w);
}

template<typename T>
NzVector4<T> NzMatrix4<T>::Transform(const NzVector4<T>& vector) const
{
	return NzVector4<T>(m11 * vector.x + m12 * vector.y + m13 * vector.z + m14 * vector.w,
						m21 * vector.x + m22 * vector.y + m23 * vector.z + m24 * vector.w,
						m31 * vector.x + m32 * vector.y + m33 * vector.z + m34 * vector.w,
						m41 * vector.x + m42 * vector.y + m43 * vector.z + m44 * vector.w);
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::Transpose()
{
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
		ss << __FILE__ << ':' << __LINE__ << ": Index out of range: (" << x << ", " << y << ") > (3,3)";

		throw std::out_of_range(ss.ToString());
	}
	#endif

	return (&m11)[x*4+y];
}

template<typename T>
const T& NzMatrix4<T>::operator()(unsigned int x, unsigned int y) const
{
	#if NAZARA_MATH_SAFE
	if (x > 3 || y > 3)
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Index out of range: (" << x << ", " << y << ") > (3,3)";

		throw std::out_of_range(ss.ToString());
	}
	#endif

	return (&m11)[x*4+y];
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::operator*(const NzMatrix4& mat) const
{
	NzMatrix4 matrix;
	for(int k = 0; k < 4; k++)
    {
        for(int j = 0; j < 4; j++)
        {
            for(int i = 0; i < 4; i++)
                matrix(j, k) += (*this)(j, i) * mat(i, k);
        }
    }

    return matrix;
	/*return NzMatrix4(m11 * mat.m11 + m21 * mat.m12 + m31 * mat.m13 + m41 * mat.m14,
				     m12 * mat.m11 + m22 * mat.m12 + m32 * mat.m13 + m42 * mat.m14,
					 m13 * mat.m11 + m23 * mat.m12 + m33 * mat.m13 + m43 * mat.m14,
				     m14 * mat.m11 + m24 * mat.m12 + m34 * mat.m13 + m44 * mat.m14,
				     m11 * mat.m21 + m21 * mat.m22 + m31 * mat.m23 + m41 * mat.m24,
				     m12 * mat.m21 + m22 * mat.m22 + m32 * mat.m23 + m42 * mat.m24,
				     m13 * mat.m21 + m23 * mat.m22 + m33 * mat.m23 + m43 * mat.m24,
				     m14 * mat.m21 + m24 * mat.m22 + m34 * mat.m23 + m44 * mat.m24,
				     m11 * mat.m31 + m21 * mat.m32 + m31 * mat.m33 + m41 * mat.m34,
				     m12 * mat.m31 + m22 * mat.m32 + m32 * mat.m33 + m42 * mat.m34,
				     m13 * mat.m31 + m23 * mat.m32 + m33 * mat.m33 + m43 * mat.m34,
				     m14 * mat.m31 + m24 * mat.m32 + m34 * mat.m33 + m44 * mat.m34,
				     m11 * mat.m41 + m21 * mat.m42 + m31 * mat.m43 + m41 * mat.m44,
				     m12 * mat.m41 + m22 * mat.m42 + m32 * mat.m43 + m42 * mat.m44,
				     m13 * mat.m41 + m23 * mat.m42 + m33 * mat.m43 + m43 * mat.m44,
				     m14 * mat.m41 + m24 * mat.m42 + m34 * mat.m43 + m44 * mat.m44);*/

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
	return NzMatrix4(m11 * scalar, m12 * scalar, m13 * scalar, m14 * scalar,
				   m21 * scalar, m22 * scalar, m23 * scalar, m24 * scalar,
				   m31 * scalar, m32 * scalar, m33 * scalar, m34 * scalar,
				   m41 * scalar, m42 * scalar, m43 * scalar, m44 * scalar);
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::operator*=(const NzMatrix4& mat)
{
	T r11 = m11 * mat.m11 + m21 * mat.m12 + m31 * mat.m13 + m41 * mat.m14;
	T r12 = m12 * mat.m11 + m22 * mat.m12 + m32 * mat.m13 + m42 * mat.m14;
	T r13 = m13 * mat.m11 + m23 * mat.m12 + m33 * mat.m13 + m43 * mat.m14;
	T r14 = m14 * mat.m11 + m24 * mat.m12 + m34 * mat.m13 + m44 * mat.m14;
	T r21 = m11 * mat.m21 + m21 * mat.m22 + m31 * mat.m23 + m41 * mat.m24;
	T r22 = m12 * mat.m21 + m22 * mat.m22 + m32 * mat.m23 + m42 * mat.m24;
	T r23 = m13 * mat.m21 + m23 * mat.m22 + m33 * mat.m23 + m43 * mat.m24;
	T r24 = m14 * mat.m21 + m24 * mat.m22 + m34 * mat.m23 + m44 * mat.m24;
	T r31 = m11 * mat.m31 + m21 * mat.m32 + m31 * mat.m33 + m41 * mat.m34;
	T r32 = m12 * mat.m31 + m22 * mat.m32 + m32 * mat.m33 + m42 * mat.m34;
	T r33 = m13 * mat.m31 + m23 * mat.m32 + m33 * mat.m33 + m43 * mat.m34;
	T r34 = m14 * mat.m31 + m24 * mat.m32 + m34 * mat.m33 + m44 * mat.m34;
	T r41 = m11 * mat.m41 + m21 * mat.m42 + m31 * mat.m43 + m41 * mat.m44;
	T r42 = m12 * mat.m41 + m22 * mat.m42 + m32 * mat.m43 + m42 * mat.m44;
	T r43 = m13 * mat.m41 + m23 * mat.m42 + m33 * mat.m43 + m43 * mat.m44;

	m44 = m14 * mat.m41 + m24 * mat.m42 + m34 * mat.m43 + m44 * mat.m44;
	m43 = r43;
	m42 = r42;
	m41 = r41;
	m34 = r34;
	m33 = r33;
	m32 = r32;
	m31 = r31;
	m24 = r24;
	m23 = r23;
	m22 = r22;
	m21 = r21;
	m14 = r14;
	m13 = r13;
	m12 = r12;
	m11 = r11;

	return *this;
}

template<typename T>
NzMatrix4<T>& NzMatrix4<T>::operator*=(T scalar)
{
	m11 *= scalar;
	m12 *= scalar;
	m13 *= scalar;
	m14 *= scalar;
	m21 *= scalar;
	m22 *= scalar;
	m23 *= scalar;
	m24 *= scalar;
	m31 *= scalar;
	m32 *= scalar;
	m33 *= scalar;
	m34 *= scalar;
	m41 *= scalar;
	m42 *= scalar;
	m43 *= scalar;
	m44 *= scalar;

	return *this;
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::LookAt(const NzVector3<T>& eye, const NzVector3<T>& center, const NzVector3<T>& up)
{
	NzMatrix4 mat;
	mat.SetLookAt(eye, center, up);

	return mat;
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::Perspective(T angle, T ratio, T zNear, T zFar)
{
	NzMatrix4 mat;
	mat.SetPerspective(angle, ratio, zNear, zFar);

	return mat;
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::Rotate(const NzQuaternion<T>& rotation)
{
	NzMatrix4 mat;
	mat.SetRotation(rotation);

	return mat;
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::Scale(const NzVector3<T>& scale)
{
	NzMatrix4 mat;
	mat.SetScale(scale);

	return mat;
}

template<typename T>
NzMatrix4<T> NzMatrix4<T>::Translate(const NzVector3<T>& translation)
{
	NzMatrix4 mat;
	mat.SetTranslation(translation);

	return mat;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzMatrix4<T>& mat)
{
	return out << mat.ToString();
}

#include <Nazara/Core/DebugOff.hpp>
