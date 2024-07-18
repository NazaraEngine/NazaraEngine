// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/StringExt.hpp>
#include <NazaraUtils/MathUtils.hpp>
#include <algorithm>
#include <cstring>
#include <sstream>

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
	constexpr Box<T>::Box(T Width, T Height, T Depth) :
	x(0),
	y(0),
	z(0),
	width(Width),
	height(Height),
	depth(Depth)
	{
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
	constexpr Box<T>::Box(T X, T Y, T Z, T Width, T Height, T Depth) :
	x(X),
	y(Y),
	z(Z),
	width(Width),
	height(Height),
	depth(Depth)
	{
	}

	/*!
	* \brief Constructs a Box object from a Rect
	*
	* \param rect Rectangle which describes (X, Y) position and (width, height) lenghts
	*
	* \remark Z position is 0 and depth is 1
	*/
	template<typename T>
	constexpr Box<T>::Box(const Rect<T>& rect) :
	x(rect.x),
	y(rect.y),
	z(0),
	width(rect.width),
	height(rect.height),
	depth(0)
	{
	}

	/*!
	* \brief Constructs a Rect object from a vector representing its length
	* The position will be set to zero
	*
	* \param pos (X, Y, Z) of the box
	* \param lengths (Width, Height, Depth) of the box
	*/
	template<typename T>
	constexpr Box<T>::Box(const Vector3<T>& lengths) :
	Box(Vector3<T>::Zero(), lengths)
	{
	}

	/*!
	* \brief Constructs a Box object from two vector representing position and lengths
	*
	* \param pos (X, Y, Z) of the box
	* \param lengths (Width, Height, Depth) of the box
	*/
	template<typename T>
	constexpr Box<T>::Box(const Vector3<T>& pos, const Vector3<T>& lengths) :
	x(pos.x),
	y(pos.y),
	z(pos.z),
	width(lengths.x),
	height(lengths.y),
	depth(lengths.z)
	{
	}

	/*!
	* \brief Constructs a Box object from another type of Box
	*
	* \param box Box of type U to convert to type T
	*/
	template<typename T>
	template<typename U>
	constexpr Box<T>::Box(const Box<U>& box) :
	x(static_cast<T>(box.x)),
	y(static_cast<T>(box.y)),
	z(static_cast<T>(box.z)),
	width(static_cast<T>(box.width)),
	height(static_cast<T>(box.height)),
	depth(static_cast<T>(box.depth))
	{
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
	constexpr bool Box<T>::ApproxEqual(const Box& box, T maxDifference) const
	{
		return NumberEquals(x, box.x, maxDifference) && NumberEquals(y, box.y, maxDifference) && NumberEquals(z, box.z, maxDifference) &&
		       NumberEquals(width, box.width, maxDifference) && NumberEquals(height, box.height, maxDifference) && NumberEquals(depth, box.depth, maxDifference);
	}

	template<typename T>
	constexpr bool Box<T>::Contains(T X, T Y, T Z) const
	{
		return X >= GetLeft() && X < GetRight() &&
		       Y >= GetTop() && Y < GetBottom() &&
		       Z >= GetNear() && Z < GetFar();
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
	constexpr bool Box<T>::Contains(const Box<T>& box) const
	{
		return Contains(box.GetLeft(), box.GetTop(), box.GetNear()) &&
		       Contains(box.GetRight(), box.GetBottom(), box.GetFar());
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
	constexpr bool Box<T>::Contains(const Vector3<T>& point) const
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
	constexpr Box<T>& Box<T>::ExtendTo(T X, T Y, T Z)
	{
		width = std::max(GetRight(), X);
		height = std::max(GetBottom(), Y);
		depth = std::max(GetFar(), Z);

		x = std::min(GetLeft(), X);
		y = std::min(GetTop(), Y);
		z = std::min(GetNear(), Z);

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
	constexpr Box<T>& Box<T>::ExtendTo(const Box& box)
	{
		width = std::max(GetRight(), box.GetRight());
		height = std::max(GetBottom(), box.GetBottom());
		depth = std::max(GetFar(), box.GetFar());

		x = std::min(GetLeft(), box.GetLeft());
		y = std::min(GetTop(), box.GetTop());
		z = std::min(GetNear(), box.GetNear());

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
	constexpr Box<T>& Box<T>::ExtendTo(const Vector3<T>& point)
	{
		return ExtendTo(point.x, point.y, point.z);
	}

	template<typename T>
	constexpr T Box<T>::GetBottom() const
	{
		if constexpr (std::is_floating_point_v<T>)
			return IsInfinity(height) ? Infinity<T> : y + height;
		else
			return y + height;
	}

	/*!
	* \brief Gets the bounding sphere for the box
	* \return A sphere containing the box
	*
	* \see GetSquaredBoundingSphere
	*/
	template<typename T>
	constexpr Sphere<T> Box<T>::GetBoundingSphere() const
	{
		return Sphere<T>(GetCenter(), GetRadius());
	}

	/*!
	* \brief Gets a Vector3 for the center
	* \return The position of the center of the box
	*/
	template<typename T>
	constexpr Vector3<T> Box<T>::GetCenter() const
	{
		return GetPosition() + GetLengths() / T(2.0);
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
	constexpr Vector3<T> Box<T>::GetCorner(BoxCorner corner) const
	{
		switch (corner)
		{
			case BoxCorner::FarLeftBottom:
				return Vector3<T>(GetLeft(), GetBottom(), GetFar());

			case BoxCorner::FarLeftTop:
				return Vector3<T>(GetLeft(), GetTop(), GetFar());

			case BoxCorner::FarRightBottom:
				return Vector3<T>(GetRight(), GetBottom(), GetFar());

			case BoxCorner::FarRightTop:
				return Vector3<T>(GetRight(), GetTop(), GetFar());

			case BoxCorner::NearLeftBottom:
				return Vector3<T>(GetLeft(), GetBottom(), GetNear());

			case BoxCorner::NearLeftTop:
				return Vector3<T>(GetLeft(), GetTop(), GetNear());

			case BoxCorner::NearRightBottom:
				return Vector3<T>(GetRight(), GetBottom(), GetNear());

			case BoxCorner::NearRightTop:
				return Vector3<T>(GetRight(), GetTop(), GetNear());
		}

		NazaraErrorFmt("Corner not handled ({0:#x})", UnderlyingCast(corner));
		return Vector3<T>();
	}

	template<typename T>
	constexpr EnumArray<BoxCorner, Vector3<T>> Box<T>::GetCorners() const
	{
		return {
			GetCorner(BoxCorner::FarLeftBottom),
			GetCorner(BoxCorner::FarLeftTop),
			GetCorner(BoxCorner::FarRightBottom),
			GetCorner(BoxCorner::FarRightTop),
			GetCorner(BoxCorner::NearLeftBottom),
			GetCorner(BoxCorner::NearLeftTop),
			GetCorner(BoxCorner::NearRightBottom),
			GetCorner(BoxCorner::NearRightTop)
		};
	}

	template<typename T>
	constexpr T Box<T>::GetFar() const
	{
		if constexpr (std::is_floating_point_v<T>)
			return IsInfinity(depth) ? Infinity<T> : z + depth;
		else
			return z + depth;
	}

	template<typename T>
	constexpr T Box<T>::GetLeft() const
	{
		return x;
	}

	/*!
	* \brief Gets a Vector3 for the lengths
	* \return The lengths of the box (width, height, depth)
	*/
	template<typename T>
	constexpr Vector3<T> Box<T>::GetLengths() const
	{
		return Vector3<T>(width, height, depth);
	}

	/*!
	* \brief Gets a Vector3 for the maximum point
	* \return The BoxCorner::NearRightTop of the box
	*
	* \see GetCorner
	*/
	template<typename T>
	constexpr Vector3<T> Box<T>::GetMaximum() const
	{
		return { GetRight(), GetBottom(), GetFar() };
	}

	/*!
	* \brief Gets a Vector3 for the minimum point
	* \return The BoxCorner::FarLeftBottom of the box
	*
	* \see GetCorner, GetPosition
	*/
	template<typename T>
	constexpr Vector3<T> Box<T>::GetMinimum() const
	{
		return { GetLeft(), GetTop(), GetNear() };
	}

	template<typename T>
	constexpr T Box<T>::GetNear() const
	{
		return z;
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
	constexpr Vector3<T> Box<T>::GetNegativeVertex(const Vector3<T>& normal) const
	{
		Vector3<T> neg;
		neg.x = (normal.x < T(0.0)) ? GetRight() : GetLeft();
		neg.y = (normal.y < T(0.0)) ? GetBottom() : GetTop();
		neg.z = (normal.z < T(0.0)) ? GetFar() : GetNear();

		return neg;
	}

	/*!
	* \brief Gets a Vector3 for the position
	* \return The BoxCorner::FarLeftBottom of the box
	*
	* \see GetCorner, GetMinimum
	*/
	template<typename T>
	constexpr Vector3<T> Box<T>::GetPosition() const
	{
		return Vector3<T>(GetLeft(), GetTop(), GetNear());
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
	constexpr Vector3<T> Box<T>::GetPositiveVertex(const Vector3<T>& normal) const
	{
		Vector3<T> pos;
		pos.x = (normal.x > T(0.0)) ? GetRight() : GetLeft();
		pos.y = (normal.y > T(0.0)) ? GetBottom() : GetTop();
		pos.z = (normal.z > T(0.0)) ? GetFar() : GetNear();

		return pos;
	}

	/*!
	* \brief Gets the radius of the box
	* \return Value of the radius which is the biggest distance between a corner and the center
	*/
	template<typename T>
	constexpr T Box<T>::GetRadius() const
	{
		return std::sqrt(GetSquaredRadius());
	}

	template<typename T>
	constexpr T Box<T>::GetRight() const
	{
		if constexpr (std::is_floating_point_v<T>)
			return IsInfinity(width) ? Infinity<T> : x + width;
		else
			return x + width;
	}

	/*!
	* \brief Gets the squared bounding sphere for the box
	* \return A sphere containing the box
	*
	* \see GetBoundingSphere
	*/
	template<typename T>
	constexpr Sphere<T> Box<T>::GetSquaredBoundingSphere() const
	{
		return Sphere<T>(GetCenter(), GetSquaredRadius());
	}

	/*!
	* \brief Gets the squared radius of the box
	* \return Value of the squared radius which is the squared of biggest distance between a corner and the center
	*/
	template<typename T>
	constexpr T Box<T>::GetSquaredRadius() const
	{
		Vector3<T> size(GetLengths());
		size /= T(2.0); // The size only depends on the lengths and not the center

		return size.GetSquaredLength();
	}

	template<typename T>
	constexpr T Box<T>::GetTop() const
	{
		return y;
	}

	/*!
	* \brief Checks whether or not this box intersects another one
	* \return true if the box intersects
	*
	* \param box Box to check
	* \param intersection Optional argument for the box which represent the intersection
	*/
	template<typename T>
	constexpr bool Box<T>::Intersect(const Box& box, Box* intersection) const
	{
		T left = std::max(GetLeft(), box.GetLeft());
		T right = std::min(GetRight(), box.GetRight());
		T bottom = std::max(GetBottom(), box.GetBottom());
		T top = std::min(GetTop(), box.GetTop());
		T near = std::max(GetNear(), box.GetNear());
		T far = std::min(GetFar(), box.GetFar());

		if (left > right || top > bottom || near > far)
			return false;

		if (intersection)
		{
			intersection->x = left;
			intersection->y = top;
			intersection->z = near;
			if constexpr (std::is_floating_point_v<T>)
			{
				intersection->width = IsInfinity(right) ? Infinity<T> : right - left;
				intersection->height = IsInfinity(bottom) ? Infinity<T> : bottom - top;
				intersection->depth = IsInfinity(far) ? Infinity<T> : far - near;
			}
			else
			{
				intersection->width = right - left;
				intersection->height = bottom - top;
				intersection->depth = far - near;
			}
		}

		return true;
	}

	/*!
	* \brief Checks whether this box is valid
	* \return true if the box has a negative or zero width, height and depth
	*/
	template<typename T>
	constexpr bool Box<T>::IsNull() const
	{
		return width <= T(0.0) && height <= T(0.0) && depth <= T(0.0);
	}

	/*!
	* \brief Checks whether this box is valid
	* \return true if the box has a positive width, height and depth
	*/
	template<typename T>
	constexpr bool Box<T>::IsValid() const
	{
		return width >= T(0.0) && height >= T(0.0) && depth >= T(0.0);
	}

	/*!
	* \brief Multiplies the lengths of this box with the scalar
	* \return A reference to this box where lengths are the product of these lengths and the scalar
	*
	* \param scalar The scalar to multiply width, height and depth with
	*/
	template<typename T>
	constexpr Box<T>& Box<T>::Scale(T scalar)
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
	constexpr Box<T>& Box<T>::Scale(const Vector3<T>& vec)
	{
		width *= vec.x;
		height *= vec.y;
		depth *= vec.z;

		return *this;
	}

	/*!
	* \brief Multiplies the lengths of this box with the scalar (the box center doesn't move)
	* \return A reference to this box where lengths are the product of these lengths and the scalar
	*
	* \param scalar The scalar to multiply width, height and depth with
	*/
	template<typename T>
	constexpr Box<T>& Box<T>::ScaleAroundCenter(T scalar)
	{
		x -= (width  * scalar - width)  / T(2.0);
		y -= (height * scalar - height) / T(2.0);
		z -= (depth  * scalar - depth)  / T(2.0);

		width  *= scalar;
		height *= scalar;
		depth  *= scalar;

		return *this;
	}

	/*!
	* \brief Multiplies the lengths of this box with the vector but changes the origin (the box center doesn't move)
	* \return A reference to this box where width, height and depth are the product of the old width,  height and depth with the vec
	*
	* \param vec The vector where component one multiply width, two height and three depth
	*/
	template<typename T>
	constexpr Box<T>& Box<T>::ScaleAroundCenter(const Vector3<T>& vec)
	{
		x -= (width  * vec.x - width)  / T(2.0);
		y -= (height * vec.y - height) / T(2.0);
		z -= (depth  * vec.z - depth)  / T(2.0);

		width  *= vec.x;
		height *= vec.y;
		depth  *= vec.z;

		return *this;
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object: "Box(x, y, z, width, height, depth)"
	*/
	template<typename T>
	std::string Box<T>::ToString() const
	{
		std::ostringstream ss;
		ss << *this;

		return ss.str();
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
		Vector3<T> center = matrix.Transform(GetCenter(), (applyTranslation) ? T(1.0) : T(0.0)); // Value multiplying the translation
		Vector3<T> halfSize = GetLengths() / T(2.0);

		halfSize = Vector3f(std::abs(matrix(0,0)) * halfSize.x + std::abs(matrix(1,0)) * halfSize.y + std::abs(matrix(2,0)) * halfSize.z,
		                    std::abs(matrix(0,1)) * halfSize.x + std::abs(matrix(1,1)) * halfSize.y + std::abs(matrix(2,1)) * halfSize.z,
		                    std::abs(matrix(0,2)) * halfSize.x + std::abs(matrix(1,2)) * halfSize.y + std::abs(matrix(2,2)) * halfSize.z);

		return operator=(Boxf::FromExtents(center - halfSize, center + halfSize));
	}

	/*!
	* \brief Translates the box
	* \return A reference to this box translated
	*
	* \param translation Vector3 which is the translation for the position
	*/
	template<typename T>
	constexpr Box<T>& Box<T>::Translate(const Vector3<T>& translation)
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
	*/
	template<typename T>
	constexpr T& Box<T>::operator[](std::size_t i)
	{
		NazaraAssertFmt(i < 6, "index out of range ({0} >= 6)", i);

		return *(&x+i);
	}

	/*!
	* \brief Returns the ith element of the box
	* \return A value to the ith element of the box
	*
	* \remark Access to index greater than 6 is undefined behavior
	*/
	template<typename T>
	constexpr const T& Box<T>::operator[](std::size_t i) const
	{
		NazaraAssertFmt(i < 6, "index out of range ({0} >= 6)", i);

		return *(&x+i);
	}

	/*!
	* \brief Compares the box to other one
	* \return true if the boxes are the same
	*
	* \param box Other box to compare with
	*/
	template<typename T>
	constexpr bool Box<T>::operator==(const Box& box) const
	{
		return x == box.x && y == box.y && z == box.z && width == box.width && height == box.height && depth == box.depth;
	}

	/*!
	* \brief Compares the box to other one
	* \return false if the boxes are the same
	*
	* \param box Other box to compare with
	*/
	template<typename T>
	constexpr bool Box<T>::operator!=(const Box& box) const
	{
		return !operator==(box);
	}

	template<typename T>
	constexpr Box<T> Box<T>::ApproxEqual(const Box& lhs, const Box& rhs, T maxDifference)
	{
		return lhs.ApproxEqual(rhs, maxDifference);
	}

	template<typename T>
	constexpr Box<T> Box<T>::FromExtents(const Vector3<T>& vec1, const Vector3<T>& vec2)
	{
		Box box;
		box.x = std::min(vec1.x, vec2.x);
		box.y = std::min(vec1.y, vec2.y);
		box.z = std::min(vec1.z, vec2.z);
		box.width = (vec2.x > vec1.x) ? vec2.x - vec1.x : vec1.x - vec2.x;
		box.height = (vec2.y > vec1.y) ? vec2.y - vec1.y : vec1.y - vec2.y;
		box.depth = (vec2.z > vec1.z) ? vec2.z - vec1.z : vec1.z - vec2.z;

		return box;
	}

	/*!
	* \brief Interpolates the box to other one with a factor of interpolation
	* \return A new box which is the interpolation of two rectangles
	*
	* \param from Initial box
	* \param to Target box
	* \param interpolation Factor of interpolation
	*
	* \see Lerp
	*/
	template<typename T>
	constexpr Box<T> Box<T>::Lerp(const Box& from, const Box& to, T interpolation)
	{
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
	*/
	template<typename T>
	constexpr Box<T> Box<T>::Invalid()
	{
		return Box(-1, -1, -1, -1, -1, -1);
	}

	/*!
	* \brief Shorthand for the box (0, 0, 0, 0, 0, 0)
	* \return A box with position (0, 0, 0) and lengths (0, 0, 0)
	*/
	template<typename T>
	constexpr Box<T> Box<T>::Zero()
	{
		return Box(Vector3<T>::Zero(), Vector3<T>::Zero());
	}

	/*!
	* \brief Serializes a Box
	* \return true if successfully serialized
	*
	* \param context Serialization context
	* \param box Input Box
	*/
	template<typename T>
	bool Serialize(SerializationContext& context, const Box<T>& box, TypeTag<Box<T>>)
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
	* \brief Deserializes a Box
	* \return true if successfully deserialized
	*
	* \param context Serialization context
	* \param box Output Box
	*/
	template<typename T>
	bool Deserialize(SerializationContext& context, Box<T>* box, TypeTag<Box<T>>)
	{
		if (!Deserialize(context, &box->x))
			return false;

		if (!Deserialize(context, &box->y))
			return false;

		if (!Deserialize(context, &box->z))
			return false;

		if (!Deserialize(context, &box->width))
			return false;

		if (!Deserialize(context, &box->height))
			return false;

		if (!Deserialize(context, &box->depth))
			return false;

		return true;
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
		return out << "Box(" << box.x << ", " << box.y << ", " << box.z << ", " << box.width << ", " << box.height << ", " << box.depth << ')';
	}
}

