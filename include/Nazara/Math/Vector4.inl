// Copyright (C) 2024 Rémi Bèges - Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Error.hpp>
#include <NazaraUtils/Algorithm.hpp>
#include <NazaraUtils/Hash.hpp>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <Nazara/Core/Debug.hpp>

///FIXME: Les calculs effectués ici sont probablements tous faux, la composante W étant spéciale dans le monde de la 3D

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
	constexpr Vector4<T>::Vector4(T X, T Y, T Z, T W) :
	x(X),
	y(Y),
	z(Z),
	w(W)
	{
	}

	/*!
	* \brief Constructs a Vector4 object from two components and a Vector2<T>
	*
	* \param X X component
	* \param Y Y component
	* \param vec vec.X = Z component and vec.y = W component
	*/
	template<typename T>
	constexpr Vector4<T>::Vector4(T X, T Y, const Vector2<T>& vec) :
	x(X),
	y(Y),
	z(vec.x),
	w(vec.y)
	{
	}

	/*!
	* \brief Constructs a Vector4 object from one component, a Vector2<T> and one component
	*
	* \param X X component
	* \param vec vec.X = Y component and vec.y = Z component
	* \param W W component
	*/
	template<typename T>
	constexpr Vector4<T>::Vector4(T X, const Vector2<T>& vec, T W) :
	x(X),
	y(vec.x),
	z(vec.y),
	w(W)
	{
	}

	/*!
	* \brief Constructs a Vector4 object from one component and a Vector3<T>
	*
	* \param X X component
	* \param vec vec.X = Y component, vec.y = Z component and vec.z = W component
	*/
	template<typename T>
	constexpr Vector4<T>::Vector4(T X, const Vector3<T>& vec) :
	x(X),
	y(vec.x),
	z(vec.y),
	w(vec.z)
	{
	}

	/*!
	* \brief Constructs explicitely a Vector4 object from its "scale"
	*
	* \param scale X component = Y component = Z component = W component
	*/
	template<typename T>
	constexpr Vector4<T>::Vector4(T scale) :
	x(scale),
	y(scale),
	z(scale),
	w(scale)
	{
	}

	/*!
	* \brief Constructs a Vector4 object from a Vector2<T> and two components
	*
	* \param vec vec.X = X component and vec.y = Y component
	* \param Z Z component
	* \param W W component
	*/
	template<typename T>
	constexpr Vector4<T>::Vector4(const Vector2<T>& vec, T Z, T W) :
	x(vec.x),
	y(vec.y),
	z(Z),
	w(W)
	{
	}

	/*!
	* \brief Constructs a Vector4 object from one component and a Vector3<T>
	*
	* \param vec vec.X = X component, vec.y = Y component and vec.z = Z component
	* \param W W component
	*/
	template<typename T>
	constexpr Vector4<T>::Vector4(const Vector3<T>& vec, T W) :
	x(vec.x),
	y(vec.y),
	z(vec.z),
	w(W)
	{
	}

	/*!
	* \brief Constructs a Vector4 object from another type of Vector4
	*
	* \param vec Vector of type U to convert to type T
	*/

	template<typename T>
	template<typename U>
	constexpr Vector4<T>::Vector4(const Vector4<U>& vec) :
	x(static_cast<T>(vec.x)),
	y(static_cast<T>(vec.y)),
	z(static_cast<T>(vec.z)),
	w(static_cast<T>(vec.w))
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
	T Vector4<T>::AbsDotProduct(const Vector4& vec) const
	{
		return std::abs(x * vec.x) + std::abs(y * vec.y) + std::abs(z * vec.z) + std::abs(w * vec.w);
	}

	template<typename T>
	constexpr bool Vector4<T>::ApproxEqual(const Vector4& vec, T maxDifference) const
	{
		return NumberEquals(x, vec.x, maxDifference) && NumberEquals(y, vec.y, maxDifference) && NumberEquals(z, vec.z, maxDifference) && NumberEquals(w, vec.w, maxDifference);
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
	constexpr T Vector4<T>::DotProduct(const Vector4& vec) const
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
	* \brief Sets this vector's components to the maximum of its own and other components
	* \return A reference to this vector with replaced values with the corresponding max value
	*
	* \param vec Other vector to compare the components with
	*
	* \see Minimize
	*/
	template<typename T>
	constexpr Vector4<T>& Vector4<T>::Maximize(const Vector4& vec)
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
	constexpr Vector4<T>& Vector4<T>::Minimize(const Vector4& vec)
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
		T invLength = T(1.0) / w;
		x *= invLength;
		y *= invLength;
		z *= invLength;

		if (length)
			*length = w;

		w = T(1.0);

		return *this;
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object: "Vector4(x, y, z, w)"
	*/
	template<typename T>
	std::string Vector4<T>::ToString() const
	{
		std::ostringstream ss;
		ss << *this;

		return ss.str();
	}

	/*!
	* \brief Access a vector component by index
	* \return X, Y, Z depending on index (0, 1, 2)
	*/
	template<typename T>
	constexpr T& Vector4<T>::operator[](std::size_t i)
	{
		NazaraAssert(i < 4, "index out of range");
		return *(&x + i);
	}

	/*!
	* \brief Access a vector component by index
	* \return X, Y, Z depending on index (0, 1, 2)
	*/
	template<typename T>
	constexpr const T& Vector4<T>::operator[](std::size_t i) const
	{
		NazaraAssert(i < 4, "index out of range");
		return *(&x + i);
	}

	/*!
	* \brief Helps to represent the sign of the vector
	* \return A constant reference to this vector
	*/
	template<typename T>
	constexpr const Vector4<T>& Vector4<T>::operator+() const
	{
		return *this;
	}

	/*!
	* \brief Negates the components of the vector
	* \return A constant reference to this vector with negate components
	*/
	template<typename T>
	constexpr Vector4<T> Vector4<T>::operator-() const
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
	constexpr Vector4<T> Vector4<T>::operator+(const Vector4& vec) const
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
	constexpr Vector4<T> Vector4<T>::operator-(const Vector4& vec) const
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
	constexpr Vector4<T> Vector4<T>::operator*(const Vector4& vec) const
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
	constexpr Vector4<T> Vector4<T>::operator*(T scale) const
	{
		return Vector4(x * scale, y * scale, z * scale, w * scale);
	}

	/*!
	* \brief Divides the components of the vector with other vector
	* \return A vector where components are the quotient of this vector and the other one
	*
	* \param vec The other vector to divide components with
	*/
	template<typename T>
	constexpr Vector4<T> Vector4<T>::operator/(const Vector4& vec) const
	{
		return Vector4(x / vec.x, y / vec.y, z / vec.z, w / vec.w);
	}

	/*!
	* \brief Divides the components of the vector with a scalar
	* \return A vector where components are the quotient of this vector and the scalar
	*
	* \param scale The scalar to divide components with
	*/
	template<typename T>
	constexpr Vector4<T> Vector4<T>::operator/(T scale) const
	{
		return Vector4(x / scale, y / scale, z / scale, w / scale);
	}

	template<typename T>
	constexpr Vector4<T> Vector4<T>::operator%(const Vector4& vec) const
	{
		return Vector4(Mod(x, vec.x), Mod(y, vec.y), Mod(z, vec.z), Mod(w, vec.w));
	}

	template<typename T>
	constexpr Vector4<T> Vector4<T>::operator%(T mod) const
	{
		return Vector4(Mod(x, mod), Mod(y, mod), Mod(z, mod), Mod(z, mod));
	}

	/*!
	* \brief Adds the components of other vector to this vector
	* \return A reference to this vector where components are the sum of this vector and the other one
	*
	* \param vec The other vector to add components with
	*/
	template<typename T>
	constexpr Vector4<T>& Vector4<T>::operator+=(const Vector4& vec)
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
	constexpr Vector4<T>& Vector4<T>::operator-=(const Vector4& vec)
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
	constexpr Vector4<T>& Vector4<T>::operator*=(const Vector4& vec)
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
	constexpr Vector4<T>& Vector4<T>::operator*=(T scale)
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
	*/
	template<typename T>
	constexpr Vector4<T>& Vector4<T>::operator/=(const Vector4& vec)
	{
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
	*/
	template<typename T>
	constexpr Vector4<T>& Vector4<T>::operator/=(T scale)
	{
		x /= scale;
		y /= scale;
		z /= scale;
		w /= scale;

		return *this;
	}

	template<typename T>
	constexpr Vector4<T>& Vector4<T>::operator%=(const Vector4& vec)
	{
		x = Mod(x, vec.x);
		y = Mod(y, vec.y);
		z = Mod(z, vec.z);
		w = Mod(w, vec.w);

		return *this;
	}

	template<typename T>
	constexpr Vector4<T>& Vector4<T>::operator%=(T mod)
	{
		x = Mod(x, mod);
		y = Mod(y, mod);
		z = Mod(z, mod);
		w = Mod(w, mod);

		return *this;
	}

	/*!
	* \brief Compares the vector to other one
	* \return true if the vectors are the same
	*
	* \param vec Other vector to compare with
	*/
	template<typename T>
	constexpr bool Vector4<T>::operator==(const Vector4& vec) const
	{
		return x == vec.x && y == vec.y && z == vec.z && w == vec.w;
	}

	/*!
	* \brief Compares the vector to other one
	* \return false if the vectors are the same
	*
	* \param vec Other vector to compare with
	*/
	template<typename T>
	constexpr bool Vector4<T>::operator!=(const Vector4& vec) const
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
	constexpr bool Vector4<T>::operator<(const Vector4& vec) const
	{
		if (x != vec.x)
			return x < vec.x;

		if (y != vec.y)
			return y < vec.y;

		if (z != vec.z)
			return z < vec.z;

		return w < vec.w;
	}

	/*!
	* \brief Compares the vector to other one
	* \return true if this vector has its first components inferior or equal to the other ones
	*
	* \param vec Other vector to compare with
	*/
	template<typename T>
	constexpr bool Vector4<T>::operator<=(const Vector4& vec) const
	{
		if (x != vec.x)
			return x < vec.x;

		if (y != vec.y)
			return y < vec.y;

		if (z != vec.z)
			return z < vec.z;

		return w <= vec.w;
	}

	/*!
	* \brief Compares the vector to other one
	* \return true if this vector has its first components superior to the other ones
	*
	* \param vec Other vector to compare with
	*/
	template<typename T>
	constexpr bool Vector4<T>::operator>(const Vector4& vec) const
	{
		if (x != vec.x)
			return x > vec.x;

		if (y != vec.y)
			return y > vec.y;

		if (z != vec.z)
			return z > vec.z;

		return w > vec.w;
	}

	/*!
	* \brief Compares the vector to other one
	* \return true if this vector has its first components superior or equal to the other ones
	*
	* \param vec Other vector to compare with
	*/
	template<typename T>
	constexpr bool Vector4<T>::operator>=(const Vector4& vec) const
	{
		if (x != vec.x)
			return x > vec.x;

		if (y != vec.y)
			return y > vec.y;

		if (z != vec.z)
			return z > vec.z;

		return w >= vec.w;
	}

	template<typename T>
	constexpr Vector4<T> Vector4<T>::Apply(T(*func)(T), const Vector4& vec)
	{
		return Vector4(func(vec.x), func(vec.y), func(vec.z), func(vec.w));
	}

	template<typename T>
	constexpr bool Vector4<T>::ApproxEqual(const Vector4& lhs, const Vector4& rhs, T maxDifference)
	{
		return lhs.ApproxEqual(rhs, maxDifference);
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
	constexpr T Vector4<T>::DotProduct(const Vector4& vec1, const Vector4& vec2)
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
	constexpr Vector4<T> Vector4<T>::Lerp(const Vector4& from, const Vector4& to, T interpolation)
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
	*/
	template<typename T>
	constexpr Vector4<T> Vector4<T>::UnitX()
	{
		return Vector4(1, 0, 0, 1);
	}

	/*!
	* \brief Shorthand for the vector (0, 1, 0, 1)
	* \return A vector with components (0, 1, 0, 1)
	*/
	template<typename T>
	constexpr Vector4<T> Vector4<T>::UnitY()
	{
		return Vector4(0, 1, 0, 1);
	}

	/*!
	* \brief Shorthand for the vector (0, 0, 1, 1)
	* \return A vector with components (0, 0, 1, 1)
	*/
	template<typename T>
	constexpr Vector4<T> Vector4<T>::UnitZ()
	{
		return Vector4(0, 0, 1, 1);
	}

	/*!
	* \brief Shorthand for the vector (0, 0, 0, 1)
	* \return A vector with components (0, 0, 0, 1)
	*/
	template<typename T>
	constexpr Vector4<T> Vector4<T>::Zero()
	{
		return Vector4(0, 0, 0, 1);
	}

	/*!
	* \brief Serializes a Vector4
	* \return true if successfully serialized
	*
	* \param context Serialization context
	* \param vector Input Vector3
	*/
	template<typename T>
	bool Serialize(SerializationContext& context, const Vector4<T>& vector, TypeTag<Vector4<T>>)
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
	bool Unserialize(SerializationContext& context, Vector4<T>* vector, TypeTag<Vector4<T>>)
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

	/*!
	* \brief Output operator
	* \return The stream
	*
	* \param out The stream
	* \param vec The vector to output
	*/
	template<typename T>
	std::ostream& operator<<(std::ostream& out, const Vector4<T>& vec)
	{
		return out << "Vector4(" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << ')';
	}

	/*!
	* \brief Multiplies the components of the vector with a scalar
	* \return A vector where components are the product of this vector and the scalar
	*
	* \param scale The scalar to multiply components with
	*/
	template<typename T>
	constexpr Vector4<T> operator*(T scale, const Vector4<T>& vec)
	{
		return Vector4<T>(scale * vec.x, scale * vec.y, scale * vec.z, scale * vec.w);
	}

	/*!
	* \brief Divides the components of the vector with a scalar
	* \return A vector where components are the quotient of this vector and the scalar
	*
	* \param scale The scalar to divide components with
	*/
	template<typename T>
	constexpr Vector4<T> operator/(T scale, const Vector4<T>& vec)
	{
		return Vector4<T>(scale / vec.x, scale / vec.y, scale / vec.z, scale / vec.w);
	}

	template<typename T>
	constexpr Vector4<T> operator%(T mod, const Vector4<T>& vec)
	{
		return Vector4<T>(Mod(mod, vec.x), Mod(mod, vec.y), Mod(mod, vec.z), Mod(mod, vec.w));
	}
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
			return Nz::HashCombine(v.x, v.y, v.z, v.w);
		}
	};
}

#include <Nazara/Core/DebugOff.hpp>
