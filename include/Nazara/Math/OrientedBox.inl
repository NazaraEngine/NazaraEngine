// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

///DOC: Pour que les coins soient valides, la méthode Update doit être appelée

#define F(a) static_cast<T>(a)

namespace Nz
{
	/*!
    * \ingroup math
	* \class Nz::OrientedBox
	* \brief Math class that represents an oriented three dimensional box
	*
	* \remark You need to call Update not to have undefined behaviour
	*/

	/*!
	* \brief Constructs a OrientedBox object from its position and sizes
	*
	* \param X X component of position
	* \param Y Y component of position
	* \param Z Z component of position
	* \param Width Width of the box (following X)
	* \param Height Height of the box (following Y)
	* \param Depth Depth of the box (following Z)
	*/

	template<typename T>
	OrientedBox<T>::OrientedBox(T X, T Y, T Z, T Width, T Height, T Depth)
	{
		Set(X, Y, Z, Width, Height, Depth);
	}

	/*!
	* \brief Constructs a OrientedBox object from a box
	*
	* \param box Box<T> object
	*/

	template<typename T>
	OrientedBox<T>::OrientedBox(const Box<T>& box)
	{
		Set(box);
	}

	/*!
	* \brief Constructs a OrientedBox object from two vectors representing point of the space
	* (X, Y, Z) will be the components minimum of the two vectors and the (width, height, depth) will be the components maximum - minimum
	*
	* \param vec1 First point
	* \param vec2 Second point
	*/

	template<typename T>
	OrientedBox<T>::OrientedBox(const Vector3<T>& vec1, const Vector3<T>& vec2)
	{
		Set(vec1, vec2);
	}

	/*!
	* \brief Constructs a OrientedBox object from another type of OrientedBox
	*
	* \param orientedBox OrientedBox of type U to convert to type T
	*/

	template<typename T>
	template<typename U>
	OrientedBox<T>::OrientedBox(const OrientedBox<U>& orientedBox)
	{
		Set(orientedBox);
	}

	/*!
	* \brief Gets the Vector3 for the corner
	* \return The position of the corner of the oriented box according to enum BoxCorner
	*
	* \param corner Enumeration of type BoxCorner
	*
	* \remark If enumeration is not defined in BoxCorner, a NazaraError is thrown and a Vector3 uninitialised is returned
	*/

	template<typename T>
	const Vector3<T>& OrientedBox<T>::GetCorner(BoxCorner corner) const
	{
		#ifdef NAZARA_DEBUG
		if (corner > BoxCorner_Max)
		{
			NazaraError("Corner not handled (0x" + String::Number(corner, 16) + ')');

			static Vector3<T> dummy;
			return dummy;
		}
		#endif

		return m_corners[corner];
	}

	/*!
	* \brief Checks whether this oriented box is valid
	* \return true if the oriented box has a strictly positive width, height and depth
	*/

	template<typename T>
	bool OrientedBox<T>::IsValid() const
	{
		return localBox.IsValid();
	}

	/*!
	* \brief Makes the oriented box position (0, 0, 0) and lengths (0, 0, 0)
	* \return A reference to this oriented box with position (0, 0, 0) and lengths (0, 0, 0)
	*
	* \see Zero
	*/

	template<typename T>
	OrientedBox<T>& OrientedBox<T>::MakeZero()
	{
		localBox.MakeZero();

		return *this;
	}

	/*!
	* \brief Sets the components of the oriented box
	* \return A reference to this oriented box
	*
	* \param X X position
	* \param Y Y position
	* \param Z Z position
	* \param Width Width of the oriented box (following X)
	* \param Height Height of the oriented box (following Y)
	* \param Depth Depth of the oriented box (following Z)
	*/

	template<typename T>
	OrientedBox<T>& OrientedBox<T>::Set(T X, T Y, T Z, T Width, T Height, T Depth)
	{
		localBox.Set(X, Y, Z, Width, Height, Depth);

		return *this;
	}

	/*!
	* \brief Sets the components of the oriented box from a box
	* \return A reference to this oriented box
	*
	* \param box Box<T> object
	*/

	template<typename T>
	OrientedBox<T>& OrientedBox<T>::Set(const Box<T>& box)
	{
		localBox.Set(box);

		return *this;
	}

	/*!
	* \brief Sets the components of the oriented box with components from another
	* \return A reference to this oriented box
	*
	* \param orientedBox The other OrientedBox
	*/

	template<typename T>
	OrientedBox<T>& OrientedBox<T>::Set(const OrientedBox& orientedBox)
	{
		std::memcpy(this, &orientedBox, sizeof(OrientedBox));

		return *this;
	}

