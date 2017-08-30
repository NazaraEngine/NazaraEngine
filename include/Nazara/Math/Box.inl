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
	* \class Nz::Box
	* \brief Math class that represents a three dimensional box
	*/

	/*!
	* \brief Constructs a Box object from its width, height and depth
	*
	* \param Width Width of the box (following X)
	* \param Height Height of the box (following Y)
	* \param Depth Depth of the box (following Z)
	*
	* \remark Position will be (0, 0, 0)
	*/

	template<typename T>
	Box<T>::Box(T Width, T Height, T Depth)
	{
		Set(Width, Height, Depth);
	}

	/*!
	* \brief Constructs a Rect<T> object from its position, width, height and depth
	*
	* \param X X position
	* \param Y Y position
	* \param Z Z position
	* \param Width Width of the box (following X)
	* \param Height Height of the box (following Y)
	* \param Depth Depth of the box (following Z)
	*/

	template<typename T>
	Box<T>::Box(T X, T Y, T Z, T Width, T Height, T Depth)
	{
		Set(X, Y, Z, Width, Height, Depth);
	}

	/*!
	* \brief Constructs a Box object from an array of six elements
	*
	* \param vec[6] vec[0] is X position, vec[1] is Y position, vec[2] is Z position, vec[3] is width, vec[4] is height and vec[5] is depth
	*/

	template<typename T>
	Box<T>::Box(const T vec[6])
	{
		Set(vec);
	}

	/*!
	* \brief Constructs a Box object from a Rect
	*
	* \param rect Rectangle which describes (X, Y) position and (width, height) lenghts
	*
	* \remark Z position is 0 and depth is 1
	*/

	template<typename T>
	Box<T>::Box(const Rect<T>& rect)
	{
		Set(rect);
	}

	/*!
	* \brief Constructs a Box object from a vector representing width, height and depth
	*
	* \param lengths (Width, Height, Depth) of the box
	*
	* \remark Positions will be (0, 0, 0)
	*/

	template<typename T>
	Box<T>::Box(const Vector3<T>& lengths)
	{
		Set(lengths);
	}

	/*!
	* \brief Constructs a Box object from two vectors representing point of the space
	* (X, Y, Z) will be the components minimum of the two vectors and the (width, height, depth) will be the components maximum - minimum
	*
	* \param vec1 First point
	* \param vec2 Second point
	*/

	template<typename T>
	Box<T>::Box(const Vector3<T>& vec1, const Vector3<T>& vec2)
	{
		Set(vec1, vec2);
	}

	/*!
	* \brief Constructs a Box object from another type of Box
	*
	* \param box Box of type U to convert to type T
	*/

	template<typename T>
	template<typename U>
	Box<T>::Box(const Box<U>& box)
	{
		Set(box);
	}

	/*!
	* \brief Tests whether the box contains the provided point inclusive of the edge of the box
	* \return true if inclusive
	*
	* \param X X position of the point
	* \param Y Y position of the point
	* \param Z Z position of the point
	*
	* \see Contains
	*/

	template<typename T>
	bool Box<T>::Contains(T X, T Y, T Z) const
	{
		return X >= x && X < x + width &&
		       Y >= y && Y < y + height &&
		       Z >= z && Z < z + depth;
	}

	/*!
	* \brief Tests whether the box contains the provided box inclusive of the edge of the box
	* \return true if inclusive
	*
	* \param box Other box to test
	*
	* \see Contains
	*/

	template<typename T>
	bool Box<T>::Contains(const Box<T>& box) const
	{
		return Contains(box.x, box.y, box.z) &&
		       Contains(box.x + box.width, box.y + box.height, box.z + box.depth);
	}

	/*!
	* \brief Tests whether the box contains the provided point inclusive of the edge of the box
	* \return true if inclusive
	*
	* \param point Position of the point
	*
	* \see Contains
	*/

	template<typename T>
	bool Box<T>::Contains(const Vector3<T>& point) const
	{
		return Contains(point.x, point.y, point.z);
	}

	/*!
	* \brief Extends the box to contain the point in the boundary
	* \return A reference to this box extended
	*
	* \param X X position of the point
	* \param Y Y position of the point
	* \param Z Z position of the point
	*
	* \see ExtendTo
	*/

	template<typename T>
	Box<T>& Box<T>::ExtendTo(T X, T Y, T Z)
	{
		width = std::max(x + width, X);
		height = std::max(y + height, Y);
		depth = std::max(z + depth, Z);

		x = std::min(x, X);
		y = std::min(y, Y);
		z = std::min(z, Z);

		width -= x;
		height -= y;
		depth -= z;

		return *this;
	}

	/*!
	* \brief Extends the box to contain the box
	* \return A reference to this box extended
	*
	* \param box Other box to contain
	*
	* \see ExtendTo
	*/

	template<typename T>
	Box<T>& Box<T>::ExtendTo(const Box& box)
	{
		width = std::max(x + width, box.x + box.width);
		height = std::max(y + height, box.y + box.height);
		depth = std::max(z + depth, box.z + box.depth);

		x = std::min(x, box.x);
		y = std::min(y, box.y);
		z = std::min(z, box.z);

		width -= x;
		height -= y;
		depth -= z;

		return *this;
	}

	/*!
	* \brief Extends the box to contain the point in the boundary
	* \return A reference to this box extended
	*
	* \param point Position of the point
	*
	* \see ExtendTo
	*/

	template<typename T>
	Box<T>& Box<T>::ExtendTo(const Vector3<T>& point)
	{
		return ExtendTo(point.x, point.y, point.z);
	}

	/*!
	* \brief Gets the bounding sphere for the box
	* \return A sphere containing the box
	*
	* \see GetSquaredBoundingSphere
	*/

	template<typename T>
	Sphere<T> Box<T>::GetBoundingSphere() const
	{
		return Sphere<T>(GetCenter(), GetRadius());
	}

	/*!
	* \brief Gets a Vector3 for the center
	* \return The position of the center of the box
	*/

	template<typename T>
	Vector3<T> Box<T>::GetCenter() const
	{
		return GetPosition() + GetLengths() / F(2.0);
	}

	/*!
	* \brief Gets the Vector3 for the corner
	* \return The position of the corner of the box according to enum BoxCorner
	*
	* \param corner Enumeration of type BoxCorner
	*
	* \remark If enumeration is not defined in BoxCorner, a NazaraError is thrown and a Vector3 uninitialised is returned
	*/

	template<typename T>
	Vector3<T> Box<T>::GetCorner(BoxCorner corner) const
	{
		switch (corner)
		{
			case BoxCorner_FarLeftBottom:
				return Vector3<T>(x, y, z);

			case BoxCorner_FarLeftTop:
				return Vector3<T>(x, y + height, z);

			case BoxCorner_FarRightBottom:
				return Vector3<T>(x + width, y, z);

			case BoxCorner_FarRightTop:
				return Vector3<T>(x + width, y + height, z);

			case BoxCorner_NearLeftBottom:
				return Vector3<T>(x, y, z + depth);

			case BoxCorner_NearLeftTop:
				return Vector3<T>(x, y + height, z + depth);

			case BoxCorner_NearRightBottom:
				return Vector3<T>(x + width, y, z + depth);

			case BoxCorner_NearRightTop:
				return Vector3<T>(x + width, y + height, z + depth);
		}

		NazaraError("Corner not handled (0x" + String::Number(corner, 16) + ')');
		return Vector3<T>();
	}

	/*!
	* \brief Gets a Vector3 for the lengths
	* \return The lengths of the box (width, height, depth)
	*/

	template<typename T>
	Vector3<T> Box<T>::GetLengths() const
	{
		return Vector3<T>(width, height, depth);
	}

	/*!
	* \brief Gets a Vector3 for the maximum point
	* \return The BoxCorner_NearRightTop of the box
	*
	* \see GetCorner
	*/

	template<typename T>
	Vector3<T> Box<T>::GetMaximum() const
	{
		return GetPosition() + GetLengths();
	}

	/*!
	* \brief Gets a Vector3 for the minimum point
	* \return The BoxCorner_FarLeftBottom of the box
	*
	* \see GetCorner, GetPosition
	*/

	template<typename T>
	Vector3<T> Box<T>::GetMinimum() const
	{
		return GetPosition();
	}

	/*!
	* \brief Computes the negative vertex of one direction
	* \return The position of the vertex on the box in the opposite way of the normal while considering the center. It means that if the normal has one component negative, the component is set to width, height or depth corresponding to the sign
	*
	* \param normal Vector indicating a direction
	*
	* \see GetPositiveVertex
	*/

	template<typename T>
	Vector3<T> Box<T>::GetNegativeVertex(const Vector3<T>& normal) const
	{
		Vector3<T> neg(GetPosition());

		if (normal.x < F(0.0))
			neg.x += width;

		if (normal.y < F(0.0))
			neg.y += height;

		if (normal.z < F(0.0))
			neg.z += depth;

		return neg;
	}

	/*!
	* \brief Gets a Vector3 for the position
	* \return The BoxCorner_FarLeftBottom of the box
	*
	* \see GetCorner, GetMinimum
	*/

	template<typename T>
	Vector3<T> Box<T>::GetPosition() const
	{
		return Vector3<T>(x, y, z);
	}

	/*!
	* \brief Computes the positive vertex of one direction
	* \return The position of the vertex on the box in the same way of the normal while considering the center. It means that if the normal has one component positive, the component is set to width or height corresponding to the sign
	*
	* \param normal Vector indicating a direction
	*
	* \see GetNegativeVertex
	*/

	template<typename T>
	Vector3<T> Box<T>::GetPositiveVertex(const Vector3<T>& normal) const
	{
		Vector3<T> pos(GetPosition());

		if (normal.x > F(0.0))
			pos.x += width;

		if (normal.y > F(0.0))
			pos.y += height;

		if (normal.z > F(0.0))
			pos.z += depth;

		return pos;
	}

	/*!
	* \brief Gets the radius of the box
	* \return Value of the radius which is the biggest distance between a corner and the center
	*/

	template<typename T>
	T Box<T>::GetRadius() const
	{
		return std::sqrt(GetSquaredRadius());
	}

	/*!
	* \brief Gets the squared bounding sphere for the box
	* \return A sphere containing the box
	*
	* \see GetBoundingSphere
	*/

	template<typename T>
	Sphere<T> Box<T>::GetSquaredBoundingSphere() const
	{
		return Sphere<T>(GetCenter(), GetSquaredRadius());
	}

	/*!
	* \brief Gets the squared radius of the box
	* \return Value of the squared radius which is the squared of biggest distance between a corner and the center
	*/

	template<typename T>
	T Box<T>::GetSquaredRadius() const
	{
		Vector3<T> size(GetLengths());
		size /= F(2.0); // The size only depends on the lengths and not the center

		return size.GetSquaredLength();
	}

	/*!
	* \brief Checks whether or not this box intersects another one
	* \return true if the box intersects
	*
	* \param box Box to check
	* \param intersection Optional argument for the box which represent the intersection
	*/

	template<typename T>
	bool Box<T>::Intersect(const Box& box, Box* intersection) const
	{
		T left = std::max(x, box.x);
		T right = std::min(x + width, box.x + box.width);
		if (left >= right)
			return false;

		T top = std::max(y, box.y);
		T bottom = std::min(y + height, box.y + box.height);
		if (top >= bottom)
			return false;

		T up = std::max(z, box.z);
		T down = std::min(z + depth, box.z + box.depth);
		if (up >= down)
			return false;

		if (intersection)
		{
			intersection->x = left;
			intersection->y = top;
			intersection->z = up;
			intersection->width = right - left;
			intersection->height = bottom - top;
			intersection->depth = down - up;
		}

		return true;
	}

	/*!
	* \brief Checks whether this box is valid
	* \return true if the box has a strictly positive width, height and depth
	*/

	template<typename T>
	bool Box<T>::IsValid() const
	{
		return width > F(0.0) && height > F(0.0) && depth > F(0.0);
	}

	/*!
	* \brief Makes the box position (0, 0, 0) and lengths (0, 0, 0)
	* \return A reference to this box with position (0, 0, 0) and lengths (0, 0, 0)
	*
	* \see Zero
	*/

	template<typename T>
	Box<T>& Box<T>::MakeZero()
	{
		x = F(0.0);
		y = F(0.0);
		z = F(0.0);
		width = F(0.0);
		height = F(0.0);
		depth = F(0.0);

		return *this;
	}

	/*!
	* \brief Sets the components of the box with width, height and depth
	* \return A reference to this box
	*
	* \param Width Width of the box (following X)
	* \param Height Height of the box (following Y)
	* \param Depth Depth of the box (following Z)
	*
	* \remark Position will be (0, 0, 0)
	*/

	template<typename T>
	Box<T>& Box<T>::Set(T Width, T Height, T Depth)
	{
		x = F(0.0);
		y = F(0.0);
		z = F(0.0);
		width = Width;
		height = Height;
		depth = Depth;

		return *this;
	}

	/*!
	* \brief Constructs a Box object from its position and sizes
	*
	* \param X X component of position
	* \param Y Y component of position
	* \param Z Z component of position
	* \param Width Width of the box (following X)
	* \param Height Height of the box (following Y)
	* \param Depth Depth of the box (following Z)
	*/

	template<typename T>
	Box<T>& Box<T>::Set(T X, T Y, T Z, T Width, T Height, T Depth)
	{
		x = X;
		y = Y;
		z = Z;
		width = Width;
		height = Height;
		depth = Depth;

		return *this;
	}

	/*!
	* \brief Sets the components of the box from an array of six elements
	* \return A reference to this box
	*
	* \param box[6] box[0] is X position, box[1] is Y position, box[2] is Z position, box[3] is width, box[4] is height and box[5] is depth
	*/

	template<typename T>
	Box<T>& Box<T>::Set(const T box[6])
	{
		x = box[0];
		y = box[1];
		z = box[2];
		width = box[3];
		height = box[4];
		depth = box[5];

		return *this;
	}

	/*!
	* \brief Sets the components of the box with components from another
	* \return A reference to this box
	*
	* \param box The other box
	*/

	template<typename T>
	Box<T>& Box<T>::Set(const Box& box)
	{
		std::memcpy(this, &box, sizeof(Box));

		return *this;
	}

	/*!
	* \brief Sets the components of the box with components from a Rect
	* \return A reference to this box
	*
	* \param rect Rectangle which describes (X, Y) position and (width, height) lenghts
	*
	* \remark Z position is 0 and depth is 1
	*/

	template<typename T>
	Box<T>& Box<T>::Set(const Rect<T>& rect)
	{
		x = rect.x;
		y = rect.y;
		z = F(0.0);
		width = rect.width;
		height = rect.height;
		depth = F(1.0);

		return *this;
	}

	/*!
	* \brief Sets the components of the box from a vector representing width, height and depth
	* \return A reference to this box
	*
	* \param lengths (Width, Height, depth) of the box
	*
	* \remark Position will be (0, 0, 0)
	*/

	template<typename T>
	Box<T>& Box<T>::Set(const Vector3<T>& lengths)
	{
		return Set(lengths.x, lengths.y, lengths.z);
	}

	/*!
	* \brief Sets the components of the box from two vectors representing point of the space
	* (X, Y, Z) will be the components minimum of the two vectors and the (width, height, depth) will be the components maximum - minimum
	* \return A reference to this box
	*
	* \param vec1 First point
	* \param vec2 Second point
	*/

	template<typename T>
	Box<T>& Box<T>::Set(const Vector3<T>& vec1, const Vector3<T>& vec2)
	{
		x = std::min(vec1.x, vec2.x);
		y = std::min(vec1.y, vec2.y);
		z = std::min(vec1.z, vec2.z);
		width = (vec2.x > vec1.x) ? vec2.x - vec1.x : vec1.x - vec2.x;
		height = (vec2.y > vec1.y) ? vec2.y - vec1.y : vec1.y - vec2.y;
		depth = (vec2.z > vec1.z) ? vec2.z - vec1.z : vec1.z - vec2.z;

		return *this;
	}

	/*!
	* \brief Sets the components of the box from another type of Box
	* \return A reference to this box
	*
	* \param box Box of type U to convert its components
	*/

	template<typename T>
	template<typename U>
	Box<T>& Box<T>::Set(const Box<U>& box)
	{
		x = F(box.x);
		y = F(box.y);
		z = F(box.z);
		width = F(box.width);
		height = F(box.height);
		depth = F(box.depth);

		return *this;
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object: "Box(x, y, z, width, height, depth)"
	*/

	template<typename T>
	String Box<T>::ToString() const
	{
		StringStream ss;

		return ss << "Box(" << x << ", " << y << ", " << z << ", " << width << ", " << height << ", " << depth << ')';
	}

	/*!
	* \brief Transforms the box according to the matrix
	* \return A reference to this box transformed
	*
	* \param matrix Matrix4 representing the transformation
	* \param applyTranslation Should transform the position or the direction
	*/

	template<typename T>
	Box<T>& Box<T>::Transform(const Matrix4<T>& matrix, bool applyTranslation)
	{
		Vector3<T> center = matrix.Transform(GetCenter(), (applyTranslation) ? F(1.0) : F(0.0)); // Value multiplying the translation
		Vector3<T> halfSize = GetLengths() / F(2.0);

		halfSize.Set(std::abs(matrix(0,0)) * halfSize.x + std::abs(matrix(1,0)) * halfSize.y + std::abs(matrix(2,0)) * halfSize.z,
		             std::abs(matrix(0,1)) * halfSize.x + std::abs(matrix(1,1)) * halfSize.y + std::abs(matrix(2,1)) * halfSize.z,
		             std::abs(matrix(0,2)) * halfSize.x + std::abs(matrix(1,2)) * halfSize.y + std::abs(matrix(2,2)) * halfSize.z);

		return Set(center - halfSize, center + halfSize);
	}

	/*!
	* \brief Translates the box
	* \return A reference to this box translated
	*
	* \param translation Vector3 which is the translation for the position
	*/

	template<typename T>
	Box<T>& Box<T>::Translate(const Vector3<T>& translation)
	{
		x += translation.x;
		y += translation.y;
		z += translation.z;

		return *this;
	}

	/*!
	* \brief Returns the ith element of the box
	* \return A reference to the ith element of the box
	*
	* \remark Access to index greater than 6 is undefined behavior
	* \remark Produce a NazaraError if you try to access to index greater than 6 with NAZARA_MATH_SAFE defined
	* \throw std::domain_error if NAZARA_MATH_SAFE is defined and one of you try to acces to index greather than 6
	*/

	template<typename T>
	T& Box<T>::operator[](std::size_t i)
	{
		NazaraAssert(i < 6, "Index out of range");

		return *(&x+i);
	}

	/*!
	* \brief Returns the ith element of the box
	* \return A value to the ith element of the box
	*
	* \remark Access to index greater than 6 is undefined behavior
	* \remark Produce a NazaraError if you try to access to index greater than 6 with NAZARA_MATH_SAFE defined
	* \throw std::domain_error if NAZARA_MATH_SAFE is defined and one of you try to acces to index greather than 6
	*/

	template<typename T>
	T Box<T>::operator[](std::size_t i) const
	{
		NazaraAssert(i < 6, "Index out of range");

		return *(&x+i);
	}

	/*!
	* \brief Multiplies the lengths with the scalar
	* \return A box where the position is the same and width, height and depth are the product of the old width, height and depth and the scalar
	*
	* \param scalar The scalar to multiply width, height and depth with
	*/

	template<typename T>
	Box<T> Box<T>::operator*(T scalar) const
	{
		return Box(x, y, z, width * scalar, height * scalar, depth * scalar);
	}

	/*!
	* \brief Multiplies the lengths with the vector
	* \return A box where the position is the same and width, height and depth are the product of the old width, height and depth with the vec
	*
	* \param vec The vector where component one multiply width, two height and three depth
	*/

	template<typename T>
	Box<T> Box<T>::operator*(const Vector3<T>& vec) const
	{
		return Box(x, y, z, width * vec.x, height * vec.y, depth * vec.z);
	}

	/*!
	* \brief Multiplies the lengths of this box with the scalar
	* \return A reference to this box where lengths are the product of these lengths and the scalar
	*
	* \param scalar The scalar to multiply width, height and depth with
	*/

	template<typename T>
	Box<T>& Box<T>::operator*=(T scalar)
	{
		width *= scalar;
		height *= scalar;
		depth *= scalar;

		return *this;
	}

	/*!
	* \brief Multiplies the lengths of this box with the vector
	* \return A reference to this box where width, height and depth are the product of the old width,  height and depth with the vec
	*
	* \param vec The vector where component one multiply width, two height and three depth
	*/

	template<typename T>
	Box<T>& Box<T>::operator*=(const Vector3<T>& vec)
	{
		width *= vec.x;
		height *= vec.y;
		depth *= vec.z;

		return *this;
	}

	/*!
	* \brief Compares the box to other one
	* \return true if the boxes are the same
	*
	* \param box Other box to compare with
	*/

	template<typename T>
	bool Box<T>::operator==(const Box& box) const
	{
		return NumberEquals(x, box.x) && NumberEquals(y, box.y) && NumberEquals(z, box.z) &&
		       NumberEquals(width, box.width) && NumberEquals(height, box.height) && NumberEquals(depth, box.depth);
	}

	/*!
	* \brief Compares the box to other one
	* \return false if the boxes are the same
	*
	* \param box Other box to compare with
	*/

	template<typename T>
	bool Box<T>::operator!=(const Box& box) const
	{
		return !operator==(box);
	}

	/*!
	* \brief Interpolates the box to other one with a factor of interpolation
	* \return A new box which is the interpolation of two rectangles
	*
	* \param from Initial box
	* \param to Target box
	* \param interpolation Factor of interpolation
	*
	* \remark interpolation is meant to be between 0 and 1, other values are potentially undefined behavior
	* \remark With NAZARA_DEBUG, a NazaraError is thrown and Zero() is returned
	*
	* \see Lerp
	*/

	template<typename T>
	Box<T> Box<T>::Lerp(const Box& from, const Box& to, T interpolation)
	{
		#ifdef NAZARA_DEBUG
		if (interpolation < F(0.0) || interpolation > F(1.0))
		{
			NazaraError("Interpolation must be in range [0..1] (Got " + String::Number(interpolation) + ')');
			return Zero();
		}
		#endif

		Box box;
		box.x = Nz::Lerp(from.x, to.x, interpolation);
		box.y = Nz::Lerp(from.y, to.y, interpolation);
		box.z = Nz::Lerp(from.z, to.z, interpolation);
		box.width = Nz::Lerp(from.width, to.width, interpolation);
		box.height = Nz::Lerp(from.height, to.height, interpolation);
		box.depth = Nz::Lerp(from.depth, to.depth, interpolation);

		return box;
	}

	/*!
	* \brief Shorthand for the box (0, 0, 0, 0, 0, 0)
	* \return A box with position (0, 0, 0) and lengths (0, 0, 0)
	*
	* \see MakeZero
	*/

	template<typename T>
	Box<T> Box<T>::Zero()
	{
		Box box;
		box.MakeZero();

		return box;
	}

	/*!
	* \brief Serializes a Box
	* \return true if successfully serialized
	*
	* \param context Serialization context
	* \param box Input Box
	*/
	template<typename T>
	bool Serialize(SerializationContext& context, const Box<T>& box)
	{
		if (!Serialize(context, box.x))
			return false;

		if (!Serialize(context, box.y))
			return false;

		if (!Serialize(context, box.z))
			return false;

		if (!Serialize(context, box.width))
			return false;

		if (!Serialize(context, box.height))
			return false;

		if (!Serialize(context, box.depth))
			return false;

		return true;
	}

	/*!
	* \brief Unserializes a Box
	* \return true if successfully unserialized
	*
	* \param context Serialization context
	* \param box Output Box
	*/
	template<typename T>
	bool Unserialize(SerializationContext& context, Box<T>* box)
	{
		if (!Unserialize(context, &box->x))
			return false;

		if (!Unserialize(context, &box->y))
			return false;

		if (!Unserialize(context, &box->z))
			return false;

		if (!Unserialize(context, &box->width))
			return false;

		if (!Unserialize(context, &box->height))
			return false;

		if (!Unserialize(context, &box->depth))
			return false;

		return true;
	}
}

/*!
* \brief Output operator
* \return The stream
*
* \param out The stream
* \param box The box to output
*/

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nz::Box<T>& box)
{
	return out << box.ToString();
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
