// Copyright (C) 2015 Rémi Bèges - Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)

namespace Nz
{
	/*!
	* \ingroup math
	* \class Nz::Vector3
	* \brief Math class that represents an element of the three dimensional vector space
	*
	* \remark The basis is said to be "right-hand". It means that with your right hand, the thumb is X positive, the index finger Y positive and the middle finger (pointing to you) Z positive
	*/

	/*!
	* \brief Constructs a Vector3 object from its coordinates
	*
	* \param X X component
	* \param Y Y component
	* \param Z Z component
	*/
	template<typename T>
	Vector3<T>::Vector3(T X, T Y, T Z)
	{
		Set(X, Y, Z);
	}

	/*!
	* \brief Constructs a Vector3 object from a component and a Vector2<T>
	*
	* \param X X component
	* \param vec vec.X = Y component and vec.y = Z component
	*/
	template<typename T>
	Vector3<T>::Vector3(T X, const Vector2<T>& vec)
	{
		Set(X, vec);
	}

	/*!
	* \brief Constructs explicitely a Vector3 object from its "scale"
	*
	* \param scale X component = Y component = Z component
	*/
	template<typename T>
	Vector3<T>::Vector3(T scale)
	{
		Set(scale);
	}

	/*!
	* \brief Constructs a Vector3 object from an array of three elements
	*
	* \param vec[3] vec[0] is X component, vec[1] is Y component and vec[2] is Z component
	*/
	template<typename T>
	Vector3<T>::Vector3(const T vec[3])
	{
		Set(vec);
	}

	/*!
	* \brief Constructs a Vector3 object from a Vector2<T> and a component
	*
	* \param vec vec.X = X component and vec.y = Y component
	* \param Z Z component
	*/
	template<typename T>
	Vector3<T>::Vector3(const Vector2<T>& vec, T Z)
	{
		Set(vec, Z);
	}

	/*!
	* \brief Constructs a Vector3 object from another type of Vector3
	*
	* \param vec Vector of type U to convert to type T
	*/
	template<typename T>
	template<typename U>
	Vector3<T>::Vector3(const Vector3<U>& vec)
	{
		Set(vec);
	}

	/*!
	* \brief Constructs a Vector3 object from a Vector4
	*
	* \param vec Vector4 where only the first three components are taken
	*/
	template<typename T>
	Vector3<T>::Vector3(const Vector4<T>& vec)
	{
		Set(vec);
	}

	/*!
	* \brief Calculates the absolute dot (scalar) product with two vectors
	* \return The dot product with absolutes values on each component
	*
	* \param vec The other vector to calculate the absolute dot product with
	*
	* \see DotProduct
	*/
	template<typename T>
	T Vector3<T>::AbsDotProduct(const Vector3& vec) const
	{
		return std::abs(x * vec.x) + std::abs(y * vec.y) + std::abs(z * vec.z);
	}

	/*!
	* \brief Calculates the angle between two vectors in orthonormal basis
	* \return The angle unit depends of NAZARA_MATH_ANGLE_RADIAN in the range 0..pi
	*
	* \param vec The other vector to measure the angle with
	*
	* \remark The vectors do not need to be normalised
	* \remark Produce a NazaraError if one of the vec components is null with NAZARA_MATH_SAFE defined
	* \throw std::domain_error if NAZARA_MATH_SAFE is defined and one of the vec components is null
	*
	* \see NormalizeAngle
	*/
	template<typename T>
	T Vector3<T>::AngleBetween(const Vector3& vec) const
	{
		// sqrt(a) * sqrt(b) = sqrt(a*b)
		T divisor = std::sqrt(GetSquaredLength() * vec.GetSquaredLength());

		#if NAZARA_MATH_SAFE
		if (NumberEquals(divisor, F(0.0)))
		{
			String error("Division by zero");

			NazaraError(error);
			throw std::domain_error(error);
		}
		#endif

		T alpha = DotProduct(vec) / divisor;
		return FromRadians(std::acos(Clamp(alpha, F(-1.0), F(1.0))));
	}

	/*!
	* \brief Calculates the cross (scalar) product with two vectors
	* \return The vector of the cross product according to "right-hand" rule
	*
	* \param vec The other vector to calculate the cross product with
	*
	* \see CrossProduct
	*/
	template<typename T>
	Vector3<T> Vector3<T>::CrossProduct(const Vector3& vec) const
	{
		return Vector3(y * vec.z - z * vec.y, z * vec.x - x * vec.z, x * vec.y - y * vec.x);
	}

