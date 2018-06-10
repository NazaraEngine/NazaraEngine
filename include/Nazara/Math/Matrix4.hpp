// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MATRIX4_HPP
#define NAZARA_MATRIX4_HPP

///FIXME: Matrices column-major, difficile de bosser avec (Tout passer en row-major et transposer dans les shaders ?)

#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Config.hpp>

namespace Nz
{
	struct SerializationContext;

	template<typename T> class EulerAngles;
	template<typename T> class Quaternion;
	template<typename T> class Vector2;
	template<typename T> class Vector3;
	template<typename T> class Vector4;

	template<typename T>
	class Matrix4
	{
		public:
			Matrix4() = default;
			Matrix4(T r11, T r12, T r13, T r14,
			        T r21, T r22, T r23, T r24,
			        T r31, T r32, T r33, T r34,
			        T r41, T r42, T r43, T r44);
			//Matrix4(const Matrix3<T>& matrix);
			Matrix4(const T matrix[16]);
			template<typename U> explicit Matrix4(const Matrix4<U>& matrix);
			Matrix4(const Matrix4& matrix) = default;
			~Matrix4() = default;

			Matrix4& ApplyRotation(const Quaternion<T>& rotation);
			Matrix4& ApplyScale(const Vector3<T>& scale);
			Matrix4& ApplyTranslation(const Vector3<T>& translation);

			Matrix4& Concatenate(const Matrix4& matrix);
			Matrix4& ConcatenateAffine(const Matrix4& matrix);

			Vector4<T> GetColumn(unsigned int column) const;
			T GetDeterminant() const;
			T GetDeterminantAffine() const;
			bool GetInverse(Matrix4* dest) const;
			bool GetInverseAffine(Matrix4* dest) const;
			Quaternion<T> GetRotation() const;
			//Matrix3 GetRotationMatrix() const;
			Vector4<T> GetRow(unsigned int row) const;
			Vector3<T> GetScale() const;
			Vector3<T> GetSquaredScale() const;
			Vector3<T> GetTranslation() const;
			void GetTransposed(Matrix4* dest) const;

			bool HasNegativeScale() const;
			bool HasScale() const;

			Matrix4& Inverse(bool* succeeded = nullptr);
			Matrix4& InverseAffine(bool* succeeded = nullptr);

			bool IsAffine() const;
			bool IsIdentity() const;

			Matrix4& MakeIdentity();
			Matrix4& MakeLookAt(const Vector3<T>& eye, const Vector3<T>& target, const Vector3<T>& up = Vector3<T>::Up());
			Matrix4& MakeOrtho(T left, T right, T top, T bottom, T zNear = -1.0, T zFar = 1.0);
			Matrix4& MakePerspective(T angle, T ratio, T zNear, T zFar);
			Matrix4& MakeRotation(const Quaternion<T>& rotation);
			Matrix4& MakeScale(const Vector3<T>& scale);
			Matrix4& MakeTranslation(const Vector3<T>& translation);
			Matrix4& MakeTransform(const Vector3<T>& translation, const Quaternion<T>& rotation);
			Matrix4& MakeTransform(const Vector3<T>& translation, const Quaternion<T>& rotation, const Vector3<T>& scale);
			Matrix4& MakeViewMatrix(const Vector3<T>& translation, const Quaternion<T>& rotation);
			Matrix4& MakeZero();

			Matrix4& Set(T r11, T r12, T r13, T r14,
			             T r21, T r22, T r23, T r24,
			             T r31, T r32, T r33, T r34,
			             T r41, T r42, T r43, T r44);
			Matrix4& Set(const T matrix[16]);
			//Matrix4(const Matrix3<T>& matrix);
			Matrix4& Set(const Matrix4& matrix);
			template<typename U> Matrix4& Set(const Matrix4<U>& matrix);
			Matrix4& SetRotation(const Quaternion<T>& rotation);
			Matrix4& SetScale(const Vector3<T>& scale);
			Matrix4& SetTranslation(const Vector3<T>& translation);

			String ToString() const;

			Vector2<T> Transform(const Vector2<T>& vector, T z = 0.0, T w = 1.0) const;
			Vector3<T> Transform(const Vector3<T>& vector, T w = 1.0) const;
			Vector4<T> Transform(const Vector4<T>& vector) const;

			Matrix4& Transpose();

			operator T* ();
			operator const T* () const;

			T& operator()(unsigned int x, unsigned int y);
			T operator()(unsigned int x, unsigned int y) const;

			Matrix4& operator=(const Matrix4& matrix) = default;

			Matrix4 operator*(const Matrix4& matrix) const;
			Vector2<T> operator*(const Vector2<T>& vector) const;
			Vector3<T> operator*(const Vector3<T>& vector) const;
			Vector4<T> operator*(const Vector4<T>& vector) const;
			Matrix4 operator*(T scalar) const;

			Matrix4& operator*=(const Matrix4& matrix);
			Matrix4& operator*=(T scalar);

			bool operator==(const Matrix4& mat) const;
			bool operator!=(const Matrix4& mat) const;

			static Matrix4 Concatenate(const Matrix4& left, const Matrix4& right);
			static Matrix4 ConcatenateAffine(const Matrix4& left, const Matrix4& right);
			static Matrix4 Identity();
			static Matrix4 LookAt(const Vector3<T>& eye, const Vector3<T>& target, const Vector3<T>& up = Vector3<T>::Up());
			static Matrix4 Ortho(T left, T right, T top, T bottom, T zNear = -1.0, T zFar = 1.0);
			static Matrix4 Perspective(T angle, T ratio, T zNear, T zFar);
			static Matrix4 Rotate(const Quaternion<T>& rotation);
			static Matrix4 Scale(const Vector3<T>& scale);
			static Matrix4 Translate(const Vector3<T>& translation);
			static Matrix4 Transform(const Vector3<T>& translation, const Quaternion<T>& rotation);
			static Matrix4 Transform(const Vector3<T>& translation, const Quaternion<T>& rotation, const Vector3<T>& scale);
			static Matrix4 ViewMatrix(const Vector3<T>& translation, const Quaternion<T>& rotation);
			static Matrix4 Zero();

			T m11, m12, m13, m14,
			 m21, m22, m23, m24,
			 m31, m32, m33, m34,
			 m41, m42, m43, m44;
	};

	using Matrix4d = Matrix4<double>;
	using Matrix4f = Matrix4<float>;

	template<typename T> bool Serialize(SerializationContext& context, const Matrix4<T>& matrix, TypeTag<Matrix4<T>>);
	template<typename T> bool Unserialize(SerializationContext& context, Matrix4<T>* matrix, TypeTag<Matrix4<T>>);
}

template<typename T> std::ostream& operator<<(std::ostream& out, const Nz::Matrix4<T>& matrix);

template<typename T> Nz::Matrix4<T> operator*(T scale, const Nz::Matrix4<T>& matrix);


#include <Nazara/Math/Matrix4.inl>

#endif // NAZARA_MATRIX4_HPP
