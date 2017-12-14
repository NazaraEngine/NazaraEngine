// Copyright (C) 2017 Rémi Bèges - Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <cstring>
#include <stdexcept>
#include <Nazara/Core/Debug.hpp>

///FIXME: Les calculs effectués ici sont probablements tous faux, la composante W étant spéciale dans le monde de la 3D

#define F(a) static_cast<T>(a)

namespace Nz
{

	/*!
    * \ingroup math
	* \class Nz::Vector4
	* \brief Math class that represents an element of the three dimensional vector space with the notion of projectivity. When the fourth component is 1, it describes an 'usual' point and when it is 0, it represents the point at infinity
	*/

	/*!
	* \brief Constructs a Vector4 object from its coordinates
	*
	* \param X X component
	* \param Y Y component
	* \param Z Z component
	* \param W W component
	*/

	template<typename T>
	Vector4<T>::Vector4(T X, T Y, T Z, T W)
	{
		Set(X, Y, Z, W);
	}

	/*!
	* \brief Constructs a Vector4 object from two components and a Vector2<T>
	*
	* \param X X component
	* \param Y Y component
	* \param vec vec.X = Z component and vec.y = W component
	*/

	template<typename T>
	Vector4<T>::Vector4(T X, T Y, const Vector2<T>& vec)
	{
		Set(X, Y, vec);
	}

	/*!
	* \brief Constructs a Vector4 object from one component, a Vector2<T> and one component
	*
	* \param X X component
	* \param vec vec.X = Y component and vec.y = Z component
	* \param W W component
	*/

	template<typename T>
	Vector4<T>::Vector4(T X, const Vector2<T>& vec, T W)
	{
		Set(X, vec, W);
	}

	/*!
	* \brief Constructs a Vector4 object from one component and a Vector3<T>
	*
	* \param X X component
	* \param vec vec.X = Y component, vec.y = Z component and vec.z = W component
	*/

	template<typename T>
	Vector4<T>::Vector4(T X, const Vector3<T>& vec)
	{
		Set(X, vec);
	}

	/*!
	* \brief Constructs explicitely a Vector4 object from its "scale"
	*
	* \param scale X component = Y component = Z component = W component
	*/

	template<typename T>
	Vector4<T>::Vector4(T scale)
	{
		Set(scale);
	}

	/*!
	* \brief Constructs a Vector4 object from an array of four elements
	*
	* \param vec[4] vec[0] is X component, vec[1] is Y component, vec[2] is Z component and vec[3] is W component
	*/

	template<typename T>
	Vector4<T>::Vector4(const T vec[4])
	{
		Set(vec);
	}

	/*!
	* \brief Constructs a Vector4 object from a Vector2<T> and two components
	*
	* \param vec vec.X = X component and vec.y = Y component
	* \param Z Z component
	* \param W W component
	*/

	template<typename T>
	Vector4<T>::Vector4(const Vector2<T>& vec, T Z, T W)
	{
		Set(vec, Z, W);
	}

	/*!
	* \brief Constructs a Vector4 object from one component and a Vector3<T>
	*
	* \param vec vec.X = X component, vec.y = Y component and vec.z = Z component
	* \param W W component
	*/

	template<typename T>
	Vector4<T>::Vector4(const Vector3<T>& vec, T W)
	{
		Set(vec, W);
	}

	/*!
	* \brief Constructs a Vector4 object from another type of Vector4
	*
	* \param vec Vector of type U to convert to type T
	*/