	/*!
	* \brief Calculates the distance between two vectors
	* \return The metric distance between two vectors with euclidean norm
	*
	* \param vec The other vector to measure the distance with
	*
	* \see SquaredDistance
	*/
	template<typename T>
	T Vector3<T>::Distance(const Vector3& vec) const
	{
		return std::sqrt(SquaredDistance(vec));
	}

	/*!
	* \brief Calculates the distance between two vectors
	* \return The metric distance in float between two vectors with euclidean norm
	*
	* \param vec The other vector to measure the distance with
	*/
	template<typename T>
	float Vector3<T>::Distancef(const Vector3& vec) const
	{
		return std::sqrt(static_cast<float>(SquaredDistance(vec)));
	}

	/*!
	* \brief Calculates the dot (scalar) product with two vectors
	* \return The value of the dot product
	*
	* \param vec The other vector to calculate the dot product with
	*
	* \see AbsDotProduct, DotProduct
	*/
	template<typename T>
	T Vector3<T>::DotProduct(const Vector3& vec) const
	{
		return x * vec.x + y * vec.y + z * vec.z;
	}

	/*!
	* \brief Calculates the length (magnitude) of the vector
	* \return The length of the vector
	*
	* \see GetSquaredLength
	*/
	template<typename T>
	T Vector3<T>::GetLength() const
	{
		return static_cast<T>(std::sqrt(GetSquaredLength()));
	}

	/*!
	* \brief Calculates the length (magnitude) of the vector
	* \return The length in float of the vector
	*/
	template<typename T>
	float Vector3<T>::GetLengthf() const
	{
		return std::sqrt(static_cast<float>(GetSquaredLength()));
	}

	/*!
	* \brief Gets a copy normalized of the vector
	* \return A new vector which is the vector normalized
	*
	* \param length Optional argument to obtain the length's ratio of the vector and the unit-length
	*
	* \remark If ths vector is (0, 0, 0), then it returns (0, 0, 0) and length is 0
	*
	* \see Normalize
	*/
	template<typename T>
	Vector3<T> Vector3<T>::GetNormal(T* length) const
	{
		Vector3 vec(*this);
		vec.Normalize(length);

		return vec;
	}

	/*!
	* \brief Calculates the squared length (magnitude) of the vector
	* \return The squared length of the vector
	*
	* \see GetLength
	*/
	template<typename T>
	T Vector3<T>::GetSquaredLength() const
	{
		return x*x + y*y + z*z;
	}

	/*!
	* \brief Makes the vector (0, 0, 1)
	* \return A reference to this vector with components (0, 0, 1)
	*
	* \see Backward
	*/
	template<typename T>
	Vector3<T>& Vector3<T>::MakeBackward()
	{
		return Set(F(0.0), F(0.0), F(1.0));
	}

	/*!
	* \brief Makes the vector (0, -1, 0)
	* \return A reference to this vector with components (0, -1, 0)
	*
	* \see Down
	*/
	template<typename T>
	Vector3<T>& Vector3<T>::MakeDown()
	{
		return Set(F(0.0), F(-1.0), F(0.0));
	}

	/*!
	* \brief Makes the vector (0, 0, -1)
	* \return A reference to this vector with components (0, 0, -1)
	*
	* \see Forward
	*/
	template<typename T>
	Vector3<T>& Vector3<T>::MakeForward()
	{
		return Set(F(0.0), F(0.0), F(-1.0));
	}

	/*!
	* \brief Makes the vector (-1, 0, 0)
	* \return A reference to this vector with components (-1, 0, 0)
	*
	* \see Left
	*/
	template<typename T>
	Vector3<T>& Vector3<T>::MakeLeft()
	{
		return Set(F(-1.0), F(0.0), F(0.0));
	}

	/*!
	* \brief Makes the vector (1, 0, 0)
	* \return A reference to this vector with components (1, 0, 0)
	*
	* \see Right
	*/
	template<typename T>
	Vector3<T>& Vector3<T>::MakeRight()
	{
		return Set(F(1.0), F(0.0), F(0.0));
	}

