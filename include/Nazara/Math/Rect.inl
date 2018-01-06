// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <algorithm>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)

namespace Nz
{
	/*!
	* \ingroup math
	* \class Nz::Rect
	* \brief Math class that represents an axis-aligned rectangle in two dimensions
	*
	* \remark The basis is said to be "left-hand". It means that with your left hand, the thumb is X positive, the index finger Y positive pointing to the bottom
	*/

	/*!
	* \brief Constructs a Rect object from its width and height
	*
	* \param Width Width of the rectangle (following X)
	* \param Height Height of the rectangle (following Y)
	*
	* \remark Position will be (0, 0)
	*/

	template<typename T>
	Rect<T>::Rect(T Width, T Height)
	{
		Set(Width, Height);
	}

	/*!
	* \brief Constructs a Rect object from its position, width and height
	*
	* \param X X position
	* \param Y Y position
	* \param Width Width of the rectangle (following X)
	* \param Height Height of the rectangle (following Y)
	*/

	template<typename T>
	Rect<T>::Rect(T X, T Y, T Width, T Height)
	{
		Set(X, Y, Width, Height);
	}

	/*!
	* \brief Constructs a Rect object from an array of four elements
	*
	* \param vec[4] vec[0] is X position, vec[1] is Y position, vec[2] is width and vec[3] is height
	*/

	template<typename T>
	Rect<T>::Rect(const T vec[4])
	{
		Set(vec);
	}

	/*!
	* \brief Constructs a Rect object from a vector representing width and height
	*
	* \param lengths (Width, Height) of the rectangle
	*
	* \remark X and Y will be (0, 0)
	*/

	template<typename T>
	Rect<T>::Rect(const Vector2<T>& lengths)
	{
		Set(lengths);
	}

	/*!
	* \brief Constructs a Rect object from two vectors representing point of the space
	* (X, Y) will be the components minimum of the two vectors and the width and height will be the components maximum - minimum
	*
	* \param vec1 First point
	* \param vec2 Second point
	*/

	template<typename T>
	Rect<T>::Rect(const Vector2<T>& vec1, const Vector2<T>& vec2)
	{
		Set(vec1, vec2);
	}

	/*!
	* \brief Constructs a Rect object from another type of Rect
	*
	* \param rect Rect of type U to convert to type T
	*/

	template<typename T>
	template<typename U>
	Rect<T>::Rect(const Rect<U>& rect)
	{
		Set(rect);
	}

	/*!
	* \brief Tests whether the rectangle contains the provided point inclusive of the edge of the rectangle
	* \return true if inclusive
	*
	* \param X X position of the point
	* \param Y Y position of the point
	*
	* \see Contains
	*/

	template<typename T>
	bool Rect<T>::Contains(T X, T Y) const
	{
		return X >= x && X < (x + width) &&
		       Y >= y && Y < (y + height);
	}

	/*!
	* \brief Tests whether the rectangle contains the provided rectangle inclusive of the edge of the rectangle
	* \return true if inclusive
	*
	* \param rect Other rectangle to test
	*
	* \see Contains
	*/

	template<typename T>
	bool Rect<T>::Contains(const Rect<T>& rect) const
	{
		return Contains(rect.x, rect.y) &&
		       Contains(rect.x + rect.width, rect.y + rect.height);
	}

	/*!
	* \brief Tests whether the rectangle contains the provided point inclusive of the edge of the rectangle
	* \return true if inclusive
	*
	* \param point Position of the point
	*
	* \see Contains
	*/

	template<typename T>
	bool Rect<T>::Contains(const Vector2<T>& point) const
	{
		return Contains(point.x, point.y);
	}

	/*!
	* \brief Extends the rectangle to contain the point in the boundary
	* \return A reference to this rectangle extended
	*
	* \param X X position of the point
	* \param Y Y position of the point
	*
	* \see ExtendTo
	*/

	template<typename T>
	Rect<T>& Rect<T>::ExtendTo(T X, T Y)
	{
		width = std::max(x + width, X);
		height = std::max(y + height, Y);

		x = std::min(x, X);
		y = std::min(y, Y);

		width -= x;
		height -= y;

		return *this;
	}

