// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <cstring>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup math
	* \class Nz::Vector2
	* \brief Math class that represents an element of the two dimensional vector space
	*/

	/*!
	* \brief Constructs a Vector2 object from its coordinates
	*
	* \param X X component
	* \param Y Y component
	*/

	template<typename T>
	Vector2<T>::Vector2(T X, T Y)
	{
		Set(X, Y);
	}

	/*!
	* \brief Constructs explicitely a Vector2 object from its "scale"
	*
	* \param scale X component = Y component
	*/

	template<typename T>
	Vector2<T>::Vector2(T scale)
	{
		Set(scale);
	}

	/*!
	* \brief Constructs a Vector2 object from another type of Vector2
	*
	* \param vec Vector of type U to convert to type T
	*/

	template<typename T>
	template<typename U>
	Vector2<T>::Vector2(const Vector2<U>& vec)
	{
		Set(vec);
	}

	/*!
	* \brief Constructs a Vector2 object from a Vector3
	*
	* \param vec Vector3 where only the first two components are taken
	*/

	template<typename T>
	Vector2<T>::Vector2(const Vector3<T>& vec)
	{
		Set(vec);
	}

	/*!
	* \brief Constructs a Vector2 object from a Vector4
	*
	* \param vec Vector4 where only the first two components are taken
	*/

	template<typename T>
	Vector2<T>::Vector2(const Vector4<T>& vec)
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
	T Vector2<T>::AbsDotProduct(const Vector2& vec) const
	{
		return std::abs(x * vec.x) + std::abs(y * vec.y);
	}

	/*!
	* \brief Calculates the angle between two vectors in orthonormal basis
	* \return The angle
	*
	* \param vec The other vector to measure the angle with
	*
	* \remark The vectors do not need to be normalised and if the angle is normalised, it represents the rotation from *this to vec in anti-clockwise direction
	*
	* \see NormalizeAngle
	*/

	template<typename T>
	RadianAngle<T> Vector2<T>::AngleBetween(const Vector2& vec) const
	{
		return std::atan2(vec.y, vec.x) - std::atan2(y, x);
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
	U Vector2<T>::Distance(const Vector2& vec) const
	{
		return static_cast<U>(std::sqrt(SquaredDistance(vec)));
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
	T Vector2<T>::DotProduct(const Vector2& vec) const
	{
		return x*vec.x + y*vec.y;
	}

	/*!
	* \brief Calculates the length (magnitude) of the vector
	* \return The length of the vector
	*
	* \see GetSquaredLength
	*/

	template<typename T>
	T Vector2<T>::GetLength() const
	{
		return static_cast<T>(std::sqrt(GetSquaredLength()));
	}

	/*!
	* \brief Calculates the length (magnitude) of the vector
	* \return The length in float of the vector
	*/

	template<typename T>
	float Vector2<T>::GetLengthf() const
	{
		return std::sqrt(static_cast<float>(GetSquaredLength()));
	}

	/*!
	* \brief Gets a copy normalized of the vector
	* \return A new vector which is the vector normalized
	*
	* \param length Optional argument to obtain the length's ratio of the vector and the unit-length
	*
	* \remark If this vector is (0, 0), then it returns (0, 0) and length is 0
	*
	* \see Normalize
	*/

	template<typename T>
	Vector2<T> Vector2<T>::GetNormal(T* length) const
	{
		Vector2 vec(*this);
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
	T Vector2<T>::GetSquaredLength() const
	{
		return x*x + y*y;
	}

	/*!
	* \brief Makes the vector (1, 1)
	* \return A reference to this vector with components (1, 1)
	*
	* \see Unit
	*/

	template<typename T>
	Vector2<T>& Vector2<T>::MakeUnit()
	{
		return Set(T(1.0), T(1.0));
	}

	/*!
	* \brief Makes the vector (1, 0)
	* \return A reference to this vector with components (1, 0)
	*
	* \see UnitX
	*/

	template<typename T>
	Vector2<T>& Vector2<T>::MakeUnitX()
	{
		return Set(T(1.0), T(0.0));
	}

	/*!
	* \brief Makes the vector (0, 1)
	* \return A reference to this vector with components (0, 1)
	*
	* \see UnitY
	*/

	template<typename T>
	Vector2<T>& Vector2<T>::MakeUnitY()
	{
		return Set(T(0.0), T(1.0));
	}

	/*!
	* \brief Makes the vector (0, 0)
	* \return A reference to this vector with components (0, 0)
	*
	* \see Zero
	*/

	template<typename T>
	Vector2<T>& Vector2<T>::MakeZero()
	{
		return Set(T(0.0), T(0.0));
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
	Vector2<T>& Vector2<T>::Maximize(const Vector2& vec)
	{
		if (vec.x > x)
			x = vec.x;

		if (vec.y > y)
			y = vec.y;

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
	Vector2<T>& Vector2<T>::Minimize(const Vector2& vec)
	{
		if (vec.x < x)
			x = vec.x;

		if (vec.y < y)
			y = vec.y;

		return *this;
	}

	/*!
	* \brief Normalizes the current vector
	* \return A reference to this vector
	*
	* \param length Optional argument to obtain the length's ratio of the vector and the unit-length
	*
	* \remark If the vector is (0, 0), then it returns (0, 0) and length is 0
	*
	* \see GetNormal
	*/

	template<typename T>
	Vector2<T>& Vector2<T>::Normalize(T* length)
	{
		T norm = GetLength();
		if (norm > T(0.0))
		{
			T invNorm = T(1.0) / norm;
			x *= invNorm;
			y *= invNorm;
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
	*/

	template<typename T>
	Vector2<T>& Vector2<T>::Set(T X, T Y)
	{
		x = X;
		y = Y;

		return *this;
	}

	/*!
	* \brief Sets the components of the vector from a "scale"
	* \return A reference to this vector
	*
	* \param scale X component = Y component
	*/

	template<typename T>
	Vector2<T>& Vector2<T>::Set(T scale)
	{
		x = scale;
		y = scale;

		return *this;
	}

	/*!
	* \brief Sets the components of the vector from an array of two elements
	* \return A reference to this vector
	*
	* \param vec[2] vec[0] is X component and vec[1] is Y component
	*/
	template<typename T>
	Vector2<T>& Vector2<T>::Set(const T* vec)
	{
		x = vec[0];
		y = vec[1];

		return *this;
	}

	/*!
	* \brief Sets the components of the vector from another type of Vector2
	* \return A reference to this vector
	*
	* \param vec Vector of type U to convert its components
	*/

	template<typename T>
	template<typename U>
	Vector2<T>& Vector2<T>::Set(const Vector2<U>& vec)
	{
		x = T(vec.x);
		y = T(vec.y);

		return *this;
	}

	/*!
	* \brief Sets the components of the vector from a Vector3
	* \return A reference to this vector
	*
	* \param vec Vector3 where only the first two components are taken
	*/

	template<typename T>
	Vector2<T>& Vector2<T>::Set(const Vector3<T>& vec)
	{
		x = vec.x;
		y = vec.y;

		return *this;
	}

	/*!
	* \brief Sets the components of the vector from a Vector4
	* \return A reference to this vector
	*
	* \param vec Vector4 where only the first two components are taken
	*/

	template<typename T>
	Vector2<T>& Vector2<T>::Set(const Vector4<T>& vec)
	{
		x = vec.x;
		y = vec.y;

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
	T Vector2<T>::SquaredDistance(const Vector2& vec) const
	{
		return (*this - vec).GetSquaredLength();
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object: "Vector2(x, y)"
	*/

	template<typename T>
	std::string Vector2<T>::ToString() const
	{
		std::ostringstream ss;
		ss << *this;

		return ss.str();
	}

	/*!
	* \brief Access a vector component by index
	* \return X, Y depending on index (0, 1)
	*/
	template<typename T>
	T& Vector2<T>::operator[](std::size_t i)
	{
		NazaraAssert(i < 2, "index out of range");
		return *(&x + i);
	}

	/*!
	* \brief Access a vector component by index
	* \return X, Y depending on index (0, 1)
	*/
	template<typename T>
	T Vector2<T>::operator[](std::size_t i) const
	{
		NazaraAssert(i < 2, "index out of range");
		return *(&x + i);
	}

	/*!
	* \brief Helps to represent the sign of the vector
	* \return A constant reference to this vector
	*/

	template<typename T>
	const Vector2<T>& Vector2<T>::operator+() const
	{
		return *this;
	}

	/*!
	* \brief Negates the components of the vector
	* \return A constant reference to this vector with negate components
	*/

	template<typename T>
	Vector2<T> Vector2<T>::operator-() const
	{
		return Vector2(-x, -y);
	}

	/*!
	* \brief Adds the components of the vector with other vector
	* \return A vector where components are the sum of this vector and the other one
	*
	* \param vec The other vector to add components with
	*/

	template<typename T>
	Vector2<T> Vector2<T>::operator+(const Vector2& vec) const
	{
		return Vector2(x + vec.x, y + vec.y);
	}

	/*!
	* \brief Substracts the components of the vector with other vector
	* \return A vector where components are the difference of this vector and the other one
	*
	* \param vec The other vector to substract components with
	*/

	template<typename T>
	Vector2<T> Vector2<T>::operator-(const Vector2& vec) const
	{
		return Vector2(x - vec.x, y - vec.y);
	}

	/*!
	* \brief Multiplies the components of the vector with other vector
	* \return A vector where components are the product of this vector and the other one
	*
	* \param vec The other vector to multiply components with
	*/

	template<typename T>
	Vector2<T> Vector2<T>::operator*(const Vector2& vec) const
	{
		return Vector2(x * vec.x, y * vec.y);
	}

	/*!
	* \brief Multiplies the components of the vector with a scalar
	* \return A vector where components are the product of this vector and the scalar
	*
	* \param scale The scalar to multiply components with
	*/

	template<typename T>
	Vector2<T> Vector2<T>::operator*(T scale) const
	{
		return Vector2(x * scale, y * scale);
	}

	/*!
	* \brief Divides the components of the vector with other vector
	* \return A vector where components are the quotient of this vector and the other one
	*
	* \param vec The other vector to divide components with
	*/

	template<typename T>
	Vector2<T> Vector2<T>::operator/(const Vector2& vec) const
	{
		return Vector2(x / vec.x, y / vec.y);
	}

	/*!
	* \brief Divides the components of the vector with a scalar
	* \return A vector where components are the quotient of this vector and the scalar
	*
	* \param scale The scalar to divide components with
	*/

	template<typename T>
	Vector2<T> Vector2<T>::operator/(T scale) const
	{
		return Vector2(x / scale, y / scale);
	}

	/*!
	* \brief Adds the components of other vector to this vector
	* \return A reference to this vector where components are the sum of this vector and the other one
	*
	* \param vec The other vector to add components with
	*/

	template<typename T>
	Vector2<T>& Vector2<T>::operator+=(const Vector2& vec)
	{
		x += vec.x;
		y += vec.y;

		return *this;
	}

	/*!
	* \brief Substracts the components of other vector to this vector
	* \return A reference to this vector where components are the difference of this vector and the other one
	*
	* \param vec The other vector to substract components with
	*/

	template<typename T>
	Vector2<T>& Vector2<T>::operator-=(const Vector2& vec)
	{
		x -= vec.x;
		y -= vec.y;

		return *this;
	}

	/*!
	* \brief Multiplies the components of other vector to this vector
	* \return A reference to this vector where components are the product of this vector and the other one
	*
	* \param vec The other vector to multiply components with
	*/

	template<typename T>
	Vector2<T>& Vector2<T>::operator*=(const Vector2& vec)
	{
		x *= vec.x;
		y *= vec.y;

		return *this;
	}

	/*!
	* \brief Multiplies the components of other vector with a scalar
	* \return A reference to this vector where components are the product of this vector and the scalar
	*
	* \param scale The scalar to multiply components with
	*/

	template<typename T>
	Vector2<T>& Vector2<T>::operator*=(T scale)
	{
		x *= scale;
		y *= scale;

		return *this;
	}

	/*!
	* \brief Multiplies the components of other vector to this vector
	* \return A reference to this vector where components are the quotient of this vector and the other one
	*
	* \param vec The other vector to multiply components with
	*/

	template<typename T>
	Vector2<T>& Vector2<T>::operator/=(const Vector2& vec)
	{
		x /= vec.x;
		y /= vec.y;

		return *this;
	}

	/*!
	* \brief Divides the components of other vector with a scalar
	* \return A reference to this vector where components are the quotient of this vector and the scalar
	*
	* \param scale The scalar to divide components with
	*/

	template<typename T>
	Vector2<T>& Vector2<T>::operator/=(T scale)
	{
		x /= scale;
		y /= scale;

		return *this;
	}

	/*!
	* \brief Compares the vector to other one
	* \return true if the vectors are the same
	*
	* \param vec Other vector to compare with
	*/

	template<typename T>
	bool Vector2<T>::operator==(const Vector2& vec) const
	{
		return NumberEquals(x, vec.x) &&
		       NumberEquals(y, vec.y);
	}

	/*!
	* \brief Compares the vector to other one
	* \return false if the vectors are the same
	*
	* \param vec Other vector to compare with
	*/

	template<typename T>
	bool Vector2<T>::operator!=(const Vector2& vec) const
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
	bool Vector2<T>::operator<(const Vector2& vec) const
	{
		if (x == vec.x)
			return y < vec.y;
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
	bool Vector2<T>::operator<=(const Vector2& vec) const
	{
		if (x == vec.x)
			return y <= vec.y;
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
	bool Vector2<T>::operator>(const Vector2& vec) const
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
	bool Vector2<T>::operator>=(const Vector2& vec) const
	{
		return !operator<(vec);
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
	template<typename U>
	U Vector2<T>::Distance(const Vector2& vec1, const Vector2& vec2)
	{
		return vec1.Distance<U>(vec2);
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
	T Vector2<T>::DotProduct(const Vector2& vec1, const Vector2& vec2)
	{
		return vec1.DotProduct(vec2);
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
	Vector2<T> Vector2<T>::Lerp(const Vector2& from, const Vector2& to, T interpolation)
	{
		Vector2 dummy;
		dummy.x = Nz::Lerp(from.x, to.x, interpolation);
		dummy.y = Nz::Lerp(from.y, to.y, interpolation);

		return dummy;
	}

	/*!
	* \brief Gives the normalized vector
	* \return A normalized vector from the vec
	*
	* \param vec Vector to normalize
	*
	* \remark If the vector is (0, 0), then it returns (0, 0)
	*
	* \see GetNormal
	*/

	template<typename T>
	Vector2<T> Vector2<T>::Normalize(const Vector2& vec)
	{
		return vec.GetNormal();
	}

	/*!
	* \brief Shorthand for the vector (1, 1)
	* \return A vector with components (1, 1)
	*
	* \see MakeUnit
	*/

	template<typename T>
	Vector2<T> Vector2<T>::Unit()
	{
		Vector2 vector;
		vector.MakeUnit();

		return vector;
	}

	/*!
	* \brief Shorthand for the vector (1, 0)
	* \return A vector with components (1, 0)
	*
	* \see MakeUnitX
	*/

	template<typename T>
	Vector2<T> Vector2<T>::UnitX()
	{
		Vector2 vector;
		vector.MakeUnitX();

		return vector;
	}

	/*!
	* \brief Shorthand for the vector (0, 1)
	* \return A vector with components (0, 1)
	*
	* \see MakeUnitY
	*/
	template<typename T>
	Vector2<T> Vector2<T>::UnitY()
	{
		Vector2 vector;
		vector.MakeUnitY();

		return vector;
	}

	/*!
	* \brief Shorthand for the vector (0, 0)
	* \return A vector with components (0, 0)
	*
	* \see MakeZero
	*/
	template<typename T>
	Vector2<T> Vector2<T>::Zero()
	{
		Vector2 vector;
		vector.MakeZero();

		return vector;
	}

	/*!
	* \brief Serializes a Vector2
	* \return true if successfully serialized
	*
	* \param context Serialization context
	* \param vector Input Vector2
	*/
	template<typename T> 
	bool Serialize(SerializationContext& context, const Vector2<T>& vector, TypeTag<Vector2<T>>)
	{
		if (!Serialize(context, vector.x))
			return false;

		if (!Serialize(context, vector.y))
			return false;

		return true;
	}

	/*!
	* \brief Unserializes a Vector2
	* \return true if successfully unserialized
	*
	* \param context Serialization context
	* \param vector Output Vector2
	*/
	template<typename T>
	bool Unserialize(SerializationContext& context, Vector2<T>* vector, TypeTag<Vector2<T>>)
	{
		if (!Unserialize(context, &vector->x))
			return false;

		if (!Unserialize(context, &vector->y))
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
std::ostream& operator<<(std::ostream& out, const Nz::Vector2<T>& vec)
{
	return out << "Vector2(" << vec.x << ", " << vec.y << ')';
}

/*!
* \brief Multiplies the components of the vector with a scalar
* \return A vector where components are the product of this vector and the scalar
*
* \param scale The scalar to multiply components with
*/

template<typename T>
Nz::Vector2<T> operator*(T scale, const Nz::Vector2<T>& vec)
{
	return Nz::Vector2<T>(scale * vec.x, scale * vec.y);
}

/*!
* \brief Divides the components of the vector with a scalar
* \return A vector where components are the quotient of this vector and the scalar
*
* \param scale The scalar to divide components with
*/

template<typename T>
Nz::Vector2<T> operator/(T scale, const Nz::Vector2<T>& vec)
{
	return Nz::Vector2<T>(scale / vec.x, scale / vec.y);
}

namespace std
{
	template<class T>
	struct hash<Nz::Vector2<T>>
	{
		/*!
		* \brief Specialisation of std to hash
		* \return Result of the hash
		*
		* \param v Vector2 to hash
		*/
		std::size_t operator()(const Nz::Vector2<T>& v) const
		{
			std::size_t seed {};

			Nz::HashCombine(seed, v.x);
			Nz::HashCombine(seed, v.y);

			return seed;
		}
	};
}

#include <Nazara/Core/DebugOff.hpp>