	/*!
	* \brief Makes the vector (1, 1, 1)
	* \return A reference to this vector with components (1, 1, 1)
	*
	* \see Unit
	*/
	template<typename T>
	Vector3<T>& Vector3<T>::MakeUnit()
	{
		return Set(F(1.0), F(1.0), F(1.0));
	}

	/*!
	* \brief Makes the vector (1, 0, 0)
	* \return A reference to this vector with components (1, 0, 0)
	*
	* \see UnitX
	*/
	template<typename T>
	Vector3<T>& Vector3<T>::MakeUnitX()
	{
		return Set(F(1.0), F(0.0), F(0.0));
	}

	/*!
	* \brief Makes the vector (0, 1, 0)
	* \return A reference to this vector with components (0, 1, 0)
	*
	* \see UnitY
	*/
	template<typename T>
	Vector3<T>& Vector3<T>::MakeUnitY()
	{
		return Set(F(0.0), F(1.0), F(0.0));
	}

	/*!
	* \brief Makes the vector (0, 0, 1)
	* \return A reference to this vector with components (0, 0, 1)
	*
	* \see UnitZ
	*/
	template<typename T>
	Vector3<T>& Vector3<T>::MakeUnitZ()
	{
		return Set(F(0.0), F(0.0), F(1.0));
	}

	/*!
	* \brief Makes the vector (0, 1, 0)
	* \return A reference to this vector with components (0, 1, 0)
	*
	* \see Up
	*/
	template<typename T>
	Vector3<T>& Vector3<T>::MakeUp()
	{
		return Set(F(0.0), F(1.0), F(0.0));
	}

	/*!
	* \brief Makes the vector (0, 0, 0)
	* \return A reference to this vector with components (0, 0, 0)
	*
	* \see Zero
	*/
	template<typename T>
	Vector3<T>& Vector3<T>::MakeZero()
	{
		return Set(F(0.0), F(0.0), F(0.0));
	}

	/*!
	* \brief Sets this vector's components to the maximum of its own and other components
	* \return A reference to this vector with replaced values with the corresponding max value
	*
	* \param vec Other vector to compare the components with
	*
	* \see Minimize
	*/
	template<typename T>
	Vector3<T>& Vector3<T>::Maximize(const Vector3& vec)
	{
		if (vec.x > x)
			x = vec.x;

		if (vec.y > y)
			y = vec.y;

		if (vec.z > z)
			z = vec.z;

		return *this;
	}

	/*!
	* \brief Sets this vector's components to the minimum of its own and other components
	* \return A reference to this vector with replaced values with the corresponding min value
	*
	* \param vec Other vector to compare the components with
	*
	* \see Maximize
	*/
	template<typename T>
	Vector3<T>& Vector3<T>::Minimize(const Vector3& vec)
	{
		if (vec.x < x)
			x = vec.x;

		if (vec.y < y)
			y = vec.y;

		if (vec.z < z)
			z = vec.z;

		return *this;
	}

	/*!
	* \brief Normalizes the current vector
	* \return A reference to this vector
	*
	* \param length Optional argument to obtain the length's ratio of the vector and the unit-length
	*
	* \remark If the vector is (0, 0, 0), then it returns (0, 0, 0) and length is 0
	*
	* \see GetNormal
	*/
	template<typename T>
	Vector3<T>& Vector3<T>::Normalize(T* length)
	{
		T norm = GetLength();
		if (norm > F(0.0))
		{
			T invNorm = F(1.0) / norm;
			x *= invNorm;
			y *= invNorm;
			z *= invNorm;
		}

		if (length)
			*length = norm;

		return *this;
	}

	/*!
	* \brief Sets the components of the vector
	* \return A reference to this vector
	*
	* \param X X component
	* \param Y Y component
	* \param Z Z component
	*/
	template<typename T>
	Vector3<T>& Vector3<T>::Set(T X, T Y, T Z)
	{
		x = X;
		y = Y;
		z = Z;

		return *this;
	}

	/*!
	* \brief Sets the components of the vector from a component and a Vector2
	*
	* \param X X component
	* \param vec vec.X = Y component and vec.y = Z component
	*/
	template<typename T>
	Vector3<T>& Vector3<T>::Set(T X, const Vector2<T>& vec)
	{
		x = X;
		y = vec.x;
		z = vec.y;

		return *this;
	}

	/*!
	* \brief Sets the components of the vector from a "scale"
	* \return A reference to this vector
	*
	* \param scale X component = Y component = Z component
	*/
	template<typename T>
	Vector3<T>& Vector3<T>::Set(T scale)
	{
		x = scale;
		y = scale;
		z = scale;

		return *this;
	}