	/*!
	* \brief Extends the rectangle to contain the rectangle
	* \return A reference to this rectangle extended
	*
	* \param rect Other rectangle to contain
	*
	* \see ExtendTo
	*/

	template<typename T>
	Rect<T>& Rect<T>::ExtendTo(const Rect& rect)
	{
		width = std::max(x + width, rect.x + rect.width);
		height = std::max(y + height, rect.y + rect.height);

		x = std::min(x, rect.x);
		y = std::min(y, rect.y);

		width -= x;
		height -= y;

		return *this;
	}

	/*!
	* \brief Extends the rectangle to contain the point in the boundary
	* \return A reference to this rectangle extended
	*
	* \param point Position of the point
	*
	* \see ExtendTo
	*/

	template<typename T>
	Rect<T>& Rect<T>::ExtendTo(const Vector2<T>& point)
	{
		return ExtendTo(point.x, point.y);
	}

	/*!
	* \brief Gets a Vector2 for the center
	* \return The position of the center of the rectangle
	*/

	template<typename T>
	Vector2<T> Rect<T>::GetCenter() const
	{
		return GetPosition() + GetLengths() / F(2.0);
	}

	/*!
	* \brief Gets the Vector2 for the corner
	* \return The position of the corner of the rectangle according to enum RectCorner
	*
	* \param corner Enumeration of type RectCorner
	*
	* \remark If enumeration is not defined in RectCorner, a NazaraError is thrown and a Vector2 uninitialised is returned
	*/

	template<typename T>
	Vector2<T> Rect<T>::GetCorner(RectCorner corner) const
	{
		switch (corner)
		{
			case RectCorner_LeftBottom:
				return Vector2<T>(x, y + height);

			case RectCorner_LeftTop:
				return Vector2<T>(x, y);

			case RectCorner_RightBottom:
				return Vector2<T>(x + width, y + height);

			case RectCorner_RightTop:
				return Vector2<T>(x + width, y);
		}

		NazaraError("Corner not handled (0x" + String::Number(corner, 16) + ')');
		return Vector2<T>();
	}

	/*!
	* \brief Gets a Vector2 for the lengths
	* \return The lengths of the rectangle (width, height)
	*/

	template<typename T>
	Vector2<T> Rect<T>::GetLengths() const
	{
		return Vector2<T>(width, height);
	}

	/*!
	* \brief Gets a Vector2 for the maximum point
	* \return The RectCorner_RightBottom of the rectangle
	*
	* \see GetCorner
	*/

	template<typename T>
	Vector2<T> Rect<T>::GetMaximum() const
	{
		return GetPosition() + GetLengths();
	}

	/*!
	* \brief Gets a Vector2 for the minimum point
	* \return The RectCorner_LeftTop of the rectangle
	*
	* \see GetCorner, GetPosition
	*/

	template<typename T>
	Vector2<T> Rect<T>::GetMinimum() const
	{
		return GetPosition();
	}

	/*!
	* \brief Computes the negative vertex of one direction
	* \return The position of the vertex on the rectangle in the opposite way of the normal while considering the center. It means that if the normal has one component negative, the component is set to width or height corresponding to the sign
	*
	* \param normal Vector indicating a direction
	*
	* \see GetPositiveVertex
	*/

	template<typename T>
	Vector2<T> Rect<T>::GetNegativeVertex(const Vector2<T>& normal) const
	{
		Vector2<T> neg(GetPosition());

		if (normal.x < F(0.0))
			neg.x += width;

		if (normal.y < F(0.0))
			neg.y += height;

		return neg;
	}

	/*!
	* \brief Gets a Vector2 for the position
	* \return The RectCorner_LeftTop of the rectangle
	*
	* \see GetCorner, GetMinimum
	*/

	template<typename T>
	Vector2<T> Rect<T>::GetPosition() const
	{
		return Vector2<T>(x, y);
	}

	/*!
	* \brief Computes the positive vertex of one direction
	* \return The position of the vertex on the rectangle in the same way of the normal while considering the center. It means that if the normal has one component positive, the component is set to width or height corresponding to the sign
	*
	* \param normal Vector indicating a direction
	*
	* \see GetNegativeVertex
	*/

	template<typename T>
	Vector2<T> Rect<T>::GetPositiveVertex(const Vector2<T>& normal) const
	{
		Vector2<T> pos(GetPosition());

		if (normal.x > F(0.0))
			pos.x += width;

		if (normal.y > F(0.0))
			pos.y += height;

		return pos;
	}