	/*!
	* \brief Sets a OrientedBox object from two vectors representing point of the space
	* (X, Y, Z) will be the components minimum of the two vectors and the (width, height, depth) will be the components maximum - minimum
	*
	* \param vec1 First point
	* \param vec2 Second point
	*/

	template<typename T>
	OrientedBox<T>& OrientedBox<T>::Set(const Vector3<T>& vec1, const Vector3<T>& vec2)
	{
		localBox.Set(vec1, vec2);

		return *this;
	}

	/*!
	* \brief Sets the components of the orientedBox from another type of OrientedBox
	* \return A reference to this orientedBox
	*
	* \param orientedBox OrientedBox of type U to convert its components
	*/

	template<typename T>
	template<typename U>
	OrientedBox<T>& OrientedBox<T>::Set(const OrientedBox<U>& orientedBox)
	{
		for (unsigned int i = 0; i <= BoxCorner_Max; ++i)
			m_corners[i].Set(orientedBox(i));

		localBox.Set(orientedBox.localBox);

		return *this;
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object: "OrientedBox(...)"
	*/

	template<typename T>
	String OrientedBox<T>::ToString() const
	{
		StringStream ss;

		return ss << "OrientedBox(FLB: " << m_corners[BoxCorner_FarLeftBottom].ToString() << "\n"
		       << "            FLT: " << m_corners[BoxCorner_FarLeftTop].ToString() << "\n"
		       << "            FRB: " << m_corners[BoxCorner_FarRightBottom].ToString() << "\n"
		       << "            FRT: " << m_corners[BoxCorner_FarRightTop].ToString() << "\n"
		       << "            NLB: " << m_corners[BoxCorner_NearLeftBottom].ToString() << "\n"
		       << "            NLT: " << m_corners[BoxCorner_NearLeftTop].ToString() << "\n"
		       << "            NRB: " << m_corners[BoxCorner_NearRightBottom].ToString() << "\n"
		       << "            NRT: " << m_corners[BoxCorner_NearRightTop].ToString() << ")\n";
	}

	/*!
	* \brief Updates the corners of the box
	*
	* \param transformMatrix Matrix4 which represents the transformation to apply on the local box
	*/

	template<typename T>
	void OrientedBox<T>::Update(const Matrix4<T>& transformMatrix)
	{
		for (unsigned int i = 0; i <= BoxCorner_Max; ++i)
			m_corners[i] = transformMatrix.Transform(localBox.GetCorner(static_cast<BoxCorner>(i)));
	}

	/*!
	* \brief Updates the corners of the box
	*
	* \param translation Vector3 which represents the translation to apply on the local box
	*/

	template<typename T>
	void OrientedBox<T>::Update(const Vector3<T>& translation)
	{
		for (unsigned int i = 0; i <= BoxCorner_Max; ++i)
			m_corners[i] = localBox.GetCorner(static_cast<BoxCorner>(i)) + translation;
	}

	/*!
	* \brief Converts oriented box to pointer of Vector3 to its own corners
	* \return A pointer to the own corners
	*
	* \remark Access to index greather than BoxCorner_Max is undefined behavior
	*/

	template<typename T>
	OrientedBox<T>::operator Vector3<T>* ()
	{
		return &m_corners[0];
	}

	/*!
	* \brief Converts oriented box to pointer of Vector3 to its own corners
	* \return A const pointer to the own corners
	*
	* \remark Access to index greather than BoxCorner_Max is undefined behavior
	*/

	template<typename T>
	OrientedBox<T>::operator const Vector3<T>* () const
	{
		return &m_corners[0];
	}

	/*!
	* \brief Gets the ith corner of the oriented box
	* \return A reference to this corner
	*
	* \remark Produce a NazaraError if you try to access to index greather than BoxCorner_Max with NAZARA_MATH_SAFE defined. If not, it is undefined behaviour
	* \throw std::out_of_range if NAZARA_MATH_SAFE is defined and you try to acces to index greather than BoxCorner_Max
	*/

	template<typename T>
	Vector3<T>& OrientedBox<T>::operator()(unsigned int i)
	{
		#if NAZARA_MATH_SAFE
		if (i > BoxCorner_Max)
		{
			StringStream ss;
			ss << "Index out of range: (" << i << " >= " << BoxCorner_Max << ")";

			NazaraError(ss);
			throw std::out_of_range(ss.ToString());
		}
		#endif

		return m_corners[i];
	}

	/*!
	* \brief Gets the ith corner of the oriented box
	* \return A reference to this corner
	*
	* \remark Produce a NazaraError if you try to access to index greather than BoxCorner_Max with NAZARA_MATH_SAFE defined. If not, it is undefined behaviour
	* \throw std::out_of_range if NAZARA_MATH_SAFE is defined and you try to acces to index greather than BoxCorner_Max
	*/

	template<typename T>
	Vector3<T> OrientedBox<T>::operator()(unsigned int i) const
	{
		#if NAZARA_MATH_SAFE
		if (i > BoxCorner_Max)
		{
			StringStream ss;
			ss << "Index out of range: (" << i << " >= " << BoxCorner_Max << ")";

			NazaraError(ss);
			throw std::out_of_range(ss.ToString());
		}
		#endif

		return m_corners[i];
	}

	/*!
	* \brief Multiplies the lengths with the scalar
	* \return A OrientedBox where the position is the same and width, height and depth are the product of the old width, height and depth and the scalar
	*
	* \param scalar The scalar to multiply width, height and depth with
	*/

	template<typename T>
	OrientedBox<T> OrientedBox<T>::operator*(T scalar) const
	{
		OrientedBox box(*this);
		box *= scalar;

		return box;
	}

	/*!
	* \brief Multiplies the lengths of this oriented box with the scalar
	* \return A reference to this oriented box where lengths are the product of these lengths and the scalar
	*
	* \param scalar The scalar to multiply width, height and depth with
	*/

	template<typename T>
	OrientedBox<T>& OrientedBox<T>::operator*=(T scalar)
	{
		localBox *= scalar;

		return *this;
	}

	/*!
	* \brief Compares the oriented box to other one
	* \return true if the two oriented boxes are the same
	*
	* \param box Other oriented box to compare with
	*/

	template<typename T>
	bool OrientedBox<T>::operator==(const OrientedBox& box) const
	{
		return localBox == box.localBox;
	}

	/*!
	* \brief Compares the oriented box to other one
	* \return false if the two oriented boxes are the same
	*
	* \param box Other oriented box to compare with
	*/

	template<typename T>
	bool OrientedBox<T>::operator!=(const OrientedBox& box) const
	{
		return !operator==(box);
	}

	/*!
	* \brief Interpolates the oriented box to other one with a factor of interpolation
	* \return A new oriented box which is the interpolation of two oriented boxes
	*
	* \param from Initial oriented box
	* \param to Target oriented box
	* \param interpolation Factor of interpolation
	*
	* \remark interpolation is meant to be between 0 and 1, other values are potentially undefined behavior
	* \remark With NAZARA_DEBUG, a NazaraError is thrown and Zero() is returned
	*
	* \see Lerp
	*/

	template<typename T>
	OrientedBox<T> OrientedBox<T>::Lerp(const OrientedBox& from, const OrientedBox& to, T interpolation)
	{
		OrientedBox orientedBox;
		orientedBox.Set(Box<T>::Lerp(from.localBox, to.localBox, interpolation));

		return orientedBox;
	}

	/*!
	* \brief Shorthand for the oriented box (0, 0, 0, 0, 0, 0)
	* \return A oriented box with position (0, 0, 0) and lengths (0, 0, 0)
	*
	* \see MakeZero
	*/

	template<typename T>
	OrientedBox<T> OrientedBox<T>::Zero()
	{
		OrientedBox orientedBox;
		orientedBox.MakeZero();

		return orientedBox;
	}

	/*!
	* \brief Serializes a OrientedBox
	* \return true if successfully serialized
	*
	* \param context Serialization context
	* \param obb Input oriented box
	*
	* \remark Does not save OBB corners
	*/
	template<typename T>
	bool Serialize(SerializationContext& context, const OrientedBox<T>& obb)
	{
		if (!Serialize(context, obb.localBox))
			return false;

		return true;
	}

	/*!
	* \brief Unserializes a Matrix4
	* \return true if successfully unserialized
	*
	* \param context Serialization context
	* \param obb Output oriented box
	*
	* \remark The resulting oriented box corners will *not* be updated, a call to Update is required
	*/
	template<typename T>
	bool Unserialize(SerializationContext& context, OrientedBox<T>* obb)
	{
		if (!Unserialize(context, &obb->localBox))
			return false;

		return true;
	}
}

/*!
* \brief Output operator
* \return The stream
*
* \param out The stream
* \param orientedBox The orientedBox to output
*/

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nz::OrientedBox<T>& orientedBox)
{
	return out << orientedBox.ToString();
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