	/*!
	* \brief Sets the components of the vector from an array of three elements
	* \return A reference to this vector
	*
	* \param vec[3] vec[0] is X component, vec[1] is Y component and vec[2] is Z component
	*/
	template<typename T>
	Vector3<T>& Vector3<T>::Set(const T vec[3])
	{
		std::memcpy(&x, vec, 3*sizeof(T));

		return *this;
	}

	/*!
	* \brief Sets the components of the vector from a Vector2 and a component
	*
	* \param vec vec.X = X component and vec.y = Y component
	* \param Z Z component
	*/
	template<typename T>
	Vector3<T>& Vector3<T>::Set(const Vector2<T>& vec, T Z)
	{
		x = vec.x;
		y = vec.y;
		z = Z;

		return *this;
	}

	/*!
	* \brief Sets the components of the vector from another vector
	* \return A reference to this vector
	*
	* \param vec The other vector
	*/
	template<typename T>
	Vector3<T>& Vector3<T>::Set(const Vector3& vec)
	{
		std::memcpy(this, &vec, sizeof(Vector3));

		return *this;
	}

	/*!
	* \brief Sets the components of the vector from another type of Vector3
	* \return A reference to this vector
	*
	* \param vec Vector of type U to convert its components
	*/
	template<typename T>
	template<typename U>
	Vector3<T>& Vector3<T>::Set(const Vector3<U>& vec)
	{
		x = F(vec.x);
		y = F(vec.y);
		z = F(vec.z);

		return *this;
	}

	/*!
	* \brief Sets the components of the vector from a Vector4
	* \return A reference to this vector
	*
	* \param vec Vector4 where only the first three components are taken
	*/
	template<typename T>
	Vector3<T>& Vector3<T>::Set(const Vector4<T>& vec)
	{
		x = vec.x;
		y = vec.y;
		z = vec.z;

		return *this;
	}