	/*!
	* \brief Checks whether or not this rectangle intersects another one
	* \return true if the rectangle intersects
	*
	* \param rect Rectangle to check
	* \param intersection Optional argument for the rectangle which represent the intersection
	*/

	template<typename T>
	bool Rect<T>::Intersect(const Rect& rect, Rect* intersection) const
	{
		T left = std::max(x, rect.x);
		T right = std::min(x + width, rect.x + rect.width);
		if (left >= right)
			return false;

		T top = std::max(y, rect.y);
		T bottom = std::min(y + height, rect.y + rect.height);
		if (top >= bottom)
			return false;

		if (intersection)
		{
			intersection->x = left;
			intersection->y = top;
			intersection->width = right - left;
			intersection->height = bottom - top;
		}

		return true;
	}

	/*!
	* \brief Checks whether this rectangle is valid
	* \return true if the rectangle has a strictly positive width and height
	*/

	template<typename T>
	bool Rect<T>::IsValid() const
	{
		return width > F(0.0) && height > F(0.0);
	}

	/*!
	* \brief Makes the rectangle position (0, 0) and lengths (0, 0)
	* \return A reference to this box with position (0, 0) and lengths (0, 0)
	*
	* \see Zero
	*/

	template<typename T>
	Rect<T>& Rect<T>::MakeZero()
	{
		x = F(0.0);
		y = F(0.0);
		width = F(0.0);
		height = F(0.0);

		return *this;
	}

	/*!
	* \brief Sets the components of the rectangle with width and height
	* \return A reference to this rectangle
	*
	* \param Width Width of the rectangle (following X)
	* \param Height Height of the rectangle (following Y)
	*
	* \remark Position will be (0, 0)
	*/

	template<typename T>
	Rect<T>& Rect<T>::Set(T Width, T Height)
	{
		x = F(0.0);
		y = F(0.0);
		width = Width;
		height = Height;

		return *this;
	}

	/*!
	* \brief Sets the components of the rectangle
	* \return A reference to this rectangle
	*
	* \param X X position
	* \param Y Y position
	* \param Width Width of the rectangle (following X)
	* \param Height Height of the rectangle (following Y)
	*/

	template<typename T>
	Rect<T>& Rect<T>::Set(T X, T Y, T Width, T Height)
	{
		x = X;
		y = Y;
		width = Width;
		height = Height;

		return *this;
	}

	/*!
	* \brief Sets the components of the rectangle from an array of four elements
	* \return A reference to this rectangle
	*
	* \param rect[4] rect[0] is X position, rect[1] is Y position, rect[2] is width and rect[3] is height
	*/

	template<typename T>
	Rect<T>& Rect<T>::Set(const T rect[4])
	{
		x = rect[0];
		y = rect[1];
		width = rect[2];
		height = rect[3];

		return *this;
	}

	/*!
	* \brief Sets the components of the rectangle with components from another
	* \return A reference to this rectangle
	*
	* \param rect The other Rect
	*/

	template<typename T>
	Rect<T>& Rect<T>::Set(const Rect<T>& rect)
	{
		std::memcpy(this, &rect, sizeof(Rect));

		return *this;
	}

	/*!
	* \brief Sets the components of the rectange from a vector representing width and height
	* \return A reference to this rectangle
	*
	* \param lengths (Width, Height) of the rectangle
	*
	* \remark Position will be (0, 0)
	*/

	template<typename T>
	Rect<T>& Rect<T>::Set(const Vector2<T>& lengths)
	{
		return Set(lengths.x, lengths.y);
	}

	/*!
	* \brief Set the position and the lenghts of the Rect
	*
	* \param vec1 First point
	* \param vec2 Second point
	*/

	template<typename T>
	Rect<T>& Rect<T>::Set(const Vector2<T>& pos, const Vector2<T>& length)
	{
		x = pos.x;
		y = pos.y;
		width = length.x;
		height = length.y;

		return *this;
	}

	/*!
	* \brief Sets the components of the rectangle from another type of Rect
	* \return A reference to this rectangle
	*
	* \param rect Rectangle of type U to convert its components
	*/

