// Copyright (C) 2024 Rémi Bèges - Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Error.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <NazaraUtils/Hash.hpp>
#include <cstring>
#include <limits>
#include <stdexcept>

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
	constexpr Vector3<T>::Vector3(T X, T Y, T Z) :
	x(X),
	y(Y),
	z(Z)
	{
	}

	/*!
	* \brief Constructs a Vector3 object from a component and a Vector2<T>
	*
	* \param X X component
	* \param vec vec.X = Y component and vec.y = Z component
	*/
	template<typename T>
	constexpr Vector3<T>::Vector3(T X, const Vector2<T>& vec) :
	x(X),
	y(vec.x),
	z(vec.y)
	{
	}

	/*!
	* \brief Constructs explicitely a Vector3 object from its "scale"
	*
	* \param scale X component = Y component = Z component
	*/
	template<typename T>
	constexpr Vector3<T>::Vector3(T scale) :
	x(scale),
	y(scale),
	z(scale)
	{
	}

	/*!
	* \brief Constructs a Vector3 object from a Vector2<T> and a component
	*
	* \param vec vec.X = X component and vec.y = Y component
	* \param Z Z component
	*/
	template<typename T>
	constexpr Vector3<T>::Vector3(const Vector2<T>& vec, T Z) :
	x(vec.x),
	y(vec.y),
	z(Z)
	{
	}

	/*!
	* \brief Constructs a Vector3 object from another type of Vector3
	*
	* \param vec Vector of type U to convert to type T
	*/
	template<typename T>
	template<typename U>
	constexpr Vector3<T>::Vector3(const Vector3<U>& vec) :
	x(static_cast<T>(vec.x)),
	y(static_cast<T>(vec.y)),
	z(static_cast<T>(vec.z))
	{
	}

	/*!
	* \brief Constructs a Vector3 object from a Vector4
	*
	* \param vec Vector4 where only the first three components are taken
	*/
	template<typename T>
	constexpr Vector3<T>::Vector3(const Vector4<T>& vec):
	x(vec.x),
	y(vec.y),
	z(vec.z)
	{
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
	* \return The angle
	*
	* \param vec The other vector to measure the angle with
	*
	* \remark The vectors need to be normalised
	*
	* \see NormalizeAngle
	*/
	template<typename T>
	RadianAngle<T> Vector3<T>::AngleBetween(const Vector3& vec) const
	{
		T alpha = DotProduct(vec);
		return std::acos(Nz::Clamp(alpha, T(-1.0), T(1.0)));
	}

	template<typename T>
	constexpr bool Vector3<T>::ApproxEqual(const Vector3& vec, T maxDifference) const
	{
		return NumberEquals(x, vec.x, maxDifference) && NumberEquals(y, vec.y, maxDifference) && NumberEquals(z, vec.z, maxDifference);
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
	constexpr Vector3<T> Vector3<T>::CrossProduct(const Vector3& vec) const
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
	template<typename U>
	U Vector3<T>::Distance(const Vector3& vec) const
	{
		return static_cast<U>(std::sqrt(static_cast<U>(SquaredDistance(vec))));
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
	constexpr T Vector3<T>::DotProduct(const Vector3& vec) const
	{
		return x * vec.x + y * vec.y + z * vec.z;
	}

	/*!
	* \brief Returns the absolute of this vector, ie: Vector3(abs(x), abs(y), abs(z))
	* \return The absolute of this vector
	*/
	template<typename T>
	Vector3<T> Vector3<T>::GetAbs() const
	{
		return Vector3(std::abs(x), std::abs(y), std::abs(z));
	}

	/*!
	* \brief Calculates the length (magnitude) of the vector
	* \return The length of the vector
	*
	* \see GetSquaredLength
	*/
	template<typename T>
	template<typename U>
	U Vector3<T>::GetLength() const
	{
		return static_cast<U>(std::sqrt(static_cast<U>(GetSquaredLength())));
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
	constexpr T Vector3<T>::GetSquaredLength() const
	{
		return x*x + y*y + z*z;
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
	constexpr Vector3<T>& Vector3<T>::Maximize(const Vector3& vec)
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
	constexpr Vector3<T>& Vector3<T>::Minimize(const Vector3& vec)
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
		if (norm > T(0.0))
		{
			T invNorm = T(1.0) / norm;
			x *= invNorm;
			y *= invNorm;
			z *= invNorm;
		}

		if (length)
			*length = norm;

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
	constexpr T Vector3<T>::SquaredDistance(const Vector3& vec) const
	{
		return (*this - vec).GetSquaredLength();
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object: "Vector3(x, y, z)"
	*/
	template<typename T>
	std::string Vector3<T>::ToString() const
	{
		return "Vector3(" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ')';
	}

	/*!
	* \brief Access a vector component by index
	* \return X, Y, Z depending on index (0, 1, 2)
	*/
	template<typename T>
	constexpr T& Vector3<T>::operator[](std::size_t i)
	{
		NazaraAssert(i < 3, "index out of range");
		return *(&x + i);
	}

	/*!
	* \brief Access a vector component by index
	* \return X, Y, Z depending on index (0, 1, 2)
	*/
	template<typename T>
	constexpr const T& Vector3<T>::operator[](std::size_t i) const
	{
		NazaraAssert(i < 3, "index out of range");
		return *(&x + i);
	}

	/*!
	* \brief Helps to represent the sign of the vector
	* \return A constant reference to this vector
	*/
	template<typename T>
	constexpr const Vector3<T>& Vector3<T>::operator+() const
	{
		return *this;
	}

	/*!
	* \brief Negates the components of the vector
	* \return A constant reference to this vector with negate components
	*/
	template<typename T>
	constexpr Vector3<T> Vector3<T>::operator-() const
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
	constexpr Vector3<T> Vector3<T>::operator+(const Vector3& vec) const
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
	constexpr Vector3<T> Vector3<T>::operator-(const Vector3& vec) const
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
	constexpr Vector3<T> Vector3<T>::operator*(const Vector3& vec) const
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
	constexpr Vector3<T> Vector3<T>::operator*(T scale) const
	{
		return Vector3(x * scale, y * scale, z * scale);
	}

	/*!
	* \brief Divides the components of the vector with other vector
	* \return A vector where components are the quotient of this vector and the other one
	*
	* \param vec The other vector to divide components with
	*/
	template<typename T>
	constexpr Vector3<T> Vector3<T>::operator/(const Vector3& vec) const
	{
		return Vector3(x / vec.x, y / vec.y, z / vec.z);
	}

	/*!
	* \brief Divides the components of the vector with a scalar
	* \return A vector where components are the quotient of this vector and the scalar
	*
	* \param scale The scalar to divide components with
	*/
	template<typename T>
	constexpr Vector3<T> Vector3<T>::operator/(T scale) const
	{
		return Vector3(x / scale, y / scale, z / scale);
	}

	template<typename T>
	constexpr Vector3<T> Vector3<T>::operator%(const Vector3& vec) const
	{
		return Vector3(Mod(x, vec.x), Mod(y, vec.y), Mod(z, vec.z));
	}

	template<typename T>
	constexpr Vector3<T> Vector3<T>::operator%(T mod) const
	{
		return Vector3(Mod(x, mod), Mod(y, mod), Mod(z, mod));
	}

	/*!
	* \brief Adds the components of other vector to this vector
	* \return A reference to this vector where components are the sum of this vector and the other one
	*
	* \param vec The other vector to add components with
	*/
	template<typename T>
	constexpr Vector3<T>& Vector3<T>::operator+=(const Vector3& vec)
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
	constexpr Vector3<T>& Vector3<T>::operator-=(const Vector3& vec)
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
	constexpr Vector3<T>& Vector3<T>::operator*=(const Vector3& vec)
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
	* \param scale The scalar to multiply components with
	*/
	template<typename T>
	constexpr Vector3<T>& Vector3<T>::operator*=(T scale)
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
	*/
	template<typename T>
	constexpr Vector3<T>& Vector3<T>::operator/=(const Vector3& vec)
	{
		x /= vec.x;
		y /= vec.y;
		z /= vec.z;

		return *this;
	}

	/*!
	* \brief Divides the components of other vector with a scalar
	* \return A reference to this vector where components are the quotient of this vector and the scalar
	*
	* \param scale The scalar to divide components with
	*/
	template<typename T>
	constexpr Vector3<T>& Vector3<T>::operator/=(T scale)
	{
		x /= scale;
		y /= scale;
		z /= scale;

		return *this;
	}

	template<typename T>
	constexpr Vector3<T>& Vector3<T>::operator%=(const Vector3& vec)
	{
		x = Mod(x, vec.x);
		y = Mod(y, vec.y);
		z = Mod(z, vec.z);

		return *this;
	}

	template<typename T>
	constexpr Vector3<T>& Vector3<T>::operator%=(T mod)
	{
		x = Mod(x, mod);
		y = Mod(y, mod);
		z = Mod(z, mod);

		return *this;
	}

	/*!
	* \brief Compares the vector to other one
	* \return true if the vectors are the same
	*
	* \param vec Other vector to compare with
	*/
	template<typename T>
	constexpr bool Vector3<T>::operator==(const Vector3& vec) const
	{
		return x == vec.x && y == vec.y && z == vec.z;
	}

	/*!
	* \brief Compares the vector to other one
	* \return false if the vectors are the same
	*
	* \param vec Other vector to compare with
	*/
	template<typename T>
	constexpr bool Vector3<T>::operator!=(const Vector3& vec) const
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
	constexpr bool Vector3<T>::operator<(const Vector3& vec) const
	{
		if (x != vec.x)
			return x < vec.x;

		if (y != vec.y)
			return y < vec.y;

		return z < vec.z;
	}

	/*!
	* \brief Compares the vector to other one
	* \return true if this vector has its first components inferior or equal to the other ones
	*
	* \param vec Other vector to compare with
	*/
	template<typename T>
	constexpr bool Vector3<T>::operator<=(const Vector3& vec) const
	{
		if (x != vec.x)
			return x < vec.x;

		if (y != vec.y)
			return y < vec.y;

		return z <= vec.z;
	}

	/*!
	* \brief Compares the vector to other one
	* \return true if this vector has its first components superior to the other ones
	*
	* \param vec Other vector to compare with
	*/
	template<typename T>
	constexpr bool Vector3<T>::operator>(const Vector3& vec) const
	{
		if (x != vec.x)
			return x > vec.x;

		if (y != vec.y)
			return y > vec.y;

		return z > vec.z;
	}

	/*!
	* \brief Compares the vector to other one
	* \return true if this vector has its first components superior or equal to the other ones
	*
	* \param vec Other vector to compare with
	*/
	template<typename T>
	constexpr bool Vector3<T>::operator>=(const Vector3& vec) const
	{
		if (x != vec.x)
			return x > vec.x;

		if (y != vec.y)
			return y > vec.y;

		return z >= vec.z;
	}

	template<typename T>
	constexpr Vector3<T> Vector3<T>::Apply(T(*func)(T), const Vector3& vec)
	{
		return Vector3(func(vec.x), func(vec.y), func(vec.z));
	}

	template<typename T>
	constexpr bool Vector3<T>::ApproxEqual(const Vector3& lhs, const Vector3& rhs, T maxDifference)
	{
		return lhs.ApproxEqual(rhs, maxDifference);
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
	constexpr Vector3<T> Vector3<T>::CrossProduct(const Vector3& vec1, const Vector3& vec2)
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
	constexpr T Vector3<T>::DotProduct(const Vector3& vec1, const Vector3& vec2)
	{
		return vec1.DotProduct(vec2);
	}

	/*!
	* \brief Shorthand for the vector (0, 0, 1)
	* \return A vector with components (0, 0, 1)
	*/
	template<typename T>
	constexpr Vector3<T> Vector3<T>::Backward()
	{
		return Vector3(0, 0, 1);
	}

	template<typename T>
	constexpr Vector3<T> Vector3<T>::Clamp(const Vector3& vec, const Vector3& min, const Vector3& max)
	{
		return Vector3(
			std::clamp(vec.x, min.x, max.x),
			std::clamp(vec.y, min.y, max.y),
			std::clamp(vec.z, min.z, max.z)
		);
	}

	/*!
	* \brief Measure the distance between two points
	* Shorthand for vec1.Distance(vec2)
	*
	* param vec1 the first point
	* param vec2 the second point
	*
	* \return The distance between the two vectors
	*/
	template<typename T>
	template<typename U>
	U Vector3<T>::Distance(const Vector3& vec1, const Vector3& vec2)
	{
		return vec1.Distance<U>(vec2);
	}

	/*!
	* \brief Shorthand for the vector (0, -1, 0)
	* \return A vector with components (0, -1, 0)
	*/
	template<typename T>
	constexpr Vector3<T> Vector3<T>::Down()
	{
		return Vector3(0, -1, 0);
	}

	/*!
	* \brief Shorthand for the vector (0, 0, -1)
	* \return A vector with components (0, 0, -1)
	*/
	template<typename T>
	constexpr Vector3<T> Vector3<T>::Forward()
	{
		return Vector3(0, 0, -1);
	}

	/*!
	* \brief Shorthand for the vector (-1, 0, 0)
	* \return A vector with components (-1, 0, 0)
	*/
	template<typename T>
	constexpr Vector3<T> Vector3<T>::Left()
	{
		return Vector3(-1, 0, 0);
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
	*/
	template<typename T>
	constexpr Vector3<T> Vector3<T>::Lerp(const Vector3& from, const Vector3& to, T interpolation)
	{
		Vector3 result;
		result.x = Nz::Lerp(from.x, to.x, interpolation);
		result.y = Nz::Lerp(from.y, to.y, interpolation);
		result.z = Nz::Lerp(from.z, to.z, interpolation);

		return result;
	}

	template<typename T>
	constexpr Vector3<T> Vector3<T>::Max(const Vector3& lhs, const Vector3& rhs)
	{
		Vector3 max = lhs;
		max.Maximize(rhs);

		return max;
	}

	template<typename T>
	constexpr Vector3<T> Vector3<T>::Min(const Vector3& lhs, const Vector3& rhs)
	{
		Vector3 min = lhs;
		min.Minimize(rhs);

		return min;
	}

	template<typename T>
	Vector3<T> Vector3<T>::RotateTowards(const Vector3& from, const Vector3& to, RadianAngle<T> maxAngle)
	{
		// https://gamedev.stackexchange.com/a/203036
		RadianAngle<T> angleBetween = from.AngleBetween(to);
		if (angleBetween < maxAngle)
			return to;

		Vector3 axis = CrossProduct(from, to);

		Quaternion<T> rotationIncrement = Quaternion(maxAngle, axis);
		return rotationIncrement * from;
	}

	/*!
	* \brief Gives the normalized vector
	* \return A normalized vector from the vec
	*
	* \param vec Vector to normalize
	*
	* \remark If the vector is (0, 0, 0), then it returns (0, 0, 0)
	*/
	template<typename T>
	Vector3<T> Vector3<T>::Normalize(const Vector3& vec)
	{
		return vec.GetNormal();
	}

	/*!
	* \brief Shorthand for the vector (1, 0, 0)
	* \return A vector with components (1, 0, 0)
	*/
	template<typename T>
	constexpr Vector3<T> Vector3<T>::Right()
	{
		return Vector3(1, 0, 0);
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
	constexpr T Vector3<T>::SquaredDistance(const Vector3& vec1, const Vector3& vec2)
	{
		return vec1.SquaredDistance(vec2);
	}

	/*!
	* \brief Shorthand for the vector (1, 1, 1)
	* \return A vector with components (1, 1, 1)
	*/
	template<typename T>
	constexpr Vector3<T> Vector3<T>::Unit()
	{
		return Vector3(1);
	}

	/*!
	* \brief Shorthand for the vector (1, 0, 0)
	* \return A vector with components (1, 0, 0)
	*/
	template<typename T>
	constexpr Vector3<T> Vector3<T>::UnitX()
	{
		return Vector3(1, 0, 0);
	}

	/*!
	* \brief Shorthand for the vector (0, 1, 0)
	* \return A vector with components (0, 1, 0)
	*/
	template<typename T>
	constexpr Vector3<T> Vector3<T>::UnitY()
	{
		return Vector3(0, 1, 0);
	}

	/*!
	* \brief Shorthand for the vector (0, 0, 1)
	* \return A vector with components (0, 0, 1)
	*/
	template<typename T>
	constexpr Vector3<T> Vector3<T>::UnitZ()
	{
		return Vector3(0, 0, 1);
	}

	/*!
	* \brief Shorthand for the vector (0, 1, 0)
	* \return A vector with components (0, 1, 0)
	*/
	template<typename T>
	constexpr Vector3<T> Vector3<T>::Up()
	{
		return Vector3(0, 1, 0);
	}

	/*!
	* \brief Shorthand for the vector (0, 0, 0)
	* \return A vector with components (0, 0, 0)
	*/
	template<typename T>
	constexpr Vector3<T> Vector3<T>::Zero()
	{
		return Vector3(0, 0, 0);
	}

	/*!
	* \brief Serializes a Vector3
	* \return true if successfully serialized
	*
	* \param context Serialization context
	* \param vector Input Vector3
	*/
	template<typename T>
	bool Serialize(SerializationContext& context, const Vector3<T>& vector, TypeTag<Vector3<T>>)
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
	bool Unserialize(SerializationContext& context, Vector3<T>* vector, TypeTag<Vector3<T>>)
	{
		if (!Unserialize(context, &vector->x))
			return false;

		if (!Unserialize(context, &vector->y))
			return false;

		if (!Unserialize(context, &vector->z))
			return false;

		return true;
	}

	/*!
	* \brief Output operator
	* \return The stream
	*
	* \param out The stream
	* \param vec The vector to output
	*/
	template<typename T>
	std::ostream& operator<<(std::ostream& out, const Vector3<T>& vec)
	{
		return out << "Vector3(" << vec.x << ", " << vec.y << ", " << vec.z << ')';
	}

	/*!
	* \brief Multiplies the components of the vector with a scalar
	* \return A vector where components are the product of this vector and the scalar
	*
	* \param scale The scalar to multiply components with
	*/
	template<typename T>
	constexpr Vector3<T> operator*(T scale, const Vector3<T>& vec)
	{
		return Vector3<T>(scale * vec.x, scale * vec.y, scale * vec.z);
	}

	/*!
	* \brief Divides the components of the vector with a scalar
	* \return A vector where components are the quotient of this vector and the scalar
	*
	* \param scale The scalar to divide components with
	*/
	template<typename T>
	constexpr Vector3<T> operator/(T scale, const Vector3<T>& vec)
	{
		return Vector3<T>(scale / vec.x, scale / vec.y, scale / vec.z);
	}

	template<typename T>
	constexpr Vector3<T> operator%(T mod, const Vector3<T>& vec)
	{
		return Vector3<T>(Mod(mod, vec.x), Mod(mod, vec.y), Mod(mod, vec.z));
	}
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
			return Nz::HashCombine(v.x, v.y, v.z);
		}
	};
}

