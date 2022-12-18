// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Math/BoundingVolume.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <algorithm>
#include <cstring>
#include <sstream>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{

	/*!
	* \ingroup math
	* \class Nz::BoundingVolume
	* \brief Math class that represents a bounding volume, a combination of a box and an oriented box
	*
	* \remark You need to call Update not to have undefined behaviour
	*/

	/*!
	* \brief Constructs a BoundingVolume object by default
	*
	* \remark extend is set to Extend::Null, aabb and obb are uninitialized
	*/

	template<typename T>
	BoundingVolume<T>::BoundingVolume() :
	extend(Extend::Null)
	{
	}

	/*!
	* \brief Constructs a BoundingVolume object from Extend
	* \param Extend Extend of the volume part of enumeration Extend
	*
	* \remark Aabb and obb are uninitialized
	*/

	template<typename T>
	BoundingVolume<T>::BoundingVolume(Extend Extend) :
	extend(Extend)
	{
	}

	/*!
	* \brief Constructs a BoundingVolume object from a box
	*
	* \param box Box<T> object
	*
	* \remark Aabb is uninitialized
	*/

	template<typename T>
	BoundingVolume<T>::BoundingVolume(const Box<T>& box) :
	extend(Extend::Finite),
	obb(box)
	{
	}

	/*!
	* \brief Constructs a BoundingVolume object from an oriented box
	*
	* \param orientedBox OrientedBox<T> object
	*
	* \remark Aabb is uninitialized
	*/

	template<typename T>
	BoundingVolume<T>::BoundingVolume(const OrientedBox<T>& orientedBox) :
	extend(Extend::Finite),
	obb(orientedBox)
	{
	}

	/*!
	* \brief Constructs a BoundingVolume object from another type of BoundingVolume
	*
	* \param volume BoundingVolume of type U to convert to type T
	*/

	template<typename T>
	template<typename U>
	BoundingVolume<T>::BoundingVolume(const BoundingVolume<U>& volume) :
	extend(volume.extend),
	aabb(volume.aabb),
	obb(volume.obb)
	{
	}

	/*!
	* \brief Extends the bounding volume to contain another bounding volume
	* \return A reference to the the bounding volume
	*
	* \param volume Other volume to contain
	*
	* \remark Extending to a null bounding volume has no effect while extending to a infinite bounding volume will set it as infinite
	*/
	template<typename T>
	BoundingVolume<T>& BoundingVolume<T>::ExtendTo(const BoundingVolume& volume)
	{
		switch (extend)
		{
			case Extend::Finite:
			{
				switch (volume.extend)
				{
					case Extend::Finite:
					{
						// Extend the OBB local box
						obb.localBox.ExtendTo(volume.obb.localBox);
						break;
					}

					case Extend::Infinite:
						extend = Extend::Infinite;
						break;

					case Extend::Null:
						break;
				}
				break;
			}

			case Extend::Infinite:
				break; //< We already contain the bounding volume

			case Extend::Null:
				Set(volume);
				break;
		}

		return *this;
	}

	template<typename T>
	bool BoundingVolume<T>::Intersect(const Box<T>& box) const
	{
		switch (extend)
		{
			case Extend::Infinite:
				return true;

			case Extend::Finite:
				return aabb.Intersect(box);

			case Extend::Null:
				return false;
		}

		return false;
	}

	/*!
	* \brief Checks whether the volume is finite
	* \return true if extend is Extend::Finite
	*/
	template<typename T>
	bool BoundingVolume<T>::IsFinite() const
	{
		return extend == Extend::Finite;
	}

	/*!
	* \brief Checks whether the volume is infinite
	* \return true if extend is Extend::Infinite
	*/

	template<typename T>
	bool BoundingVolume<T>::IsInfinite() const
	{
		return extend == Extend::Infinite;
	}

	/*!
	* \brief Checks whether the volume is null
	* \return true if extend is Extend::Null
	*/

	template<typename T>
	bool BoundingVolume<T>::IsNull() const
	{
		return extend == Extend::Null;
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object: "BoundingVolume(localBox="")" if finite, or "BoundingVolume(Infinite)" or "BoundingVolume(Null)"
	*
	* \remark If enumeration is not defined in Extend, a NazaraError is thrown and "BoundingVolume(ERROR)" is returned
	*/

	template<typename T>
	std::string BoundingVolume<T>::ToString() const
	{
		std::ostringstream ss;
		ss << *this;

		return ss.str();
	}

	/*!
	* \brief Updates the obb and the aabb of the bounding volume
	*
	* \param transformMatrix Matrix4 which represents the transformation to apply
	*/

	template<typename T>
	void BoundingVolume<T>::Update(const Matrix4<T>& transformMatrix)
	{
		obb.Update(transformMatrix);

		aabb = Boxf::FromExtends(obb(0), obb(1));
		for (unsigned int i = 2; i < 8; ++i)
			aabb.ExtendTo(obb(i));
	}

	/*!
	* \brief Updates the obb and the aabb of the bounding volume
	*
	* \param translation Vector3 which represents the translation to apply
	*/

	template<typename T>
	void BoundingVolume<T>::Update(const Vector3<T>& translation)
	{
		obb.Update(translation);

		aabb = Boxf::FromExtends(obb(0), obb(1));
		for (unsigned int i = 2; i < 8; ++i)
			aabb.ExtendTo(obb(i));
	}

	/*!
	* \brief Multiplies the lengths of the obb with the scalar
	* \return A BoundingVolume where the position is the same and width, height and depth are the product of the old width, height and depth and the scalar
	*
	* \param scale The scalar to multiply width, height and depth with
	*/

	template<typename T>
	BoundingVolume<T> BoundingVolume<T>::operator*(T scalar) const
	{
		BoundingVolume volume(*this);
		volume *= scalar;

		return volume;
	}

	/*!
	* \brief Multiplies the lengths of this bounding volume with the scalar
	* \return A reference to this bounding volume where lengths are the product of these lengths and the scalar
	*
	* \param scalar The scalar to multiply width, height and depth with
	*/

	template<typename T>
	BoundingVolume<T>& BoundingVolume<T>::operator*=(T scalar)
	{
		obb *= scalar;

		return *this;
	}

	/*!
	* \brief Compares the bounding volume to other one
	* \return true if the two bounding volumes are the same
	*
	* \param volume Other bounding volume to compare with
	*/

	template<typename T>
	bool BoundingVolume<T>::operator==(const BoundingVolume& volume) const
	{
		if (extend == volume.extend)
			if (extend == Extend::Finite)
				return obb == volume.obb;
			else
				return true;
		else
			return false;
	}

	/*!
	* \brief Compares the bounding volume to other one
	* \return false if the two bounding volumes are the same
	*
	* \param volume Other bounding volume to compare with
	*/

	template<typename T>
	bool BoundingVolume<T>::operator!=(const BoundingVolume& volume) const
	{
		return !operator==(volume);
	}

	/*!
	* \brief Shorthand for the bounding volume (Extend::Infinite)
	* \return A bounding volume with Extend::Infinite
	*
	* \see MakeInfinite
	*/

	template<typename T>
	BoundingVolume<T> BoundingVolume<T>::Infinite()
	{
		BoundingVolume volume;
		volume.extend = Extend::Infinite;

		return volume;
	}

	/*!
	* \brief Interpolates the bounding volume to other one with a factor of interpolation
	* \return A new bounding volume box which is the interpolation of two bounding volumes
	*
	* \param from Initial bounding volume
	* \param to Target bounding volume
	* \param interpolation Factor of interpolation
	*
	* \remark interpolation is meant to be between 0 and 1, other values are potentially undefined behavior
	* \remark With NAZARA_DEBUG, a NazaraError is thrown and Null() is returned
	* \remark If enumeration is not defined in Extend, a NazaraError is thrown and Null() is returned
	*
	* \see Lerp
	*/

	template<typename T>
	BoundingVolume<T> BoundingVolume<T>::Lerp(const BoundingVolume& from, const BoundingVolume& to, T interpolation)
	{
		if (NumberEquals(interpolation, T(0.0)))
			return from;

		if (NumberEquals(interpolation, T(1.0)))
			return to;

		switch (to.extend)
		{
			case Extend::Finite:
			{
				switch (from.extend)
				{
					case Extend::Finite:
						return BoundingVolume{ OrientedBox<T>::Lerp(from.obb, to.obb, interpolation) };

					case Extend::Infinite:
						return Infinite();

					case Extend::Null:
					{
						Box<T> destBox = to.obb.localBox;
						destBox.Scale(interpolation);

						return { destBox };
					}
				}

				// If we arrive here, the extend is invalid
				NazaraError("Invalid extend type (From) (0x" + NumberToString(UnderlyingCast(from.extend), 16) + ')');
				return Null();
			}

			case Extend::Infinite:
				return Infinite(); // A little bit of infinity is already too much ;)

			case Extend::Null:
			{
				switch (from.extend)
				{
					case Extend::Finite:
					{
						Box<T> fromBox = from.obb.localBox;
						fromBox.Scale(T(1.0) - interpolation);

						return { fromBox };
					}

					case Extend::Infinite:
						return Infinite();

					case Extend::Null:
						return Null();
				}

				// If we arrive here, the extend is invalid
				NazaraError("Invalid extend type (From) (0x" + NumberToString(UnderlyingCast(from.extend), 16) + ')');
				return Null();
			}
		}

		// If we arrive here, the extend is invalid
		NazaraError("Invalid extend type (To) (0x" + NumberToString(UnderlyingCast(to.extend), 16) + ')');
		return Null();
	}

	/*!
	* \brief Shorthand for the bounding volume (Extend::Null)
	* \return A bounding volume with Extend::Null
	*
	* \see MakeNull
	*/

	template<typename T>
	BoundingVolume<T> BoundingVolume<T>::Null()
	{
		BoundingVolume volume;
		volume.extend = Extend::Null;

		return volume;
	}

	/*!
	* \brief Serializes a BoundingVolume
	* \return true if successfully serialized
	*
	* \param context Serialization context
	* \param boundingVolume Input bounding volume
	*
	* \remark Does not save OBB corners
	*/
	template<typename T>
	bool Serialize(SerializationContext& context, const BoundingVolume<T>& boundingVolume, TypeTag<BoundingVolume<T>>)
	{
		if (!Serialize(context, static_cast<UInt8>(boundingVolume.extend)))
			return false;

		if (!Serialize(context, boundingVolume.aabb))
			return false;

		if (!Serialize(context, boundingVolume.obb))
			return false;

		return true;
	}

	/*!
	* \brief Unserializes a BoundingVolume
	* \return true if successfully unserialized
	*
	* \param context Serialization context
	* \param boundingVolume Output bounding volume
	*
	* \remark The resulting oriented box corners will *not* be updated, a call to Update is required
	*/
	template<typename T>
	bool Unserialize(SerializationContext& context, BoundingVolume<T>* boundingVolume, TypeTag<BoundingVolume<T>>)
	{
		UInt8 extend;
		if (!Unserialize(context, &extend))
			return false;

		if (extend > UnderlyingCast(Extend::Max))
			return false;

		boundingVolume->extend = static_cast<Extend>(extend);

		if (!Unserialize(context, &boundingVolume->aabb))
			return false;

		if (!Unserialize(context, &boundingVolume->obb))
			return false;

		return true;
	}
}

/*!
* \brief Output operator
* \return The stream
*
* \param out The stream
* \param volume The bounding volume to output
*/

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nz::BoundingVolume<T>& volume)
{
	switch (volume.extend)
	{
		case Nz::Extend::Finite:
			out << "BoundingVolume(localBox=" << volume.obb.localBox << ')';
			break;

		case Nz::Extend::Infinite:
			out << "BoundingVolume(Infinite)";
			break;

		case Nz::Extend::Null:
			out << "BoundingVolume(Null)";
			break;
	}

	return out;
}

#include <Nazara/Core/DebugOff.hpp>