	template<typename T>
	template<typename U>
	Rect<T>& Rect<T>::Set(const Rect<U>& rect)
	{
		x = F(rect.x);
		y = F(rect.y);
		width = F(rect.width);
		height = F(rect.height);

		return *this;
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object: "Rect(x, y, width, height)"
	*/

	template<typename T>
	String Rect<T>::ToString() const
	{
		StringStream ss;

		return ss << "Rect(" << x << ", " << y << ", " << width << ", " << height << ')';
	}

	/*!
	* \brief Translates the rectangle
	* \return A reference to this rectangle translated
	*
	* \param translation Vector2 which is the translation for the position
	*/

	template<typename T>
	Rect<T>& Rect<T>::Translate(const Vector2<T>& translation)
	{
		x += translation.x;
		y += translation.y;

		return *this;
	}

	/*!
	* \brief Returns the ith element of the rectangle
	* \return A reference to the ith element of the rectangle
	*
	* \remark Access to index greather than 4 is undefined behavior
	* \remark Produce a NazaraError if you try to access to index greater than 4 with NAZARA_MATH_SAFE defined
	* \throw std::domain_error if NAZARA_MATH_SAFE is defined and one of you try to acces to index greather than 4
	*/

	template<typename T>
	T& Rect<T>::operator[](std::size_t i)
	{
		NazaraAssert(i < 4, "Index out of range");

		return *(&x+i);
	}

	/*!
	* \brief Returns the ith element of the rectangle
	* \return A value to the ith element of the rectangle
	*
	* \remark Access to index greater than 4 is undefined behavior
	* \remark Produce a NazaraError if you try to access to index greater than 4 with NAZARA_MATH_SAFE defined
	* \throw std::domain_error if NAZARA_MATH_SAFE is defined and one of you try to acces to index greather than 4
	*/

	template<typename T>
	T Rect<T>::operator[](std::size_t i) const
	{
		NazaraAssert(i < 4, "Index out of range");

		return *(&x+i);
	}

	/*!
	* \brief Multiplies the lengths with the scalar
	* \return A rectangle where the position is the same and width and height are the product of the old width and height and the scalar
	*
	* \param scalar The scalar to multiply width and height with
	*/

	template<typename T>
	Rect<T> Rect<T>::operator*(T scalar) const
	{
		return Rect(x, y, width * scalar, height * scalar);
	}

	/*!
	* \brief Multiplies the lengths with the vector
	* \return A rectangle where the position is the same and width and height are the product of the old width and height with the vec
	*
	* \param vec The vector where component one multiply width and two height
	*/

	template<typename T>
	Rect<T> Rect<T>::operator*(const Vector2<T>& vec) const
	{
		return Rect(x, y, width*vec.x, height*vec.y);
	}

	/*!
	* \brief Divides the lengths with the scalar
	* \return A rectangle where the position is the same and width and height are the quotient of the old width and height and the scalar
	*
	* \param scalar The scalar to divide width and height with
	*/

	template<typename T>
	Rect<T> Rect<T>::operator/(T scalar) const
	{
		return Rect(x, y, width / scalar, height / scalar);
	}

	/*!
	* \brief Divides the lengths with the vector
	* \return A rectangle where the position is the same and width and height are the quotient of the old width and height with the vec
	*
	* \param vec The vector where component one divide width and two height
	*/

	template<typename T>
	Rect<T> Rect<T>::operator/(const Vector2<T>& vec) const
	{
		return Rect(x, y, width/vec.x, height/vec.y);
	}

	/*!
	* \brief Multiplies the lengths of this rectangle with the scalar
	* \return A reference to this rectangle where lengths are the product of these lengths and the scalar
	*
	* \param scalar The scalar to multiply width and height with
	*/

	template<typename T>
	Rect<T>& Rect<T>::operator*=(T scalar)
	{
		width *= scalar;
		height *= scalar;

		return *this;
	}

	/*!
	* \brief Multiplies the lengths of this rectangle with the vector
	* \return A reference to this rectangle where width and height are the product of the old width and height with the vec
	*
	* \param vec The vector where component one multiply width and two height
	*/

	template<typename T>
	Rect<T>& Rect<T>::operator*=(const Vector2<T>& vec)
	{
		width *= vec.x;
		height *= vec.y;

		return *this;
	}

	/*!
	* \brief Divides the lengths of this rectangle with the scalar
	* \return A reference to this rectangle where lengths are the quotient of these lengths and the scalar
	*
	* \param scalar The scalar to divide width and height with
	*/

	template<typename T>
	Rect<T>& Rect<T>::operator/=(T scalar)
	{
		width /= scalar;
		height /= scalar;

		return *this;
	}

	/*!
	* \brief Divives the lengths of this rectangle with the vector
	* \return A reference to this rectangle where width and height are the quotient of the old width and height with the vec
	*
	* \param vec The vector where component one divide width and two height
	*/

	template<typename T>
	Rect<T>& Rect<T>::operator/=(const Vector2<T>& vec)
	{
		width /= vec.x;
		height /= vec.y;

		return *this;
	}

	/*!
	* \brief Compares the rectangle to other one
	* \return true if the rectangles are the same
	*
	* \param rect Other rectangle to compare with
	*/

	template<typename T>
	bool Rect<T>::operator==(const Rect& rect) const
	{
		return NumberEquals(x, rect.x) && NumberEquals(y, rect.y) &&
		       NumberEquals(width, rect.width) && NumberEquals(height, rect.height);
	}

	/*!
	* \brief Compares the rectangle to other one
	* \return false if the rectangles are the same
	*
	* \param rect Other rectangle to compare with
	*/

	template<typename T>
	bool Rect<T>::operator!=(const Rect& rect) const
	{
		return !operator==(rect);
	}

	/*!
	* \brief Interpolates the rectangle to other one with a factor of interpolation
	* \return A new rectangle which is the interpolation of two rectangles
	*
	* \param from Initial rectangle
	* \param to Target rectangle
	* \param interpolation Factor of interpolation
	*
	* \remark interpolation is meant to be between 0 and 1, other values are potentially undefined behavior
	* \remark With NAZARA_DEBUG, a NazaraError is thrown and Zero() is returned
	*
	* \see Lerp
	*/

	template<typename T>
	Rect<T> Rect<T>::Lerp(const Rect& from, const Rect& to, T interpolation)
	{
		#ifdef NAZARA_DEBUG
		if (interpolation < F(0.0) || interpolation > F(1.0))
		{
			NazaraError("Interpolation must be in range [0..1] (Got " + String::Number(interpolation) + ')');
			return Zero();
		}
		#endif

		Rect rect;
		rect.x = Nz::Lerp(from.x, to.x, interpolation);
		rect.y = Nz::Lerp(from.y, to.y, interpolation);
		rect.width = Nz::Lerp(from.width, to.width, interpolation);
		rect.height = Nz::Lerp(from.height, to.height, interpolation);

		return rect;
	}

	/*!
	* \brief Shorthand for the rectangle (0, 0, 0, 0)
	* \return A rectangle with position (0, 0) and lengths (0, 0)
	*
	* \see MakeZero
	*/

	template<typename T>
	Rect<T> Rect<T>::Zero()
	{
		Rect rect;
		rect.MakeZero();

		return rect;
	}

	/*!
	* \brief Serializes a Rect
	* \return true if successfully serialized
	*
	* \param context Serialization context
	* \param rect Input Rect
	*/
	template<typename T>
	bool Serialize(SerializationContext& context, const Rect<T>& rect)
	{
		if (!Serialize(context, rect.x))
			return false;

		if (!Serialize(context, rect.y))
			return false;

		if (!Serialize(context, rect.width))
			return false;

		if (!Serialize(context, rect.height))
			return false;

		return true;
	}

	/*!
	* \brief Unserializes a Rect
	* \return true if successfully unserialized
	*
	* \param context Serialization context
	* \param rect Output Rect
	*/
	template<typename T>
	bool Unserialize(SerializationContext& context, Rect<T>* rect)
	{
		if (!Unserialize(context, &rect->x))
			return false;

		if (!Unserialize(context, &rect->y))
			return false;

		if (!Unserialize(context, &rect->width))
			return false;

		if (!Unserialize(context, &rect->height))
			return false;

		return true;
	}
}

/*!
* \brief Output operator
* \return The stream
*
* \param out The stream
* \param rect The rectangle to output
*/

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nz::Rect<T>& rect)
{
	return out << rect.ToString();
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