	/*!
	* \brief Calculates the squared distance between two vectors
	* \return The metric distance between two vectors with the squared euclidean norm
	*
	* \param vec The other vector to measure the distance with
	*
	* \see Distance
	*/
	template<typename T>
	T Vector3<T>::SquaredDistance(const Vector3& vec) const
	{
		return (*this - vec).GetSquaredLength();
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object: "Vector3(x, y, z)"
	*/
	template<typename T>
	String Vector3<T>::ToString() const
	{
		StringStream ss;

		return ss << "Vector3(" << x << ", " << y << ", " << z <<')';
	}

	/*!
	* \brief Converts vector to pointer to its own data
	* \return A pointer to the own data
	*
	* \remark Access to index greather than 2 is undefined behavior
	*/
	template<typename T>
	Vector3<T>::operator T* ()
	{
		return &x;
	}

	/*!
	* \brief Converts vector to const pointer to its own data
	* \return A constant pointer to the own data
	*
	* \remark Access to index greather than 2 is undefined behavior
	*/
	template<typename T>
	Vector3<T>::operator const T* () const
	{
		return &x;
	}

	/*!
	* \brief Helps to represent the sign of the vector
	* \return A constant reference to this vector
	*/
	template<typename T>
	const Vector3<T>& Vector3<T>::operator+() const
	{
		return *this;
	}

	/*!
	* \brief Negates the components of the vector
	* \return A constant reference to this vector with negate components
	*/
	template<typename T>
	Vector3<T> Vector3<T>::operator-() const
	{
		return Vector3(-x, -y, -z);
	}

	/*!
	* \brief Adds the components of the vector with other vector
	* \return A vector where components are the sum of this vector and the other one
	*
	* \param vec The other vector to add components with
	*/
	template<typename T>
	Vector3<T> Vector3<T>::operator+(const Vector3& vec) const
	{
		return Vector3(x + vec.x, y + vec.y, z + vec.z);
	}

	/*!
	* \brief Substracts the components of the vector with other vector
	* \return A vector where components are the difference of this vector and the other one
	*
	* \param vec The other vector to substract components with
	*/
	template<typename T>
	Vector3<T> Vector3<T>::operator-(const Vector3& vec) const
	{
		return Vector3(x - vec.x, y - vec.y, z - vec.z);
	}

	/*!
	* \brief Multiplies the components of the vector with other vector
	* \return A vector where components are the product of this vector and the other one
	*
	* \param vec The other vector to multiply components with
	*/
	template<typename T>
	Vector3<T> Vector3<T>::operator*(const Vector3& vec) const
	{
		return Vector3(x * vec.x, y * vec.y, z * vec.z);
	}

	/*!
	* \brief Multiplies the components of the vector with a scalar
	* \return A vector where components are the product of this vector and the scalar
	*
	* \param scale The scalar to multiply components with
	*/
	template<typename T>
	Vector3<T> Vector3<T>::operator*(T scale) const
	{
		return Vector3(x * scale, y * scale, z * scale);
	}

	/*!
	* \brief Divides the components of the vector with other vector
	* \return A vector where components are the quotient of this vector and the other one
	*
	* \param vec The other vector to divide components with
	*
	* \remark Produce a NazaraError if one of the vec components is null with NAZARA_MATH_SAFE defined
	* \throw std::domain_error if NAZARA_MATH_SAFE is defined and one of the vec components is null
	*/
	template<typename T>
	Vector3<T> Vector3<T>::operator/(const Vector3& vec) const
	{
		#if NAZARA_MATH_SAFE
		if (NumberEquals(vec.x, F(0.0)) || NumberEquals(vec.y, F(0.0)) || NumberEquals(vec.z, F(0.0)))
		{
			String error("Division by zero");

			NazaraError(error);
			throw std::domain_error(error);
		}
		#endif

		return Vector3(x / vec.x, y / vec.y, z / vec.z);
	}

	/*!
	* \brief Divides the components of the vector with a scalar
	* \return A vector where components are the quotient of this vector and the scalar
	*
	* \param scale The scalar to divide components with
	*
	* \remark Produce a NazaraError if scale is null with NAZARA_MATH_SAFE defined
	* \throw std::domain_error if NAZARA_MATH_SAFE is defined and scale is null
	*/
	template<typename T>
	Vector3<T> Vector3<T>::operator/(T scale) const
	{
		#if NAZARA_MATH_SAFE
		if (NumberEquals(scale, F(0.0)))
		{
			String error("Division by zero");

			NazaraError(error);
			throw std::domain_error(error);
		}
		#endif

		return Vector3(x / scale, y / scale, z / scale);
	}

	/*!
	* \brief Adds the components of other vector to this vector
	* \return A reference to this vector where components are the sum of this vector and the other one
	*
	* \param vec The other vector to add components with
	*/
	template<typename T>
	Vector3<T>& Vector3<T>::operator+=(const Vector3& vec)
	{
		x += vec.x;
		y += vec.y;
		z += vec.z;

		return *this;
	}

	/*!
	* \brief Substracts the components of other vector to this vector
	* \return A reference to this vector where components are the difference of this vector and the other one
	*
	* \param vec The other vector to substract components with
	*/
	template<typename T>
	Vector3<T>& Vector3<T>::operator-=(const Vector3& vec)
	{
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;

		return *this;
	}

	/*!
	* \brief Multiplies the components of other vector to this vector
	* \return A reference to this vector where components are the product of this vector and the other one
	*
	* \param vec The other vector to multiply components with
	*/
	template<typename T>
	Vector3<T>& Vector3<T>::operator*=(const Vector3& vec)
	{
		x *= vec.x;
		y *= vec.y;
		z *= vec.z;

		return *this;
	}

	/*!
	* \brief Multiplies the components of other vector with a scalar
	* \return A reference to this vector where components are the product of this vector and the scalar
	*
	* \param vec The other vector to multiply components with
	*/
	template<typename T>
	Vector3<T>& Vector3<T>::operator*=(T scale)
	{
		x *= scale;
		y *= scale;
		z *= scale;

		return *this;
	}

	/*!
	* \brief Multiplies the components of other vector to this vector
	* \return A reference to this vector where components are the quotient of this vector and the other one
	*
	* \param vec The other vector to multiply components with
	*
	* \remark Produce a NazaraError if one of the vec components is null with NAZARA_MATH_SAFE defined
	* \throw std::domain_error if NAZARA_MATH_SAFE is defined and one of the vec components is null
	*/
	template<typename T>
	Vector3<T>& Vector3<T>::operator/=(const Vector3& vec)
	{
		if (NumberEquals(vec.x, F(0.0)) || NumberEquals(vec.y, F(0.0)) || NumberEquals(vec.z, F(0.0)))
		{
			String error("Division by zero");

			NazaraError(error);
			throw std::domain_error(error);
		}

		x /= vec.x;
		y /= vec.y;
		z /= vec.z;

		return *this;
	}

	/*!
	* \brief Divides the components of other vector with a scalar
	* \return A reference to this vector where components are the quotient of this vector and the scalar
	*
	* \param vec The other vector to divide components with
	*
	* \remark Produce a NazaraError if scale is null with NAZARA_MATH_SAFE defined
	* \throw std::domain_error if NAZARA_MATH_SAFE is defined and scale is null
	*/
	template<typename T>
	Vector3<T>& Vector3<T>::operator/=(T scale)
	{
		if (NumberEquals(scale, F(0.0)))
		{
			String error("Division by zero");

			NazaraError(error);
			throw std::domain_error(error);
		}

		x /= scale;
		y /= scale;
		z /= scale;

		return *this;
	}

	/*!
	* \brief Compares the vector to other one
	* \return true if the vectors are the same
	*
	* \param vec Other vector to compare with
	*/
	template<typename T>
	bool Vector3<T>::operator==(const Vector3& vec) const
	{
		return NumberEquals(x, vec.x) &&
		       NumberEquals(y, vec.y) &&
		       NumberEquals(z, vec.z);
	}

	/*!
	* \brief Compares the vector to other one
	* \return false if the vectors are the same
	*
	* \param vec Other vector to compare with
	*/
	template<typename T>
	bool Vector3<T>::operator!=(const Vector3& vec) const
	{
		return !operator==(vec);
	}

	/*!
	* \brief Compares the vector to other one
	* \return true if this vector has its first components inferior to the other ones
	*
	* \param vec Other vector to compare with
	*/
	template<typename T>
	bool Vector3<T>::operator<(const Vector3& vec) const
	{
		if (x == vec.x)
		{
			if (y == vec.y)
				return z < vec.z;
			else
				return y < vec.y;
		}
		else
			return x < vec.x;
	}

	/*!
	* \brief Compares the vector to other one
	* \return true if this vector has its first components inferior or equal to the other ones
	*
	* \param vec Other vector to compare with
	*/
	template<typename T>
	bool Vector3<T>::operator<=(const Vector3& vec) const
	{
		if (x == vec.x)
		{
			if (y == vec.y)
				return z <= vec.z;
			else
				return y < vec.y;
		}
		else
			return x < vec.x;
	}

	/*!
	* \brief Compares the vector to other one
	* \return true if this vector has its first components superior to the other ones
	*
	* \param vec Other vector to compare with
	*/
	template<typename T>
	bool Vector3<T>::operator>(const Vector3& vec) const
	{
		return !operator<=(vec);
	}

	/*!
	* \brief Compares the vector to other one
	* \return true if this vector has its first components superior or equal to the other ones
	*
	* \param vec Other vector to compare with
	*/
	template<typename T>
	bool Vector3<T>::operator>=(const Vector3& vec) const
	{
		return !operator<(vec);
	}

	/*!
	* \brief Calculates the cross product with two vectors
	* \return A vector which is the cross product according to "right-hand" rule
	*
	* \param vec1 The first vector to calculate the cross product with
	* \param vec2 The second vector to calculate the cross product with
	*
	* \see CrossProduct
	*/
	template<typename T>
	Vector3<T> Vector3<T>::CrossProduct(const Vector3& vec1, const Vector3& vec2)
	{
		return vec1.CrossProduct(vec2);
	}

	/*!
	* \brief Calculates the dot (scalar) product with two vectors
	* \return The value of the dot product
	*
	* \param vec1 The first vector to calculate the dot product with
	* \param vec2 The second vector to calculate the dot product with
	*
	* \see AbsDotProduct, DotProduct
	*/
	template<typename T>
	T Vector3<T>::DotProduct(const Vector3& vec1, const Vector3& vec2)
	{
		return vec1.DotProduct(vec2);
	}

	/*!
	* \brief Shorthand for the vector (0, 0, 1)
	* \return A vector with components (0, 0, 1)
	*
	* \see MakeBackward
	*/
	template<typename T>
	Vector3<T> Vector3<T>::Backward()
	{
		Vector3 vector;
		vector.MakeBackward();

		return vector;
	}

	/*!
	* \brief Measure the distance between two points
	* Shorthand for vec1.Distance(vec2)
	*
	* param vec1 the first point
	* param vec2 the second point
	*
	* \return The distance between the two vectors
	*
	* \see SquaredDistance
	*/
	template<typename T>
	T Vector3<T>::Distance(const Vector3& vec1, const Vector3& vec2)
	{
		return vec1.Distance(vec2);
	}

	/*!
	* \brief Measure the distance between two points as a float
	* Shorthand for vec1.Distancef(vec2)
	*
	* param vec1 the first point
	* param vec2 the second point
	*
	* \return The distance between the two vectors as a float
	*
	* \see SquaredDistancef
	*/
	template<typename T>
	float Vector3<T>::Distancef(const Vector3& vec1, const Vector3& vec2)
	{
		return vec1.Distancef(vec2);
	}

	/*!
	* \brief Shorthand for the vector (0, -1, 0)
	* \return A vector with components (0, -1, 0)
	*
	* \see MakeDown
	*/
	template<typename T>
	Vector3<T> Vector3<T>::Down()
	{
		Vector3 vector;
		vector.MakeDown();

		return vector;
	}

	/*!
	* \brief Shorthand for the vector (0, 0, -1)
	* \return A vector with components (0, 0, -1)
	*
	* \see Forward
	*/
	template<typename T>
	Vector3<T> Vector3<T>::Forward()
	{
		Vector3 vector;
		vector.MakeForward();

		return vector;
	}

	/*!
	* \brief Shorthand for the vector (-1, 0, 0)
	* \return A vector with components (-1, 0, 0)
	*
	* \see MakeLeft
	*/
	template<typename T>
	Vector3<T> Vector3<T>::Left()
	{
		Vector3 vector;
		vector.MakeLeft();

		return vector;
	}

	/*!
	* \brief Interpolates the vector to other one with a factor of interpolation
	* \return A new vector which is the interpolation of two vectors
	*
	* \param from Initial vector
	* \param to Target vector
	* \param interpolation Factor of interpolation
	*
	* \remark interpolation is meant to be between 0 and 1, other values are potentially undefined behavior
	*
	* \see Lerp
	*/
	template<typename T>
	Vector3<T> Vector3<T>::Lerp(const Vector3& from, const Vector3& to, T interpolation)
	{
		Vector3 dummy;
		dummy.x = Nz::Lerp(from.x, to.x, interpolation);
		dummy.y = Nz::Lerp(from.y, to.y, interpolation);
		dummy.z = Nz::Lerp(from.z, to.z, interpolation);

		return dummy;
	}

	/*!
	* \brief Gives the normalized vector
	* \return A normalized vector from the vec
	*
	* \param vec Vector to normalize
	*
	* \remark If the vector is (0, 0, 0), then it returns (0, 0, 0)
	*
	* \see GetNormal
	*/
	template<typename T>
	Vector3<T> Vector3<T>::Normalize(const Vector3& vec)
	{
		return vec.GetNormal();
	}

	/*!
	* \brief Shorthand for the vector (1, 0, 0)
	* \return A vector with components (1, 0, 0)
	*
	* \see MakeRight
	*/
	template<typename T>
	Vector3<T> Vector3<T>::Right()
	{
		Vector3 vector;
		vector.MakeRight();

		return vector;
	}

	/*!
	* \brief Calculates the squared distance between two vectors
	* \return The metric distance between two vectors with the squared euclidean norm
	*
	* \param vec1 The first point to measure the distance with
	* \param vec2 The second point to measure the distance with
	*
	* \see Distance
	*/
	template<typename T>
	T Vector3<T>::SquaredDistance(const Vector3& vec1, const Vector3& vec2)
	{
		return vec1.SquaredDistance(vec2);
	}

	/*!
	* \brief Shorthand for the vector (1, 1, 1)
	* \return A vector with components (1, 1, 1)
	*
	* \see MakeUnit
	*/
	template<typename T>
	Vector3<T> Vector3<T>::Unit()
	{
		Vector3 vector;
		vector.MakeUnit();

		return vector;
	}

	/*!
	* \brief Shorthand for the vector (1, 0, 0)
	* \return A vector with components (1, 0, 0)
	*
	* \see MakeUnitX
	*/
	template<typename T>
	Vector3<T> Vector3<T>::UnitX()
	{
		Vector3 vector;
		vector.MakeUnitX();

		return vector;
	}

	/*!
	* \brief Shorthand for the vector (0, 1, 0)
	* \return A vector with components (0, 1, 0)
	*
	* \see MakeUnitY
	*/
	template<typename T>
	Vector3<T> Vector3<T>::UnitY()
	{
		Vector3 vector;
		vector.MakeUnitY();

		return vector;
	}

	/*!
	* \brief Shorthand for the vector (0, 0, 1)
	* \return A vector with components (0, 0, 1)
	*
	* \see MakeUnitZ
	*/
	template<typename T>
	Vector3<T> Vector3<T>::UnitZ()
	{
		Vector3 vector;
		vector.MakeUnitZ();

		return vector;
	}

	/*!
	* \brief Shorthand for the vector (0, 1, 0)
	* \return A vector with components (0, 1, 0)
	*
	* \see MakeUp
	*/
	template<typename T>
	Vector3<T> Vector3<T>::Up()
	{
		Vector3 vector;
		vector.MakeUp();

		return vector;
	}

	/*!
	* \brief Shorthand for the vector (0, 0, 0)
	* \return A vector with components (0, 0, 0)
	*
	* \see MakeZero
	*/
	template<typename T>
	Vector3<T> Vector3<T>::Zero()
	{
		Vector3 vector;
		vector.MakeZero();

		return vector;
	}

	/*!
	* \brief Serializes a Vector3
	* \return true if successfully serialized
	*
	* \param context Serialization context
	* \param vector Input Vector3
	*/
	template<typename T>
	bool Serialize(SerializationContext& context, const Vector3<T>& vector)
	{
		if (!Serialize(context, vector.x))
			return false;

		if (!Serialize(context, vector.y))
			return false;

		if (!Serialize(context, vector.z))
			return false;

		return true;
	}

	/*!
	* \brief Unserializes a Vector3
	* \return true if successfully unserialized
	*
	* \param context Serialization context
	* \param vector Output Vector3
	*/
	template<typename T>
	bool Unserialize(SerializationContext& context, Vector3<T>* vector)
	{
		if (!Unserialize(context, &vector->x))
			return false;

		if (!Unserialize(context, &vector->y))
			return false;

		if (!Unserialize(context, &vector->z))
			return false;

		return true;
	}
}

/*!
* \brief Output operator
* \return The stream
*
* \param out The stream
* \param vec The vector to output
*/

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nz::Vector3<T>& vec)
{
	return out << vec.ToString();
}

