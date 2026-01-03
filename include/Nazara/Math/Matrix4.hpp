// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_MATH_MATRIX4_HPP
#define NAZARA_MATH_MATRIX4_HPP

///FIXME: Matrices column-major, difficile de bosser avec (Tout passer en row-major et transposer dans les shaders ?)

#include <Nazara/Math/Angle.hpp>
#include <NazaraUtils/TypeTag.hpp>
#include <cstddef>
#include <string>

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
			constexpr Matrix4() = default;
			constexpr Matrix4(T r11, T r12, T r13, T r14,
			                  T r21, T r22, T r23, T r24,
			                  T r31, T r32, T r33, T r34,
			                  T r41, T r42, T r43, T r44);
			constexpr Matrix4(const T matrix[16]);
			template<typename U> constexpr explicit Matrix4(const Matrix4<U>& matrix);

			constexpr Matrix4& ApplyRotation(const Quaternion<T>& rotation);
			constexpr Matrix4& ApplyScale(const Vector3<T>& scale);
			constexpr Matrix4& ApplyTranslation(const Vector3<T>& translation);

			constexpr bool ApproxEqual(const Matrix4& vec, T maxDifference = std::numeric_limits<T>::epsilon()) const;

			constexpr Matrix4& Concatenate(const Matrix4& matrix);
			constexpr Matrix4& ConcatenateTransform(const Matrix4& matrix);

			constexpr Vector4<T> GetColumn(std::size_t column) const;
			constexpr T GetDeterminant() const;
			constexpr T GetDeterminantTransform() const;
			constexpr bool GetInverse(Matrix4* dest) const;
			constexpr bool GetInverseTransform(Matrix4* dest) const;
			Quaternion<T> GetRotation() const;
			constexpr Vector4<T> GetRow(std::size_t row) const;
			constexpr Vector3<T> GetScale() const;
			constexpr Vector3<T> GetSquaredScale() const;
			constexpr Vector3<T> GetTranslation() const;
			constexpr void GetTransposed(Matrix4* dest) const;

			constexpr bool HasNegativeScale() const;
			constexpr bool HasScale() const;

			constexpr Matrix4& Inverse(bool* succeeded = nullptr);
			constexpr Matrix4& InverseTransform(bool* succeeded = nullptr);

			constexpr bool IsTransformMatrix() const;
			constexpr bool IsIdentity() const;

			constexpr Matrix4& SetRotation(const Quaternion<T>& rotation);
			constexpr Matrix4& SetScale(const Vector3<T>& scale);
			constexpr Matrix4& SetTranslation(const Vector3<T>& translation);

			std::string ToString() const;

			constexpr Vector2<T> Transform(const Vector2<T>& vector, T z = 0.0, T w = 1.0) const;
			constexpr Vector3<T> Transform(const Vector3<T>& vector, T w = 1.0) const;
			constexpr Vector4<T> Transform(const Vector4<T>& vector) const;

			constexpr Matrix4& Transpose();

			constexpr T& operator()(std::size_t x, std::size_t y);
			constexpr const T& operator()(std::size_t x, std::size_t y) const;

			constexpr T& operator[](std::size_t i);
			constexpr const T& operator[](std::size_t i) const;

			constexpr Matrix4 operator*(const Matrix4& matrix) const;
			constexpr Vector2<T> operator*(const Vector2<T>& vector) const;
			constexpr Vector3<T> operator*(const Vector3<T>& vector) const;
			constexpr Vector4<T> operator*(const Vector4<T>& vector) const;
			constexpr Matrix4 operator*(T scalar) const;

			constexpr Matrix4& operator*=(const Matrix4& matrix);
			constexpr Matrix4& operator*=(T scalar);

			constexpr bool operator==(const Matrix4& mat) const;
			constexpr bool operator!=(const Matrix4& mat) const;

			static constexpr bool ApproxEqual(const Matrix4& lhs, const Matrix4& rhs, T maxDifference = std::numeric_limits<T>::epsilon());
			static constexpr Matrix4 Concatenate(const Matrix4& left, const Matrix4& right);
			static constexpr Matrix4 ConcatenateTransform(const Matrix4& left, const Matrix4& right);
			static constexpr Matrix4 Identity();
			static constexpr Matrix4 LookAt(const Vector3<T>& eye, const Vector3<T>& target, const Vector3<T>& up = Vector3<T>::Up());
			static constexpr Matrix4 Ortho(T left, T right, T top, T bottom, T zNear = -1.0, T zFar = 1.0);
			static Matrix4 Perspective(RadianAngle<T> angle, T ratio, T zNear, T zFar);
			static constexpr Matrix4 Rotate(const Quaternion<T>& rotation);
			static constexpr Matrix4 Scale(const Vector3<T>& scale);
			static constexpr Matrix4 Translate(const Vector3<T>& translation);
			static constexpr Matrix4 Transform(const Vector3<T>& translation, const Quaternion<T>& rotation);
			static constexpr Matrix4 Transform(const Vector3<T>& translation, const Quaternion<T>& rotation, const Vector3<T>& scale);
			static constexpr Matrix4 TransformInverse(const Vector3<T>& translation, const Quaternion<T>& rotation);
			static constexpr Matrix4 TransformInverse(const Vector3<T>& translation, const Quaternion<T>& rotation, const Vector3<T>& scale);
			static constexpr Matrix4 Zero();

			T m11, m12, m13, m14,
			  m21, m22, m23, m24,
			  m31, m32, m33, m34,
			  m41, m42, m43, m44;
	};

	using Matrix4d = Matrix4<double>;
	using Matrix4f = Matrix4<float>;

	template<typename T> bool Serialize(SerializationContext& context, const Matrix4<T>& matrix, TypeTag<Matrix4<T>>);
	template<typename T> bool Deserialize(SerializationContext& context, Matrix4<T>* matrix, TypeTag<Matrix4<T>>);

	template<typename T> std::ostream& operator<<(std::ostream& out, const Matrix4<T>& matrix);

	template<typename T> constexpr Matrix4<T> operator*(T scale, const Matrix4<T>& matrix);
}

#include <Nazara/Math/Matrix4.inl>

#endif // NAZARA_MATH_MATRIX4_HPP