	template<typename T>
	template<typename U>
	Vector4<T>::Vector4(const Vector4<U>& vec)
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
	T Vector4<T>::AbsDotProduct(const Vector4& vec) const
	{
		return std::abs(x * vec.x) + std::abs(y * vec.y) + std::abs(z * vec.z) + std::abs(w * vec.w);
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
	T Vector4<T>::DotProduct(const Vector4& vec) const
	{
		return x*vec.x + y*vec.y + z*vec.z + w*vec.w;
	}

	/*!
	* \brief Gets a copy normalized of the vector
	* \return A new vector which is the vector normalized
	*
	* \param length Optional argument to obtain the length's ratio of the vector and the unit-length in this case w
	*
	* \see Normalize
	*/

	template<typename T>
	Vector4<T> Vector4<T>::GetNormal(T* length) const
	{
		Vector4<T> vec(*this);
		vec.Normalize(length);

		return vec;
	}

	/*!
	* \brief Makes the vector (1, 0, 0, 1)
	* \return A reference to this vector with components (1, 0, 0, 1)
	*
	* \see UnitX
	*/

	template<typename T>
	Vector4<T>& Vector4<T>::MakeUnitX()
	{
		return Set(F(1.0), F(0.0), F(0.0), F(1.0));
	}

	/*!
	* \brief Makes the vector (0, 1, 0, 1)
	* \return A reference to this vector with components (0, 1, 0, 1)
	*
	* \see UnitY
	*/

	template<typename T>
	Vector4<T>& Vector4<T>::MakeUnitY()
	{
		return Set(F(0.0), F(1.0), F(0.0), F(1.0));
	}

	/*!
	* \brief Makes the vector (0, 0, 1, 1)
	* \return A reference to this vector with components (0, 0, 1, 1)
	*
	* \see UnitZ
	*/

	template<typename T>
	Vector4<T>& Vector4<T>::MakeUnitZ()
	{
		return Set(F(0.0), F(0.0), F(1.0), F(1.0));
	}

	/*!
	* \brief Makes the vector (0, 0, 0, 1)
	* \return A reference to this vector with components (0, 0, 0, 1)
	*
	* \see Zero
	*/

	template<typename T>
	Vector4<T>& Vector4<T>::MakeZero()
	{
		return Set(F(0.0), F(0.0), F(0.0), F(1.0));
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
	Vector4<T>& Vector4<T>::Maximize(const Vector4& vec)
	{
		if (vec.x > x)
			x = vec.x;

		if (vec.y > y)
			y = vec.y;

		if (vec.z > z)
			z = vec.z;

		if (vec.w > w)
			w = vec.w;

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
	Vector4<T>& Vector4<T>::Minimize(const Vector4& vec)
	{
		if (vec.x < x)
			x = vec.x;

		if (vec.y < y)
			y = vec.y;

		if (vec.z < z)
			z = vec.z;

		if (vec.w < w)
			w = vec.w;

		return *this;
	}

	/*!
	* \brief Gives the normalized vector
	* \return A normalized vector from the vec with w = 1
	*
	* \param length Optional argument to obtain the length's ratio of the vector in this case w
	*
	* \see GetNormal
	*/

	template<typename T>
	Vector4<T>& Vector4<T>::Normalize(T* length)
	{
		T invLength = F(1.0)/w;
		x *= invLength; // Warning, change this logic will break Frustum::Extract
		y *= invLength;
		z *= invLength;

		if (length)
			*length = w;

		w = F(1.0);

		return *this;
	}

	/*!
	* \brief Sets the components of the vector
	* \return A reference to this vector
	*
	* \param X X component
	* \param Y Y component
	* \param Z Z component
	* \param W W component
	*/

	template<typename T>
	Vector4<T>& Vector4<T>::Set(T X, T Y, T Z, T W)
	{
		x = X;
		y = Y;
		z = Z;
		w = W;

		return *this;
	}

	/*!
	* \brief Sets the components of the vector from two components and a Vector2
	* \return A reference to this vector
	*
	* \param X X component
	* \param Y Y component
	* \param vec vec.X = Z component and vec.y = W component
	*/

	template<typename T>
	Vector4<T>& Vector4<T>::Set(T X, T Y, const Vector2<T>& vec)
	{
		x = X;
		y = Y;
		z = vec.x;
		w = vec.y;

		return *this;
	}

	/*!
	* \brief Sets the components of the vector from one component, a Vector2 and one component
	* \return A reference to this vector
	*
	* \param X X component
	* \param vec vec.X = Y component and vec.y = Z component
	* \param W W component
	*/

	template<typename T>
	Vector4<T>& Vector4<T>::Set(T X, const Vector2<T>& vec, T W)
	{
		x = X;
		y = vec.x;
		z = vec.y;
		w = W;

		return *this;
	}

	/*!
	* \brief Sets the components of the vector from one component and a Vector3
	* \return A reference to this vector
	*
	* \param X X component
	* \param vec vec.X = Y component, vec.y = Z component and vec.z = W component
	*/

	template<typename T>
	Vector4<T>& Vector4<T>::Set(T X, const Vector3<T>& vec)
	{
		x = X;
		y = vec.x;
		z = vec.y;
		w = vec.z;

		return *this;
	}

	/*!
	* \brief Sets the components of the vector from a "scale"
	* \return A reference to this vector
	*
	* \param scale X component = Y component = Z component = W component
	*/

	template<typename T>
	Vector4<T>& Vector4<T>::Set(T scale)
	{
		x = scale;
		y = scale;
		z = scale;
		w = scale;

		return *this;
	}

	/*!
	* \brief Sets the components of the vector from an array of four elements
	* \return A reference to this vector
	*
	* \param vec[4] vec[0] is X component, vec[1] is Y component, vec[2] is Z component and vec[3] is W component
	*/

	template<typename T>
	Vector4<T>& Vector4<T>::Set(const T vec[4])
	{
		std::memcpy(&x, vec, 4*sizeof(T));

		return *this;
	}

	/*!
	* \brief Sets the components of the vector from a Vector2 and two components
	*
	* \param vec vec.X = X component and vec.y = Y component
	* \param Z Z component
	* \param W W component
	*/

	template<typename T>
	Vector4<T>& Vector4<T>::Set(const Vector2<T>& vec, T Z, T W)
	{
		x = vec.x;
		y = vec.y;
		z = Z;
		w = W;

		return *this;
	}

	/*!
	* \brief Sets the components of the vector from a Vector3 and one components
	*
	* \param vec vec.X = X component, vec.y = Y component and vec.z = Z component
	* \param W W component
	*/

	template<typename T>
	Vector4<T>& Vector4<T>::Set(const Vector3<T>& vec, T W)
	{
		x = vec.x;
		y = vec.y;
		z = vec.z;
		w = W;

		return *this;
	}

	/*!
	* \brief Sets the components of the vector from another vector
	* \return A reference to this vector
	*
	* \param vec The other vector
	*/

	template<typename T>
	Vector4<T>& Vector4<T>::Set(const Vector4& vec)
	{
		std::memcpy(this, &vec, sizeof(Vector4));

		return *this;
	}

	/*!
	* \brief Sets the components of the vector from another type of Vector4
	* \return A reference to this vector
	*
	* \param vec Vector of type U to convert its components
	*/

	template<typename T>
	template<typename U>
	Vector4<T>& Vector4<T>::Set(const Vector4<U>& vec)
	{
		x = F(vec.x);
		y = F(vec.y);
		z = F(vec.z);
		w = F(vec.w);

		return *this;
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object: "Vector4(x, y, z, w)"
	*/

	template<typename T>
	String Vector4<T>::ToString() const
	{
		StringStream ss;

		return ss << "Vector4(" << x << ", " << y << ", " << z << ", " << w << ')';
	}

	/*!
	* \brief Converts vector to pointer to its own data
	* \return A pointer to the own data
	*
	* \remark Access to index greather than 3 is undefined behavior
	*/

	template<typename T>
	Vector4<T>::operator T* ()
	{
		return &x;
	}

	/*!
	* \brief Converts vector to const pointer to its own data
	* \return A constant pointer to the own data
	*
	* \remark Access to index greather than 3 is undefined behavior
	*/

	template<typename T>
	Vector4<T>::operator const T* () const
	{
		return &x;
	}

	/*!
	* \brief Helps to represent the sign of the vector
	* \return A constant reference to this vector
	*/

	template<typename T>
	const Vector4<T>& Vector4<T>::operator+() const
	{
		return *this;
	}

	/*!
	* \brief Negates the components of the vector
	* \return A constant reference to this vector with negate components
	*/

	template<typename T>
	Vector4<T> Vector4<T>::operator-() const
	{
		return Vector4(-x, -y, -z, -w);
	}

	/*!
	* \brief Adds the components of the vector with other vector
	* \return A vector where components are the sum of this vector and the other one
	*
	* \param vec The other vector to add components with
	*/

	template<typename T>
	Vector4<T> Vector4<T>::operator+(const Vector4& vec) const
	{
		return Vector4(x + vec.x, y + vec.y, z + vec.z, w + vec.w);
	}

	/*!
	* \brief Substracts the components of the vector with other vector
	* \return A vector where components are the difference of this vector and the other one
	*
	* \param vec The other vector to substract components with
	*/

	template<typename T>
	Vector4<T> Vector4<T>::operator-(const Vector4& vec) const
	{
		return Vector4(x - vec.x, y - vec.y, z - vec.z, w - vec.w);
	}

	/*!
	* \brief Multiplies the components of the vector with other vector
	* \return A vector where components are the product of this vector and the other one
	*
	* \param vec The other vector to multiply components with
	*/

	template<typename T>
	Vector4<T> Vector4<T>::operator*(const Vector4& vec) const
	{
		return Vector4(x * vec.x, y * vec.y, z * vec.z, w * vec.w);
	}

	/*!
	* \brief Multiplies the components of the vector with a scalar
	* \return A vector where components are the product of this vector and the scalar
	*
	* \param scale The scalar to multiply components with
	*/

	template<typename T>
	Vector4<T> Vector4<T>::operator*(T scale) const
	{
		return Vector4(x * scale, y * scale, z * scale, w * scale);
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
	Vector4<T> Vector4<T>::operator/(const Vector4& vec) const
	{
		#if NAZARA_MATH_SAFE
		if (NumberEquals(vec.x, F(0.0)) || NumberEquals(vec.y, F(0.0)) || NumberEquals(vec.z, F(0.0)) || NumberEquals(vec.w, F(0.0)))
		{
			String error("Division by zero");

			NazaraError(error);
			throw std::domain_error(error.ToStdString());
		}
		#endif

		return Vector4(x / vec.x, y / vec.y, z / vec.z, w / vec.w);
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
	Vector4<T> Vector4<T>::operator/(T scale) const
	{
		#if NAZARA_MATH_SAFE
		if (NumberEquals(scale, F(0.0)))
		{
			String error("Division by zero");

			NazaraError(error);
			throw std::domain_error(error.ToStdString());
		}
		#endif

		return Vector4(x / scale, y / scale, z / scale, w / scale);
	}

	/*!
	* \brief Adds the components of other vector to this vector
	* \return A reference to this vector where components are the sum of this vector and the other one
	*
	* \param vec The other vector to add components with
	*/

	template<typename T>
	Vector4<T>& Vector4<T>::operator+=(const Vector4& vec)
	{
		x += vec.x;
		y += vec.y;
		z += vec.z;
		w += vec.w;

		return *this;
	}

	/*!
	* \brief Substracts the components of other vector to this vector
	* \return A reference to this vector where components are the difference of this vector and the other one
	*
	* \param vec The other vector to substract components with
	*/

	template<typename T>
	Vector4<T>& Vector4<T>::operator-=(const Vector4& vec)
	{
		x -= vec.x;
		y -= vec.y;
		z -= vec.z;
		w -= vec.w;

		return *this;
	}

	/*!
	* \brief Multiplies the components of other vector to this vector
	* \return A reference to this vector where components are the product of this vector and the other one
	*
	* \param vec The other vector to multiply components with
	*/

	template<typename T>
	Vector4<T>& Vector4<T>::operator*=(const Vector4& vec)
	{
		x *= vec.x;
		y *= vec.y;
		z *= vec.z;
		w *= vec.w;

		return *this;
	}

	/*!
	* \brief Multiplies the components of other vector with a scalar
	* \return A reference to this vector where components are the product of this vector and the scalar
	*
	* \param scale The scalar to multiply components with
	*/

	template<typename T>
	Vector4<T>& Vector4<T>::operator*=(T scale)
	{
		x *= scale;
		y *= scale;
		z *= scale;
		w *= scale;

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
	Vector4<T>& Vector4<T>::operator/=(const Vector4& vec)
	{
		#if NAZARA_MATH_SAFE
		if (NumberEquals(vec.x, F(0.0)) || NumberEquals(vec.y, F(0.0)) || NumberEquals(vec.z, F(0.0)) || NumberEquals(vec.w, F(0.0)))
		{
			String error("Division by zero");

			NazaraError(error);
			throw std::domain_error(error.ToStdString());
		}
		#endif

		x /= vec.x;
		y /= vec.y;
		z /= vec.z;
		w /= vec.w;

		return *this;
	}

	/*!
	* \brief Divides the components of other vector with a scalar
	* \return A reference to this vector where components are the quotient of this vector and the scalar
	*
	* \param scale The scalar to divide components with
	*
	* \remark Produce a NazaraError if scale is null with NAZARA_MATH_SAFE defined
	* \throw std::domain_error if NAZARA_MATH_SAFE is defined and scale is null
	*/

	template<typename T>
	Vector4<T>& Vector4<T>::operator/=(T scale)
	{
		#if NAZARA_MATH_SAFE
		if (NumberEquals(scale, F(0.0)))
		{
			String error("Division by zero");

			NazaraError(error);
			throw std::domain_error(error.ToStdString());
		}
		#endif

		x /= scale;
		y /= scale;
		z /= scale;
		w /= scale;

		return *this;
	}

	/*!
	* \brief Compares the vector to other one
	* \return true if the vectors are the same
	*
	* \param vec Other vector to compare with
	*/

	template<typename T>
	bool Vector4<T>::operator==(const Vector4& vec) const
	{
		return NumberEquals(x, vec.x) &&
		       NumberEquals(y, vec.y) &&
		       NumberEquals(z, vec.z) &&
		       NumberEquals(w, vec.w);
	}

	/*!
	* \brief Compares the vector to other one
	* \return false if the vectors are the same
	*
	* \param vec Other vector to compare with
	*/

	template<typename T>
	bool Vector4<T>::operator!=(const Vector4& vec) const
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
	bool Vector4<T>::operator<(const Vector4& vec) const
	{
		if (NumberEquals(x, vec.x))
		{
			if (NumberEquals(y, vec.y))
			{
				if (NumberEquals(z, vec.z))
					return w < vec.w;
				else
					return z < vec.z;
			}
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
	bool Vector4<T>::operator<=(const Vector4& vec) const
	{
		if (NumberEquals(x, vec.x))
		{
			if (NumberEquals(y, vec.y))
			{
				if (NumberEquals(z, vec.z))
					return NumberEquals(w, vec.w) || w < vec.w;
				else
					return z < vec.z;
			}
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
	bool Vector4<T>::operator>(const Vector4& vec) const
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
	bool Vector4<T>::operator>=(const Vector4& vec) const
	{
		return !operator<(vec);
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
	Vector4<T> Vector4<T>::Lerp(const Vector4& from, const Vector4& to, T interpolation)
	{
		Vector4 dummy;
		dummy.x = Nz::Lerp(from.x, to.x, interpolation);
		dummy.y = Nz::Lerp(from.y, to.y, interpolation);
		dummy.z = Nz::Lerp(from.z, to.z, interpolation);
		dummy.w = Nz::Lerp(from.w, to.w, interpolation);

		return dummy;
	}

	/*!
	* \brief Gives the normalized vector
	* \return A normalized vector from the vec with w = 1
	*
	* \param vec Vector to normalize
	*
	* \see GetNormal
	*/

	template<typename T>
	Vector4<T> Vector4<T>::Normalize(const Vector4& vec)
	{
		return vec.GetNormal();
	}

	/*!
	* \brief Shorthand for the vector (1, 0, 0, 1)
	* \return A vector with components (1, 0, 0, 1)
	*
	* \see MakeUnitX
	*/

	template<typename T>
	Vector4<T> Vector4<T>::UnitX()
	{
		Vector4 vector;
		vector.MakeUnitX();

		return vector;
	}

	/*!
	* \brief Shorthand for the vector (0, 1, 0, 1)
	* \return A vector with components (0, 1, 0, 1)
	*
	* \see MakeUnitY
	*/

	template<typename T>
	Vector4<T> Vector4<T>::UnitY()
	{
		Vector4 vector;
		vector.MakeUnitY();

		return vector;
	}

	/*!
	* \brief Shorthand for the vector (0, 0, 1, 1)
	* \return A vector with components (0, 0, 1, 1)
	*
	* \see MakeUnitZ
	*/

	template<typename T>
	Vector4<T> Vector4<T>::UnitZ()
	{
		Vector4 vector;
		vector.MakeUnitZ();

		return vector;
	}

	/*!
	* \brief Shorthand for the vector (0, 0, 0, 1)
	* \return A vector with components (0, 0, 0, 1)
	*
	* \see MakeZero
	*/

	template<typename T>
	Vector4<T> Vector4<T>::Zero()
	{
		Vector4 vector;
		vector.MakeZero();

		return vector;
	}

	/*!
	* \brief Serializes a Vector4
	* \return true if successfully serialized
	*
	* \param context Serialization context
	* \param vector Input Vector3
	*/
	template<typename T>
	bool Serialize(SerializationContext& context, const Vector4<T>& vector)
	{
		if (!Serialize(context, vector.x))
			return false;

		if (!Serialize(context, vector.y))
			return false;

		if (!Serialize(context, vector.z))
			return false;

		if (!Serialize(context, vector.w))
			return false;

		return true;
	}

	/*!
	* \brief Unserializes a Vector4
	* \return true if successfully unserialized
	*
	* \param context Serialization context
	* \param vector Output Vector3
	*/
	template<typename T>
	bool Unserialize(SerializationContext& context, Vector4<T>* vector)
	{
		if (!Unserialize(context, &vector->x))
			return false;

		if (!Unserialize(context, &vector->y))
			return false;

		if (!Unserialize(context, &vector->z))
			return false;

		if (!Unserialize(context, &vector->w))
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
std::ostream& operator<<(std::ostream& out, const Nz::Vector4<T>& vec)
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
Nz::Vector4<T> operator*(T scale, const Nz::Vector4<T>& vec)
{
	return Nz::Vector4<T>(scale * vec.x, scale * vec.y, scale * vec.z, scale * vec.w);
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
Nz::Vector4<T> operator/(T scale, const Nz::Vector4<T>& vec)
{
	#if NAZARA_MATH_SAFE
	if (NumberEquals(vec.x, F(0.0)) || NumberEquals(vec.y, F(0.0)) || NumberEquals(vec.z, F(0.0)) || NumberEquals(vec.w, F(0.0)))
	{
		Nz::String error("Division by zero");

		NazaraError(error);
		throw std::domain_error(error.ToStdString());
	}
	#endif

	return Nz::Vector4<T>(scale / vec.x, scale / vec.y, scale / vec.z, scale / vec.w);
}

namespace std
{
	template<class T>
	struct hash<Nz::Vector4<T>>
	{
		/*!
		* \brief Specialisation of std to hash
		* \return Result of the hash
		*
		* \param v Vector4 to hash
		*/
		std::size_t operator()(const Nz::Vector4<T>& v) const
		{
			std::size_t seed {};

			Nz::HashCombine(seed, v.x);
			Nz::HashCombine(seed, v.y);
			Nz::HashCombine(seed, v.z);
			Nz::HashCombine(seed, v.w);

			return seed;
		}
	};
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
