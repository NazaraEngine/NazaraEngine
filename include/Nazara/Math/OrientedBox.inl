// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <NazaraUtils/MathUtils.hpp>
#include <cstring>
#include <sstream>

///DOC: Pour que les coins soient valides, la méthode Update doit être appelée

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
	* \brief Constructs a OrientedBox object from a box
	*
	* \param box Box<T> object
	*/
	template<typename T>
	constexpr OrientedBox<T>::OrientedBox(const Box<T>& box) :
	localBox(box)
	{
	}

	/*!
	* \brief Constructs a OrientedBox object from another type of OrientedBox
	*
	* \param orientedBox OrientedBox of type U to convert to type T
	*/
	template<typename T>
	template<typename U>
	constexpr OrientedBox<T>::OrientedBox(const OrientedBox<U>& orientedBox) :
	localBox(orientedBox.localBox)
	{
		for (auto&& [cornerEnum, corner] : m_corners.iter_kv())
			corner = Vector3<T>(orientedBox.GetCorner(cornerEnum));
	}

	template<typename T>
	constexpr bool OrientedBox<T>::ApproxEqual(const OrientedBox& obb, T maxDifference) const
	{
		if (!localBox.ApproxEqual(obb.localBox, maxDifference))
			return false;

		for (auto&& [cornerEnum, corner] : m_corners.iter_kv())
		{
			if (!corner.ApproxEqual(obb.GetCorner(cornerEnum), maxDifference))
				return false;
		}

		return true;
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
	constexpr const Vector3<T>& OrientedBox<T>::GetCorner(BoxCorner corner) const
	{
		NazaraAssertMsg(corner <= BoxCorner::Max, "invalid corner");
		return m_corners[corner];
	}

	template<typename T>
	constexpr const EnumArray<BoxCorner, Vector3<T>>& OrientedBox<T>::GetCorners() const
	{
		return m_corners;
	}

	/*!
	* \brief Checks whether this oriented box is valid
	* \return true if the oriented box has a strictly positive width, height and depth
	*/
	template<typename T>
	constexpr bool OrientedBox<T>::IsValid() const
	{
		return localBox.IsValid();
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object: "OrientedBox(...)"
	*/
	template<typename T>
	std::string OrientedBox<T>::ToString() const
	{
		std::ostringstream ss;
		ss << *this;

		return ss.str();
	}

	/*!
	* \brief Updates the corners of the box
	*
	* \param transformMatrix Matrix4 which represents the transformation to apply on the local box
	*/
	template<typename T>
	constexpr void OrientedBox<T>::Update(const Matrix4<T>& transformMatrix)
	{
		for (auto&& [corner, pos] : m_corners.iter_kv())
			pos = transformMatrix.Transform(localBox.GetCorner(corner));
	}

	/*!
	* \brief Updates the corners of the box
	*
	* \param translation Vector3 which represents the translation to apply on the local box
	*/
	template<typename T>
	constexpr void OrientedBox<T>::Update(const Vector3<T>& translation)
	{
		for (auto&& [corner, pos] : m_corners.iter_kv())
			pos = localBox.GetCorner(corner) + translation;
	}

	/*!
	* \brief Gets the ith corner of the oriented box
	* \return A reference to this corner
	*/
	template<typename T>
	constexpr Vector3<T>& OrientedBox<T>::operator()(unsigned int i)
	{
		NazaraAssertMsg(i < m_corners.size(), "corner out of range");
		return m_corners[static_cast<BoxCorner>(i)];
	}

	/*!
	* \brief Gets the ith corner of the oriented box
	* \return A reference to this corner
	*/
	template<typename T>
	constexpr const Vector3<T>& OrientedBox<T>::operator()(unsigned int i) const
	{
		NazaraAssertMsg(i < m_corners.size(), "corner out of range");
		return m_corners[static_cast<BoxCorner>(i)];
	}

	/*!
	* \brief Compares the oriented box to other one
	* \return true if the two oriented boxes are the same
	*
	* \param box Other oriented box to compare with
	*/
	template<typename T>
	constexpr bool OrientedBox<T>::operator==(const OrientedBox& box) const
	{
		return localBox == box.localBox && m_corners == box.m_corners;
	}

	/*!
	* \brief Compares the oriented box to other one
	* \return false if the two oriented boxes are the same
	*
	* \param box Other oriented box to compare with
	*/
	template<typename T>
	constexpr bool OrientedBox<T>::operator!=(const OrientedBox& box) const
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
	constexpr bool OrientedBox<T>::ApproxEqual(const OrientedBox& lhs, const OrientedBox& rhs, T maxDifference)
	{
		return lhs.ApproxEqual(rhs, maxDifference);
	}

	template<typename T>
	constexpr OrientedBox<T> OrientedBox<T>::Lerp(const OrientedBox& from, const OrientedBox& to, T interpolation)
	{
		return OrientedBox{ Box<T>::Lerp(from.localBox, to.localBox, interpolation) };
	}

	/*!
	* \brief Shorthand for the oriented box (0, 0, 0, 0, 0, 0)
	* \return A oriented box with position (0, 0, 0) and lengths (0, 0, 0)
	*/
	template<typename T>
	constexpr OrientedBox<T> OrientedBox<T>::Zero()
	{
		return OrientedBox{ Box<T>::Zero() };
	}

	/*!
	* \brief Serializes a OrientedBox
	* \return true if successfully serialized
	*
	* \param context Serialization context
	* \param obb Input oriented box
	*/
	template<typename T>
	bool Serialize(SerializationContext& context, const OrientedBox<T>& obb, TypeTag<OrientedBox<T>>)
	{
		if (!Serialize(context, obb.localBox))
			return false;

		for (auto&& corner : obb.m_corners)
		{
			if (!Serialize(context, corner))
				return false;
		}

		return true;
	}

	/*!
	* \brief Deserializes a Matrix4
	* \return true if successfully deserialized
	*
	* \param context Serialization context
	* \param obb Output oriented box
	*/
	template<typename T>
	bool Deserialize(SerializationContext& context, OrientedBox<T>* obb, TypeTag<OrientedBox<T>>)
	{
		if (!Deserialize(context, &obb->localBox))
			return false;

		for (auto&& corner : obb->m_corners)
		{
			if (!Deserialize(context, &corner))
				return false;
		}

		return true;
	}

	/*!
	* \brief Output operator
	* \return The stream
	*
	* \param out The stream
	* \param orientedBox The orientedBox to output
	*/
	template<typename T>
	std::ostream& operator<<(std::ostream& out, const OrientedBox<T>& orientedBox)
	{
		return out << "OrientedBox(FLB: " << orientedBox.GetCorner(BoxCorner::LeftBottomFar) << ",\n"
				   << "            FLT: " << orientedBox.GetCorner(BoxCorner::LeftTopFar)      << ",\n"
				   << "            FRB: " << orientedBox.GetCorner(BoxCorner::RightBottomFar)  << ",\n"
				   << "            FRT: " << orientedBox.GetCorner(BoxCorner::RightTopFar)     << ",\n"
				   << "            NLB: " << orientedBox.GetCorner(BoxCorner::LeftBottomNear)  << ",\n"
				   << "            NLT: " << orientedBox.GetCorner(BoxCorner::LeftTopNear)     << ",\n"
				   << "            NRB: " << orientedBox.GetCorner(BoxCorner::RightBottomNear) << ",\n"
				   << "            NRT: " << orientedBox.GetCorner(BoxCorner::RightTopNear)    << ")\n";
	}
}

