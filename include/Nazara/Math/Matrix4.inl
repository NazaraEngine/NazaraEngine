// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <Nazara/Math/Config.hpp>
#include <Nazara/Math/EulerAngles.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <cstring>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{

	/*!
    * \ingroup math
	* \class Nz::Matrix4
	* \brief Math class that represents a transformation of the four dimensional vector space with the notion of projectivity
	*
	* \remark Matrix4 is said to be "row-major" and affine if last column is made of (0, 0, 0, 1)
	*/

	/*!
	* \brief Constructs a Matrix4 object from its components
	*
	* \param rIJ Matrix components at index(I, J)
	*/

	template<typename T>
	Matrix4<T>::Matrix4(T r11, T r12, T r13, T r14,
	                    T r21, T r22, T r23, T r24,
	                    T r31, T r32, T r33, T r34,
	                    T r41, T r42, T r43, T r44)
	{
		Set(r11, r12, r13, r14,
		    r21, r22, r23, r24,
		    r31, r32, r33, r34,
		    r41, r42, r43, r44);
	}

	/*!
	* \brief Constructs a Matrix4 object from an array of sixteen elements
	*
	* \param matrix[16] Matrix components
	*/

	template<typename T>
	Matrix4<T>::Matrix4(const T matrix[16]) :
	Matrix4(matrix[ 0], matrix[ 1], matrix[ 2], matrix[ 3],
	        matrix[ 4], matrix[ 5], matrix[ 6], matrix[ 7],
	        matrix[ 8], matrix[ 9], matrix[10], matrix[11],
	        matrix[12], matrix[13], matrix[14], matrix[15])
	{
	}

	/*!
	* \brief Constructs a Matrix4 object from another type of Matrix4
	*
	* \param matrix Matrix4 of type U to convert to type T
	*/

	template<typename T>
	template<typename U>
	Matrix4<T>::Matrix4(const Matrix4<U>& matrix)
	{
		Set(matrix);
	}

	/*!
	* \brief Apply the rotation represented by the quaternion to this matrix
	* \return A reference to this matrix which has been rotated
	*
	* \param rotation Quaternion representing a rotation of space
	*/

	template<typename T>
	Matrix4<T>& Matrix4<T>::ApplyRotation(const Quaternion<T>& rotation)
	{
		return Concatenate(Matrix4<T>::Rotate(rotation));
	}

	/*!
	* \brief Apply the scale represented by the vector to this matrix
	* \return A reference to this matrix which has been scaled
	*
	* \param scale Vector3 representing the homothety
	*/

	template<typename T>
	Matrix4<T>& Matrix4<T>::ApplyScale(const Vector3<T>& scale)
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

	/*!
	* \brief Apply the translation represented by the vector to this matrix
	* \return A reference to this matrix which has been translated
	*
	* \param translation Vector3 representing the translation
	*/

	template<typename T>
	Matrix4<T>& Matrix4<T>::ApplyTranslation(const Vector3<T>& translation)
	{
		m41 += translation.x;
		m42 += translation.y;
		m43 += translation.z;

		return *this;
	}

	/*!
	* \brief Concatenates this matrix to other one
	* \return A reference to this matrix which is the product with other one
	*
	* \param matrix Matrix to multiply with
	*
	* \remark if NAZARA_MATH_MATRIX4_CHECK_AFFINE is defined, ConcatenateAffine is called
	*
	* \see ConcatenateAffine
	*/

	template<typename T>
	Matrix4<T>& Matrix4<T>::Concatenate(const Matrix4& matrix)
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

	/*!
	* \brief Concatenates this matrix to other one
	* \return A reference to this matrix which is the product with other one
	*
	* \param matrix Matrix to multiply with
	*
	* \remark if NAZARA_DEBUG is defined and matrices are not affine, a NazaraWarning is produced and Concatenate is called
	*
	* \see Concatenate
	*/

	template<typename T>
	Matrix4<T>& Matrix4<T>::ConcatenateAffine(const Matrix4& matrix)
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
		           T(0.0),

		           m21*matrix.m11 + m22*matrix.m21 + m23*matrix.m31,
		           m21*matrix.m12 + m22*matrix.m22 + m23*matrix.m32,
		           m21*matrix.m13 + m22*matrix.m23 + m23*matrix.m33,
		           T(0.0),

		           m31*matrix.m11 + m32*matrix.m21 + m33*matrix.m31,
		           m31*matrix.m12 + m32*matrix.m22 + m33*matrix.m32,
		           m31*matrix.m13 + m32*matrix.m23 + m33*matrix.m33,
		           T(0.0),

		           m41*matrix.m11 + m42*matrix.m21 + m43*matrix.m31 + matrix.m41,
		           m41*matrix.m12 + m42*matrix.m22 + m43*matrix.m32 + matrix.m42,
		           m41*matrix.m13 + m42*matrix.m23 + m43*matrix.m33 + matrix.m43,
		           T(1.0));
	}

	template<typename T>
	void Matrix4<T>::Decompose(Vector3<T>& translation, Quaternion<T>& rotation, Vector3<T>& scale)
	{
		Matrix4f localMat(*this);

		translation = localMat.GetTranslation();
		scale = localMat.GetScale();

		Vector3<T> invScale;
		invScale.x = T(1) / scale.x;
		invScale.y = T(1) / scale.y;
		invScale.z = T(1) / scale.z;

		localMat.ApplyScale(invScale);

		rotation = localMat.GetRotation();
	}


	/*!
	* \brief Gets the ith column of the matrix
	* \return Vector4 which is the transformation of this axis
	*
	* \param column Index of the column you want
	*
	* \remark Produce a NazaraError if you try to access index greater than 3 with NAZARA_MATH_SAFE defined
	* \throw std::out_of_range if NAZARA_MATH_SAFE is defined and if you try to access index greater than 3
	*/

	template<typename T>
	Vector4<T> Matrix4<T>::GetColumn(unsigned int column) const
	{
		///FIXME: Est-ce une bonne idée de gérer la matrice de cette façon ?

		#if NAZARA_MATH_SAFE
		if (column > 3)
		{
			std::string error("Column out of range: (" + std::to_string(column) + ") > 3");

			NazaraError(error);
			throw std::out_of_range(error);
		}
		#endif

		const T* ptr = &m11 + column * 4;
		return Vector4<T>(ptr[0], ptr[1], ptr[2], ptr[3]);
	}

	/*!
	* \brief Calcultes the determinant of this matrix
	* \return The value of the determinant
	*
	* \remark if NAZARA_MATH_MATRIX4_CHECK_AFFINE is defined, GetDeterminantAffine is called
	*
	* \see GetDeterminantAffine
	*/

	template<typename T>
	T Matrix4<T>::GetDeterminant() const
	{
		#if NAZARA_MATH_MATRIX4_CHECK_AFFINE
		if (IsAffine())
			return GetDeterminantAffine();
		#endif

		T A = m22*(m33*m44 - m43*m34) - m32*(m23*m44 - m43*m24) + m42*(m23*m34 - m33*m24);
		T B = m12*(m33*m44 - m43*m34) - m32*(m13*m44 - m43*m14) + m42*(m13*m34 - m33*m14);
		T C = m12*(m23*m44 - m43*m24) - m22*(m13*m44 - m43*m14) + m42*(m13*m24 - m23*m14);
		T D = m12*(m23*m34 - m33*m24) - m22*(m13*m34 - m33*m14) + m32*(m13*m24 - m23*m14);

		return m11*A - m21*B + m31*C - m41*D;
	}

	/*!
	* \brief Calcultes the determinant of this matrix
	* \return The value of the determinant
	*
	* \remark if NAZARA_DEBUG is defined and matrix is not affine, a NazaraWarning is produced and GetDeterminant is called
	*
	* \see GetDeterminant
	*/

	template<typename T>
	T Matrix4<T>::GetDeterminantAffine() const
	{
		#ifdef NAZARA_DEBUG
		if (!IsAffine())
		{
			NazaraWarning("First matrix not affine");
			return GetDeterminant();
		}
		#endif

		T A = m22*m33 - m32*m23;
		T B = m12*m33 - m32*m13;
		T C = m12*m23 - m22*m13;

		return m11*A - m21*B + m31*C;
	}

	/*!
	* \brief Gets the inverse of this matrix
	* \return true if matrix can be inverted
	*
	* \param dest Matrix to put the result
	*
	* \remark You can call this method on the same object
	* \remark if NAZARA_MATH_MATRIX4_CHECK_AFFINE is defined, GetInverseAffine is called
	* \remark if NAZARA_DEBUG is defined, a NazaraError is produced if dest is null and false is returned
	*
	* \see GetInverseAffine
	*/

	template<typename T>
	bool Matrix4<T>::GetInverse(Matrix4* dest) const
	{
		#if NAZARA_MATH_MATRIX4_CHECK_AFFINE
		if (IsAffine())
			return GetInverseAffine(dest);
		#endif

		#ifdef NAZARA_DEBUG
		if (!dest)
		{
			NazaraError("Destination matrix must be valid");
			return false;
		}
		#endif

		T det = GetDeterminant();
		if (det != T(0.0))
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

			T invDet = T(1.0) / det;
			for (unsigned int i = 0; i < 16; ++i)
				inv[i] *= invDet;

			*dest = inv;
			return true;
		}
		else
			return false;
	}

	/*!
	* \brief Gets the inverse of this matrix
	* \return true if matrix can be inverted
	*
	* \param dest Matrix to put the result
	*
	* \remark You can call this method on the same object
	* \remark if NAZARA_DEBUG is defined and matrix is not affine, a NazaraWarning is produced and GetInverse is called
	* \remark if NAZARA_DEBUG is defined, a NazaraError is produced if dest is null and false is returned
	*
	* \see GetInverse
	*/

	template<typename T>
	bool Matrix4<T>::GetInverseAffine(Matrix4* dest) const
	{
		#ifdef NAZARA_DEBUG
		if (!IsAffine())
		{
			NazaraWarning("Matrix is not affine");
			return GetInverse(dest);
		}

		if (!dest)
		{
			NazaraError("Destination matrix must be valid");
			return false;
		}
		#endif

		T det = GetDeterminantAffine();
		if (det != T(0.0))
		{
			// http://stackoverflow.com/questions/1148309/inverting-a-4x4-matrix
			T inv[16];
			inv[0] = m22 * m33 -
			         m32 * m23;

			inv[1] = -m12 * m33 +
			         m32 * m13;

			inv[2] = m12 * m23 -
			         m22 * m13;

			inv[3] = T(0.0);

			inv[4] = -m21 * m33 +
			         m31 * m23;

			inv[5] = m11 * m33 -
			         m31 * m13;

			inv[6] = -m11 * m23 +
			         m21 * m13;

			inv[7] = T(0.0);

			inv[8] = m21 * m32 -
			         m31 * m22;

			inv[9] = -m11 * m32 +
			         m31 * m12;

			inv[10] = m11 * m22 -
			          m21 * m12;

			inv[11] = T(0.0);

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

			T invDet = T(1.0) / det;
			for (unsigned int i = 0; i < 16; ++i)
				inv[i] *= invDet;

			inv[15] = T(1.0);

			*dest = inv;
			return true;
		}
		else
			return false;
	}

	/*!
	* \brief Gets the rotation from this matrix
	* \return Quaternion which is the representation of the rotation in this matrix
	*/

	template<typename T>
	Quaternion<T> Matrix4<T>::GetRotation() const
	{
		// http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
		Quaternion<T> quat;

		T trace = m11 + m22 + m33;
		if (trace > T(0.0))
		{
			T s = T(0.5) / std::sqrt(trace + T(1.0));
			quat.w = T(0.25) / s;
			quat.x = (m23 - m32) * s;
			quat.y = (m31 - m13) * s;
			quat.z = (m12 - m21) * s;
		}
		else
		{
			if (m11 > m22 && m11 > m33)
			{
				T s = T(2.0) * std::sqrt(T(1.0) + m11 - m22 - m33);

				quat.w = (m23 - m32) / s;
				quat.x = T(0.25) * s;
				quat.y = (m21 + m12) / s;
				quat.z = (m31 + m13) / s;
			}
			else if (m22 > m33)
			{
				T s = T(2.0) * std::sqrt(T(1.0) + m22 - m11 - m33);

				quat.w = (m31 - m13) / s;
				quat.x = (m21 + m12) / s;
				quat.y = T(0.25) * s;
				quat.z = (m32 + m23) / s;
			}
			else
			{
				T s = T(2.0) * std::sqrt(T(1.0) + m33 - m11 - m22);

				quat.w = (m12 - m21) / s;
				quat.x = (m31 + m13) / s;
				quat.y = (m32 + m23) / s;
				quat.z = T(0.25) * s;
			}
		}

		return quat;
	}

	/*!
	* \brief Gets the ith row of the matrix
	* \return Vector4 which is the ith row of the matrix
	*
	* \param row Index of the row you want
	*
	* \remark Produce a NazaraError if you try to access index greater than 3 with NAZARA_MATH_SAFE defined
	* \throw std::out_of_range if NAZARA_MATH_SAFE is defined and if you try to access index greater than 3
	*/

	template<typename T>
	Vector4<T> Matrix4<T>::GetRow(unsigned int row) const
	{
		///FIXME: Est-ce une bonne idée de gérer la matrice de cette façon ?

		#if NAZARA_MATH_SAFE
		if (row > 3)
		{
			std::string error("Row out of range: (" + NumberToString(row) + ") > 3");

			NazaraError(error);
			throw std::out_of_range(error);
		}
		#endif

		const T* ptr = &m11;
		return Vector4<T>(ptr[row], ptr[row+4], ptr[row+8], ptr[row+12]);
	}

	/*!
	* \brief Gets the scale from this matrix
	* \return Vector3 which is the representation of the scale in this matrix
	*
	* \see GetSquaredScale
	*/

	template<typename T>
	Vector3<T> Matrix4<T>::GetScale() const
	{
		Vector3<T> squaredScale = GetSquaredScale();
		return Vector3<T>(std::sqrt(squaredScale.x), std::sqrt(squaredScale.y), std::sqrt(squaredScale.z));
	}

	/*!
	* \brief Gets the squared scale from this matrix
	* \return Vector3 which is the representation of the squared scale in this matrix
	*
	* \see GetScale
	*/

	template<typename T>
	Vector3<T> Matrix4<T>::GetSquaredScale() const
	{
		return Vector3<T>(m11 * m11 + m12 * m12 + m13 * m13,
		                  m21 * m21 + m22 * m22 + m23 * m23,
		                  m31 * m31 + m32 * m32 + m33 * m33);
	}

	/*!
	* \brief Gets the translation from this matrix
	* \return Vector3 which is the representation of the translation in this matrix
	*/

	template<typename T>
	Vector3<T> Matrix4<T>::GetTranslation() const
	{
		return Vector3<T>(m41, m42, m43);
	}

	/*!
	* \brief Gets the transposed of this matrix
	*
	* \param dest Matrix to put the result
	*
	* \remark You can call this method on the same object
	* \remark if NAZARA_DEBUG is defined, a NazaraError is produced if dest is null and dest is not changed
	*
	* \see Transpose
	*/

	template<typename T>
	void Matrix4<T>::GetTransposed(Matrix4* dest) const
	{
		#ifdef NAZARA_DEBUG
		if (!dest)
		{
			NazaraError("Destination matrix must be valid");
			return;
		}
		#endif

		dest->Set(m11, m21, m31, m41,
		          m12, m22, m32, m42,
		          m13, m23, m33, m43,
		          m14, m24, m34, m44);
	}

	/*!
	* \brief Checks whetever matrix has negative scale
	* \return true if determinant is negative
	*
	* \see GetDeterminant
	*/

	template<typename T>
	bool Matrix4<T>::HasNegativeScale() const
	{
		return GetDeterminant() < T(0.0);
	}

	/*!
	* \brief Checks whetever matrix has scale
	* \return true if determinant has scale
	*
	* \see HasNegativeScale
	*/

	template<typename T>
	bool Matrix4<T>::HasScale() const
	{
		T t = m11*m11 + m21*m21 + m31*m31;
		if (!NumberEquals(t, T(1.0)))
			return true;

		t = m12*m12 + m22*m22 + m32*m32;
		if (!NumberEquals(t, T(1.0)))
			return true;

		t = m13*m13 + m23*m23 + m33*m33;
		if (!NumberEquals(t, T(1.0)))
			return true;

		return false;
	}

	/*!
	* \brief Inverts this matrix
	* \return A reference to this matrix inverted
	*
	* \param succeeded Optional argument to know if matrix has been successfully inverted
	*
	* \see InverseAffine
	*/

	template<typename T>
	Matrix4<T>& Matrix4<T>::Inverse(bool* succeeded)
	{
		bool result = GetInverse(this);
		if (succeeded)
			*succeeded = result;

		return *this;
	}

	/*!
	* \brief Inverts this matrix
	* \return A reference to this matrix inverted
	*
	* \param succeeded Optional argument to know if matrix has been successfully inverted
	*
	* \see Inverse
	*/

	template<typename T>
	Matrix4<T>& Matrix4<T>::InverseAffine(bool* succeeded)
	{
		bool result = GetInverseAffine(this);
		if (succeeded)
			*succeeded = result;

		return *this;
	}

	/*!
	* \brief Checks whether the matrix is affine
	* \return true if matrix is affine
	*/

	template<typename T>
	bool Matrix4<T>::IsAffine() const
	{
		return NumberEquals(m14, T(0.0)) && NumberEquals(m24, T(0.0)) && NumberEquals(m34, T(0.0)) && NumberEquals(m44, T(1.0));
	}

	/*!
	* \brief Checks whether the matrix is identity
	* \return true if matrix is identity
	*/

	template<typename T>
	bool Matrix4<T>::IsIdentity() const
	{
		return (NumberEquals(m11, T(1.0)) && NumberEquals(m12, T(0.0)) && NumberEquals(m13, T(0.0)) && NumberEquals(m14, T(0.0)) &&
		        NumberEquals(m21, T(0.0)) && NumberEquals(m22, T(1.0)) && NumberEquals(m23, T(0.0)) && NumberEquals(m24, T(0.0)) &&
		        NumberEquals(m31, T(0.0)) && NumberEquals(m32, T(0.0)) && NumberEquals(m33, T(1.0)) && NumberEquals(m34, T(0.0)) &&
		        NumberEquals(m41, T(0.0)) && NumberEquals(m42, T(0.0)) && NumberEquals(m43, T(0.0)) && NumberEquals(m44, T(1.0)));
	}

	/*!
	* \brief Makes the matrix identity (with 1 on diagonal and 0 for others)
	* \return A reference to this matrix with components (1 on diagonal and 0 for others)
	*
	* \see Identity
	*/

	template<typename T>
	Matrix4<T>& Matrix4<T>::MakeIdentity()
	{
		Set(T(1.0), T(0.0), T(0.0), T(0.0),
		    T(0.0), T(1.0), T(0.0), T(0.0),
		    T(0.0), T(0.0), T(1.0), T(0.0),
		    T(0.0), T(0.0), T(0.0), T(1.0));

		return *this;
	}

	/*!
	* \brief Makes the matrix a 'look at matrix'
	* \return A reference to this matrix transformed in 'look at matrix'
	*
	* \param eye Position of the camera
	* \param target Position of the target of the camera
	* \param up Direction of up vector according to the orientation of camera
	*
	* \see LookAt
	*/

	template<typename T>
	Matrix4<T>& Matrix4<T>::MakeLookAt(const Vector3<T>& eye, const Vector3<T>& target, const Vector3<T>& up)
	{
		Vector3<T> f = Vector3<T>::Normalize(target - eye);
		Vector3<T> s = Vector3<T>::Normalize(f.CrossProduct(up));
		Vector3<T> u = s.CrossProduct(f);

		Set(s.x, u.x, -f.x, T(0.0),
		    s.y, u.y, -f.y, T(0.0),
		    s.z, u.z, -f.z, T(0.0),
		    -s.DotProduct(eye), -u.DotProduct(eye), f.DotProduct(eye), T(1.0));

		return *this;
	}

	/*!
	* \brief Makes the matrix a 'orthographic matrix'
	* \return A reference to this matrix transformed in 'orthographic matrix'
	*
	* \param left Distance between center and left
	* \param right Distance between center and right
	* \param top Distance between center and top
	* \param bottom Distance between center and bottom
	* \param zNear Distance where 'vision' begins
	* \param zFar Distance where 'vision' ends
	*
	* \see Ortho
	*/

	template<typename T>
	Matrix4<T>& Matrix4<T>::MakeOrtho(T left, T right, T top, T bottom, T zNear, T zFar)
	{
		// http://msdn.microsoft.com/en-us/library/windows/desktop/bb204942(v=vs.85).aspx
		Set(T(2.0) / (right - left), T(0.0), T(0.0), T(0.0),
		    T(0.0), T(2.0) / (top - bottom), T(0.0), T(0.0),
		    T(0.0), T(0.0), T(1.0) / (zNear - zFar), T(0.0),
		    (left + right) / (left - right), (top + bottom) / (bottom - top), zNear/(zNear - zFar), T(1.0));

		return *this;
	}

	/*!
	* \brief Makes the matrix a 'perspective matrix'
	* \return A reference to this matrix transformed in 'perspective matrix'
	*
	* \param angle FOV angle
	* \param ratio Rendering ratio (typically 16/9 or 4/3)
	* \param zNear Distance where 'vision' begins
	* \param zFar Distance where 'vision' ends
	*
	* \see Perspective
	*/

	template<typename T>
	Matrix4<T>& Matrix4<T>::MakePerspective(RadianAngle<T> angle, T ratio, T zNear, T zFar)
	{
		angle /= T(2.0);

		T yScale = angle.GetTan();

		Set(T(1.0) / (ratio * yScale), T(0.0),              T(0.0),                           T(0.0),
		    T(0.0),                    T(-1.0) / (yScale),  T(0.0),                           T(0.0),
		    T(0.0),                    T(0.0),              zFar / (zNear - zFar),            T(-1.0),
		    T(0.0),                    T(0.0),              -(zNear * zFar) / (zFar - zNear), T(0.0));

		return *this;
	}

	/*!
	* \brief Makes the matrix the representation of the quaternion
	* \return A reference to this matrix which is the rotation of the quaternion
	*
	* \param rotation Quaternion representing a rotation of space
	*
	* \see Rotate
	*/

	template<typename T>
	Matrix4<T>& Matrix4<T>::MakeRotation(const Quaternion<T>& rotation)
	{
		SetRotation(rotation);

		// We complete the matrix
		m14 = T(0.0);
		m24 = T(0.0);
		m34 = T(0.0);
		m41 = T(0.0);
		m42 = T(0.0);
		m43 = T(0.0);
		m44 = T(1.0);

		return *this;
	}

	/*!
	* \brief Makes the matrix with the scale
	* \return A reference to this matrix which is the scale
	*
	* \param scale Vector3 representing the homothety
	*
	* \see Scale
	*/

	template<typename T>
	Matrix4<T>& Matrix4<T>::MakeScale(const Vector3<T>& scale)
	{
		Set(scale.x, T(0.0),  T(0.0),  T(0.0),
		    T(0.0),  scale.y, T(0.0),  T(0.0),
		    T(0.0),  T(0.0),  scale.z, T(0.0),
		    T(0.0),  T(0.0),  T(0.0),  T(1.0));

		return *this;
	}

	/*!
	* \brief Makes the matrix with the translation
	* \return A reference to this matrix which is the translation
	*
	* \param translation Vector3 representing the translation
	*
	* \see Translate
	*/

	template<typename T>
	Matrix4<T>& Matrix4<T>::MakeTranslation(const Vector3<T>& translation)
	{
		Set(T(1.0), T(0.0), T(0.0), T(0.0),
		    T(0.0), T(1.0), T(0.0), T(0.0),
		    T(0.0), T(0.0), T(1.0), T(0.0),
		    translation.x, translation.y, translation.z, T(1.0));

		return *this;
	}

	/*!
	* \brief Makes the matrix with the translation and the rotation
	* \return A reference to this matrix which is transformation obtained by the translation and the rotation
	*
	* \param translation Vector3 representing the translation
	* \param rotation Quaternion representing a rotation of space
	*
	* \remark Rotation is applied first
	*
	* \see Transform
	*/

	template<typename T>
	Matrix4<T>& Matrix4<T>::MakeTransform(const Vector3<T>& translation, const Quaternion<T>& rotation)
	{
		// The rotation and the translation may be directly applied
		SetRotation(rotation);
		SetTranslation(translation);

		// We complete the matrix (the transformations are affine)
		m14 = T(0.0);
		m24 = T(0.0);
		m34 = T(0.0);
		m44 = T(1.0);

		return *this;
	}

	/*!
	* \brief Makes the matrix with the translation, the rotation and the scale
	* \return A reference to this matrix which is transformation obtained by the translation, the rotation and the scale
	*
	* \param translation Vector3 representing the translation
	* \param rotation Quaternion representing a rotation of space
	* \param scale Vector3 representing the homothety
	*
	* \remark Rotation is applied first, then translation
	*
	* \see Transform
	*/

	template<typename T>
	Matrix4<T>& Matrix4<T>::MakeTransform(const Vector3<T>& translation, const Quaternion<T>& rotation, const Vector3<T>& scale)
	{
		MakeTransform(translation, rotation);

		// Then we apply the homothety to current values
		return ApplyScale(scale);
	}

	/*!
	* \brief Makes the matrix a 'view matrix'
	* \return A reference to this matrix transformed in 'view matrix'
	*
	* \param translation Vector3 representing the translation
	* \param rotation Quaternion representing a rotation of space
	*
	* \see ViewMatrix
	*/

	template<typename T>
	Matrix4<T>& Matrix4<T>::MakeViewMatrix(const Vector3<T>& translation, const Quaternion<T>& rotation)
	{
		// A view matrix must apply an inverse transformation of the 'world' matrix
		Quaternion<T> invRot = rotation.GetConjugate(); // Inverse of the rotation

		return MakeTransform(-(invRot * translation), invRot);
	}

	/*!
	* \brief Makes the matrix zero (with 0 everywhere)
	* \return A reference to this matrix with components (0 everywhere)
	*
	* \see Zero
	*/

	template<typename T>
	Matrix4<T>& Matrix4<T>::MakeZero()
	{
		Set(T(0.0), T(0.0), T(0.0), T(0.0),
		    T(0.0), T(0.0), T(0.0), T(0.0),
		    T(0.0), T(0.0), T(0.0), T(0.0),
		    T(0.0), T(0.0), T(0.0), T(0.0));

		return *this;
	}

	/*!
	* \brief Sets the components of the matrix
	* \return A reference to this matrix
	*
	* \param rIJ Matrix components at index(I, J)
	*/

	template<typename T>
	Matrix4<T>& Matrix4<T>::Set(T r11, T r12, T r13, T r14,
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

	/*!
	* \brief Sets the components of the matrix from another type of Matrix4
	* \return A reference to this matrix
	*
	* \param matrix Matrix4 of type U to convert its components
	*/

	template<typename T>
	template<typename U>
	Matrix4<T>& Matrix4<T>::Set(const Matrix4<U>& matrix)
	{
		Set(T(matrix[ 0]), T(matrix[ 1]), T(matrix[ 2]), T(matrix[ 3]),
		    T(matrix[ 4]), T(matrix[ 5]), T(matrix[ 6]), T(matrix[ 7]),
		    T(matrix[ 8]), T(matrix[ 9]), T(matrix[10]), T(matrix[11]),
		    T(matrix[12]), T(matrix[13]), T(matrix[14]), T(matrix[15]));

		return *this;
	}

	/*!
	* \brief Sets the components of the matrix from a quaternion
	* \return A reference to this matrix which is the rotation of the quaternion
	*
	* \param rotation Quaternion representing a rotation of space
	*
	* \remark 3rd column and row are unchanged. Scale is removed.
	*/

	template<typename T>
	Matrix4<T>& Matrix4<T>::SetRotation(const Quaternion<T>& rotation)
	{
		T qw = rotation.w;
		T qx = rotation.x;
		T qy = rotation.y;
		T qz = rotation.z;

		T qx2 = qx * qx;
		T qy2 = qy * qy;
		T qz2 = qz * qz;

		m11 = T(1.0) - T(2.0) * qy2 - T(2.0) * qz2;
		m21 = T(2.0) * qx * qy - T(2.0) * qz * qw;
		m31 = T(2.0) * qx * qz + T(2.0) * qy * qw;

		m12 = T(2.0) * qx * qy + T(2.0) * qz * qw;
		m22 = T(1.0) - T(2.0) * qx2 - T(2.0) * qz2;
		m32 = T(2.0) * qy * qz - T(2.0) * qx * qw;

		m13 = T(2.0) * qx * qz - T(2.0) * qy * qw;
		m23 = T(2.0) * qy * qz + T(2.0) * qx * qw;
		m33 = T(1.0) - T(2.0) * qx2 - T(2.0) * qy2;

		return *this;
	}

	/*!
	* \brief Sets the components of the matrix from a scale
	* \return A reference to this matrix which is the scale of the Vector3
	*
	* \param scale Vector3 representing the homothety
	*
	* \remark Components are unchanged, except the three first on the diagonal
	*/

	template<typename T>
	Matrix4<T>& Matrix4<T>::SetScale(const Vector3<T>& scale)
	{
		m11 = scale.x;
		m22 = scale.y;
		m33 = scale.z;

		return *this;
	}

	/*!
	* \brief Sets the components of the matrix from a translation
	* \return A reference to this matrix which is the translation of the Vector3
	*
	* \param translation Vector3 representing the translation
	*
	* \remark Components are unchanged, except the three first on the third row
	*/

	template<typename T>
	Matrix4<T>& Matrix4<T>::SetTranslation(const Vector3<T>& translation)
	{
		m41 = translation.x;
		m42 = translation.y;
		m43 = translation.z;

		return *this;
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object: "Matrix4(m11, m12, m13, m14,\n ...)"
	*/

	template<typename T>
	std::string Matrix4<T>::ToString() const
	{
		std::ostringstream ss;
		ss << *this;

		return ss.str();
	}

	/*!
	* \brief Transforms the Vector2 and two components by the matrix
	* \return Vector2 transformed by the matrix
	*
	* \param vector To transform
	* \param z Z Component of the imaginary Vector4
	* \param w W Component of the imaginary Vector4
	*/

	template<typename T>
	Vector2<T> Matrix4<T>::Transform(const Vector2<T>& vector, T z, T w) const
	{
		return Vector2<T>(m11 * vector.x + m21 * vector.y + m31 * z + m41 * w,
		                  m12 * vector.x + m22 * vector.y + m32 * z + m42 * w);
	}

	/*!
	* \brief Transforms the Vector3 and one component by the matrix
	* \return Vector3 transformed by the matrix
	*
	* \param vector To transform
	* \param w W Component of the imaginary Vector4
	*/

	template<typename T>
	Vector3<T> Matrix4<T>::Transform(const Vector3<T>& vector, T w) const
	{
		return Vector3<T>(m11 * vector.x + m21 * vector.y + m31 * vector.z + m41 * w,
		                  m12 * vector.x + m22 * vector.y + m32 * vector.z + m42 * w,
		                  m13 * vector.x + m23 * vector.y + m33 * vector.z + m43 * w);
	}

	/*!
	* \brief Transforms the Vector4 by the matrix
	* \return Vector4 transformed by the matrix
	*
	* \param vector To transform
	*/

	template<typename T>
	Vector4<T> Matrix4<T>::Transform(const Vector4<T>& vector) const
	{
		return Vector4<T>(m11 * vector.x + m21 * vector.y + m31 * vector.z + m41 * vector.w,
		                  m12 * vector.x + m22 * vector.y + m32 * vector.z + m42 * vector.w,
		                  m13 * vector.x + m23 * vector.y + m33 * vector.z + m43 * vector.w,
		                  m14 * vector.x + m24 * vector.y + m34 * vector.z + m44 * vector.w);
	}

	/*!
	* \brief Transposes the matrix
	* \return A reference to this matrix transposed
	*
	* \see GetTransposed
	*/

	template<typename T>
	Matrix4<T>& Matrix4<T>::Transpose()
	{
		std::swap(m12, m21);
		std::swap(m13, m31);
		std::swap(m14, m41);
		std::swap(m23, m32);
		std::swap(m24, m42);
		std::swap(m34, m43);

		return *this;
	}

	/*!
	* \brief Gets the component (x, y) of the matrix
	* \return A reference to the component (x, y)
	*
	* \remark x and y must both be comprised in range [0,4[
	*/
	template<typename T>
	T& Matrix4<T>::operator()(std::size_t x, std::size_t y)
	{
		NazaraAssert(x <= 3, "index out of range");
		NazaraAssert(y <= 3, "index out of range");

		return (&m11)[y*4 + x];
	}

	/*!
	* \brief Gets the component (x, y) of the matrix
	* \return The value of the component (x, y)
	*
	* \remark x and y must both be comprised in range [0,4[
	*/
	template<typename T>
	T Matrix4<T>::operator()(std::size_t x, std::size_t y) const
	{
		NazaraAssert(x <= 3, "index out of range");
		NazaraAssert(y <= 3, "index out of range");

		return (&m11)[y*4+x];
	}

	/*!
	* \brief Gets the i-th component of the matrix
	* \return The value of the component (i)
	*
	* \remark i must be comprised in range [0,16[
	*/
	template<typename T>
	T& Matrix4<T>::operator[](std::size_t i)
	{
		NazaraAssert(i <= 16, "index out of range");

		return (&m11)[i];
	}

	/*!
	* \brief Gets the i-th component of the matrix
	* \return The value of the component (i)
	*
	* \remark i must be comprised in range [0,16[
	*/
	template<typename T>
	T Matrix4<T>::operator[](std::size_t i) const
	{
		NazaraAssert(i <= 16, "index out of range");

		return (&m11)[i];
	}

	/*!
	* \brief Multiplies the components of the matrix with other matrix
	* \return A matrix where components are the product of this matrix and the other one according to matrix product
	*
	* \param matrix The other matrix to multiply components with
	*/

	template<typename T>
	Matrix4<T> Matrix4<T>::operator*(const Matrix4& matrix) const
	{
		Matrix4 result(*this);
		return result.Concatenate(matrix);
	}

	/*!
	* \brief Multiplies the components of the matrix with a vector
	* \return A vector transposed by this matrix
	*
	* \param vector The vector to multiply the matrix with
	*/

	template<typename T>
	Vector2<T> Matrix4<T>::operator*(const Vector2<T>& vector) const
	{
		return Transform(vector);
	}

	/*!
	* \brief Multiplies the components of the matrix with a vector
	* \return A vector transposed by this matrix
	*
	* \param vector The vector to multiply the matrix with
	*/

	template<typename T>
	Vector3<T> Matrix4<T>::operator*(const Vector3<T>& vector) const
	{
		return Transform(vector);
	}

	/*!
	* \brief Multiplies the components of the matrix with a vector
	* \return A vector transposed by this matrix
	*
	* \param vector The vector to multiply the matrix with
	*/

	template<typename T>
	Vector4<T> Matrix4<T>::operator*(const Vector4<T>& vector) const
	{
		return Transform(vector);
	}

	/*!
	* \brief Multiplies the components of the matrix with a scalar
	* \return A Matrix4 where components are the product of matrix'components and the scalar
	*
	* \param scalar The scalar to multiply the matrix'components with
	*/

	template<typename T>
	Matrix4<T> Matrix4<T>::operator*(T scalar) const
	{
		Matrix4 mat;
		for (unsigned int i = 0; i < 16; ++i)
			mat[i] = (&m11)[i] * scalar;

		return mat;
	}

	/*!
	* \brief Multiplies this matrix with another one
	* \return A reference to this matrix which is the product with the other one
	*
	* \param matrix The matrix to multiply with
	*/

	template<typename T>
	Matrix4<T>& Matrix4<T>::operator*=(const Matrix4& matrix)
	{
		Concatenate(matrix);

		return *this;
	}

	/*!
	* \brief Multiplies the components of the matrix with a scalar
	* \return A reference to this matrix where components are the product with the scalar
	*
	* \param scalar The scalar to multiply with
	*/

	template<typename T>
	Matrix4<T>& Matrix4<T>::operator*=(T scalar)
	{
		for (unsigned int i = 0; i < 16; ++i)
			(&m11)[i] *= scalar;

		return *this;
	}

	/*!
	* \brief Compares the matrix to other one
	* \return true if the matrices are the same
	*
	* \param mat Other matrix to compare with
	*/

	template<typename T>
	bool Matrix4<T>::operator==(const Matrix4& mat) const
	{
		for (unsigned int i = 0; i < 16; ++i)
			if (!NumberEquals((&m11)[i], (&mat.m11)[i]))
				return false;

		return true;
	}

	/*!
	* \brief Compares the matrix to other one
	* \return false if the matrices are the same
	*
	* \param mat Other matrix to compare with
	*/

	template<typename T>
	bool Matrix4<T>::operator!=(const Matrix4& mat) const
	{
		return !operator==(mat);
	}

	/*!
	* \brief Shorthand for the concatenation of two matrices
	* \return A Matrix4 which is the product of two
	*
	* \param left Left-hand side matrix
	* \param right Right-hand side matrix
	*
	* \see Concatenate
	*/

	template<typename T>
	Matrix4<T> Matrix4<T>::Concatenate(const Matrix4& left, const Matrix4& right)
	{
		Matrix4 matrix(left); // Copy of left-hand side matrix
		matrix.Concatenate(right); // Concatenation with right-hand side

		return matrix;
	}

	/*!
	* \brief Shorthand for the concatenation of two affine matrices
	* \return A Matrix4 which is the product of two
	*
	* \param left Left-hand side matrix
	* \param right Right-hand side matrix
	*
	* \see ConcatenateAffine
	*/

	template<typename T>
	Matrix4<T> Matrix4<T>::ConcatenateAffine(const Matrix4& left, const Matrix4& right)
	{
		Matrix4 matrix(left); // Copy of left-hand side matrix
		matrix.ConcatenateAffine(right); // Affine concatenation with right-hand side

		return matrix;
	}

	/*!
	* \brief Shorthand for the identity matrix
	* \return A Matrix4 which is the identity matrix
	*
	* \see MakeIdentity
	*/

	template<typename T>
	Matrix4<T> Matrix4<T>::Identity()
	{
		Matrix4 matrix;
		matrix.MakeIdentity();

		return matrix;
	}

	/*!
	* \brief Shorthand for the 'look at' matrix
	* \return A Matrix4 which is the 'look at' matrix
	*
	* \param eye Position of the camera
	* \param target Position of the target of the camera
	* \param up Direction of up vector according to the orientation of camera
	*
	* \see MakeLookAt
	*/

	template<typename T>
	Matrix4<T> Matrix4<T>::LookAt(const Vector3<T>& eye, const Vector3<T>& target, const Vector3<T>& up)
	{
		Matrix4 matrix;
		matrix.MakeLookAt(eye, target, up);

		return matrix;
	}

	/*!
	* \brief Shorthand for the 'orthographic' matrix
	* \return A Matrix4 which is the 'orthographic' matrix
	*
	* \param left Distance between center and left
	* \param right Distance between center and right
	* \param top Distance between center and top
	* \param bottom Distance between center and bottom
	* \param zNear Distance where 'vision' begins
	* \param zFar Distance where 'vision' ends
	*
	* \see MakeOrtho
	*/

	template<typename T>
	Matrix4<T> Matrix4<T>::Ortho(T left, T right, T top, T bottom, T zNear, T zFar)
	{
		Matrix4 matrix;
		matrix.MakeOrtho(left, right, top, bottom, zNear, zFar);

		return matrix;
	}

	/*!
	* \brief Shorthand for the 'perspective' matrix
	* \return A Matrix4 which is the 'perspective' matrix
	*
	* \param angle FOV angle
	* \param ratio Rendering ratio (typically 16/9 or 4/3)
	* \param zNear Distance where 'vision' begins
	* \param zFar Distance where 'vision' ends
	*
	* \see MakePerspective
	*/

	template<typename T>
	Matrix4<T> Matrix4<T>::Perspective(RadianAngle<T> angle, T ratio, T zNear, T zFar)
	{
		Matrix4 matrix;
		matrix.MakePerspective(angle, ratio, zNear, zFar);

		return matrix;
	}

	/*!
	* \brief Shorthand for the 'rotation' matrix
	* \return A Matrix4 which is the rotation of the quaternion
	*
	* \param rotation Quaternion representing a rotation of space
	*
	* \see MakeRotation
	*/

	template<typename T>
	Matrix4<T> Matrix4<T>::Rotate(const Quaternion<T>& rotation)
	{
		Matrix4 matrix;
		matrix.MakeRotation(rotation);

		return matrix;
	}

	/*!
	* \brief Shorthand for the 'scale' matrix
	* \return A Matrix4 which is is the scale
	*
	* \param scale Vector3 representing the homothety
	*
	* \see MakeScale
	*/

	template<typename T>
	Matrix4<T> Matrix4<T>::Scale(const Vector3<T>& scale)
	{
		Matrix4 matrix;
		matrix.MakeScale(scale);

		return matrix;
	}

	/*!
	* \brief Shorthand for the 'translation' matrix
	* \return A Matrix4 which is is the translation
	*
	* \param translation Vector3 representing the translation
	*
	* \see MakeTranslation
	*/

	template<typename T>
	Matrix4<T> Matrix4<T>::Translate(const Vector3<T>& translation)
	{
		Matrix4 mat;
		mat.MakeTranslation(translation);

		return mat;
	}

	/*!
	* \brief Shorthand for the 'transform' matrix
	* \return A Matrix4 which is transformation obtained by the translation and the rotation
	*
	* \param translation Vector3 representing the translation
	* \param rotation Quaternion representing a rotation of space
	*
	* \remark Rotation is applied first
	*
	* \see MakeTransform
	*/

	template<typename T>
	Matrix4<T> Matrix4<T>::Transform(const Vector3<T>& translation, const Quaternion<T>& rotation)
	{
		Matrix4 mat;
		mat.MakeTransform(translation, rotation);

		return mat;
	}

	/*!
	* \brief Shorthand for the 'transform' matrix
	* \return A Matrix4 which is transformation obtained by the translation, the rotation and the scale
	*
	* \param translation Vector3 representing the translation
	* \param rotation Quaternion representing a rotation of space
	* \param scale Vector3 representing the homothety
	*
	* \remark Rotation is applied first, then translation
	*
	* \see MakeTransform
	*/

	template<typename T>
	Matrix4<T> Matrix4<T>::Transform(const Vector3<T>& translation, const Quaternion<T>& rotation, const Vector3<T>& scale)
	{
		Matrix4 mat;
		mat.MakeTransform(translation, rotation, scale);

		return mat;
	}

	/*!
	* \brief Shorthand for the 'view' matrix
	* \return A Matrix4 which is the 'view matrix'
	*
	* \param translation Vector3 representing the translation
	* \param rotation Quaternion representing a rotation of space
	*
	* \see MakeViewMatrix
	*/

	template<typename T>
	Matrix4<T> Matrix4<T>::ViewMatrix(const Vector3<T>& translation, const Quaternion<T>& rotation)
	{
		Matrix4 mat;
		mat.MakeViewMatrix(translation, rotation);

		return mat;
	}

	/*!
	* \brief Shorthand for the 'zero' matrix
	* \return A Matrix4 with components (0 everywhere)
	*
	* \see MakeZero
	*/

	template<typename T>
	Matrix4<T> Matrix4<T>::Zero()
	{
		Matrix4 matrix;
		matrix.MakeZero();

		return matrix;
	}

	/*!
	* \brief Serializes a Matrix4
	* \return true if successfully serialized
	*
	* \param context Serialization context
	* \param matrix Input matrix
	*/
	template<typename T>
	bool Serialize(SerializationContext& context, const Matrix4<T>& matrix, TypeTag<Matrix4<T>>)
	{
		for (unsigned int i = 0; i < 16; ++i)
		{
			if (!Serialize(context, matrix[i]))
				return false;
		}

		return true;
	}

	/*!
	* \brief Unserializes a Matrix4
	* \return true if successfully unserialized
	*
	* \param context Serialization context
	* \param matrix Output matrix
	*/
	template<typename T>
	bool Unserialize(SerializationContext& context, Matrix4<T>* matrix, TypeTag<Matrix4<T>>)
	{
		T* head = &matrix->m11;
		for (unsigned int i = 0; i < 16; ++i)
		{
			if (!Unserialize(context, head + i))
				return false;
		}

		return true;
	}
}

/*!
* \brief Output operator
* \return The stream
*
* \param out The stream
* \param matrix The matrix to output
*/

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nz::Matrix4<T>& matrix)
{
	return out << "Matrix4(" << matrix.m11 << ", " << matrix.m12 << ", " << matrix.m13 << ", " << matrix.m14 << ",\n"
	           << "        " << matrix.m21 << ", " << matrix.m22 << ", " << matrix.m23 << ", " << matrix.m24 << ",\n"
	           << "        " << matrix.m31 << ", " << matrix.m32 << ", " << matrix.m33 << ", " << matrix.m34 << ",\n"
	           << "        " << matrix.m41 << ", " << matrix.m42 << ", " << matrix.m43 << ", " << matrix.m44 << ')';
}

/*!
* \brief Multiplies the components of the matrix with a scalar
* \return A Matrix4 where components are the product of matrix'components and the scalar
*
* \param scale The scalar to multiply the matrix'components with
* \param matrix Matrix to multiply with
*/

template<typename T>
Nz::Matrix4<T> operator*(T scale, const Nz::Matrix4<T>& matrix)
{
	return matrix * scale;
}

#include <Nazara/Core/DebugOff.hpp>
#include "Matrix4.hpp"