/*!
* \brief Multiplies the components of the vector with a scalar
* \return A vector where components are the product of this vector and the scalar
*
* \param scale The scalar to multiply components with
*/

template<typename T>
Nz::Vector3<T> operator*(T scale, const Nz::Vector3<T>& vec)
{
	return Nz::Vector3<T>(scale * vec.x, scale * vec.y, scale * vec.z);
}

/*!
* \brief Divides the components of the vector with a scalar
* \return A vector where components are the quotient of this vector and the scalar
*
* \param scale The scalar to divide components with
*
* \remark Produce a NazaraError if scale is null with NAZARA_MATH_SAFE defined
* \throw std::domain_error if NAZARA_MATH_SAFE is defined and scale is null
*/

template<typename T>
Nz::Vector3<T> operator/(T scale, const Nz::Vector3<T>& vec)
{
	#if NAZARA_MATH_SAFE
	if (Nz::NumberEquals(vec.x, F(0.0)) || Nz::NumberEquals(vec.y, F(0.0)) || Nz::NumberEquals(vec.z, F(0.0)))
	{
		Nz::String error("Division by zero");

		NazaraError(error);
		throw std::domain_error(error);
	}
	#endif

	return Nz::Vector3<T>(scale / vec.x, scale / vec.y, scale / vec.z);
}

namespace std
{

template<class T>
struct hash<Nz::Vector3<T>>
{
    /*!
    * \brief Specialisation of std to hash
    * \return Result of the hash
    *
    * \param v Vector3 to hash
    */

    std::size_t operator()(const Nz::Vector3<T>& v) const
    {
        std::size_t seed {};

        Nz::HashCombine(seed, v.x);
        Nz::HashCombine(seed, v.y);
        Nz::HashCombine(seed, v.z);

        return seed;
    }
};

#undef F

#include <Nazara/Core/DebugOff.hpp>
