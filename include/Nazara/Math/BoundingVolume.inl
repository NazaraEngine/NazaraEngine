// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Error.hpp>
#include <NazaraUtils/MathUtils.hpp>
#include <algorithm>
#include <cstring>
#include <sstream>

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
	* \remark extent is set to Extent::Null, aabb and obb are uninitialized
	*/
	template<typename T>
	constexpr BoundingVolume<T>::BoundingVolume() :
	extent(Extent::Null)
	{
	}

	/*!
	* \brief Constructs a BoundingVolume object from Extent
	* \param Extent Extent of the volume part of enumeration Extent
	*
	* \remark Aabb and obb are uninitialized
	*/
	template<typename T>
	constexpr BoundingVolume<T>::BoundingVolume(Extent Extend) :
	extent(Extend)
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
	constexpr BoundingVolume<T>::BoundingVolume(const Box<T>& box) :
	extent(Extent::Finite),
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
	constexpr BoundingVolume<T>::BoundingVolume(const OrientedBox<T>& orientedBox) :
	extent(Extent::Finite),
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
	constexpr BoundingVolume<T>::BoundingVolume(const BoundingVolume<U>& volume) :
	extent(volume.extent),
	aabb(volume.aabb),
	obb(volume.obb)
	{
	}

	template<typename T>
	constexpr bool BoundingVolume<T>::ApproxEqual(const BoundingVolume& volume, T maxDifference) const
	{
		if (extent != volume.extent)
			return false;

		if (extent != Extent::Finite)
			return true;

		return aabb.ApproxEqual(volume.aabb, maxDifference) && obb.ApproxEqual(volume.obb, maxDifference);
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
	constexpr BoundingVolume<T>& BoundingVolume<T>::ExtendTo(const BoundingVolume& volume)
	{
		switch (extent)
		{
			case Extent::Finite:
			{
				switch (volume.extent)
				{
					case Extent::Finite:
					{
						// Extent the OBB local box
						obb.localBox.ExtendTo(volume.obb.localBox);
						break;
					}

					case Extent::Infinite:
						extent = Extent::Infinite;
						break;

					case Extent::Null:
						break;
				}
				break;
			}

			case Extent::Infinite:
				break; //< We already contain the bounding volume

			case Extent::Null:
				Set(volume);
				break;
		}

		return *this;
	}

	template<typename T>
	constexpr bool BoundingVolume<T>::Intersect(const Box<T>& box) const
	{
		switch (extent)
		{
			case Extent::Infinite:
				return true;

			case Extent::Finite:
				return aabb.Intersect(box);

			case Extent::Null:
				return false;
		}

		return false;
	}

	/*!
	* \brief Checks whether the volume is finite
	* \return true if extent is Extent::Finite
	*/
	template<typename T>
	constexpr bool BoundingVolume<T>::IsFinite() const
	{
		return extent == Extent::Finite;
	}

	/*!
	* \brief Checks whether the volume is infinite
	* \return true if extent is Extent::Infinite
	*/

	template<typename T>
	constexpr bool BoundingVolume<T>::IsInfinite() const
	{
		return extent == Extent::Infinite;
	}

	/*!
	* \brief Checks whether the volume is null
	* \return true if extent is Extent::Null
	*/

	template<typename T>
	constexpr bool BoundingVolume<T>::IsNull() const
	{
		return extent == Extent::Null;
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object: "BoundingVolume(localBox="")" if finite, or "BoundingVolume(Infinite)" or "BoundingVolume(Null)"
	*
	* \remark If enumeration is not defined in Extent, a NazaraError is thrown and "BoundingVolume(ERROR)" is returned
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
	constexpr void BoundingVolume<T>::Update(const Matrix4<T>& transformMatrix)
	{
		obb.Update(transformMatrix);

		aabb = Box<T>::FromExtents(obb(0), obb(1));
		for (unsigned int i = 2; i < 8; ++i)
			aabb.ExtendTo(obb(i));
	}

	/*!
	* \brief Updates the obb and the aabb of the bounding volume
	*
	* \param translation Vector3 which represents the translation to apply
	*/
	template<typename T>
	constexpr void BoundingVolume<T>::Update(const Vector3<T>& translation)
	{
		obb.Update(translation);

		aabb = Box<T>::FromExtents(obb(0), obb(1));
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
	constexpr BoundingVolume<T> BoundingVolume<T>::operator*(T scalar) const
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
	constexpr BoundingVolume<T>& BoundingVolume<T>::operator*=(T scalar)
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
	constexpr bool BoundingVolume<T>::operator==(const BoundingVolume& volume) const
	{
		if (extent != volume.extent)
			return false;

		if (extent == Extent::Finite)
		{
			if (aabb != volume.aabb)
				return false;

			if (obb != volume.obb)
				return false;
		}

		return true;
	}

	/*!
	* \brief Compares the bounding volume to other one
	* \return false if the two bounding volumes are the same
	*
	* \param volume Other bounding volume to compare with
	*/
	template<typename T>
	constexpr bool BoundingVolume<T>::operator!=(const BoundingVolume& volume) const
	{
		return !operator==(volume);
	}

	template<typename T>
	constexpr bool BoundingVolume<T>::ApproxEqual(const BoundingVolume& lhs, const BoundingVolume& rhs, T maxDifference)
	{
		return lhs.ApproxEqual(rhs, maxDifference);
	}

	/*!
	* \brief Shorthand for the bounding volume (Extent::Infinite)
	* \return A bounding volume with Extent::Infinite
	*/
	template<typename T>
	constexpr BoundingVolume<T> BoundingVolume<T>::Infinite()
	{
		return BoundingVolume(Extent::Infinite);
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
	* \remark If enumeration is not defined in Extent, a NazaraError is thrown and Null() is returned
	*
	* \see Lerp
	*/
	template<typename T>
	constexpr BoundingVolume<T> BoundingVolume<T>::Lerp(const BoundingVolume& from, const BoundingVolume& to, T interpolation)
	{
		if (NumberEquals(interpolation, T(0.0)))
			return from;

		if (NumberEquals(interpolation, T(1.0)))
			return to;

		switch (to.extent)
		{
			case Extent::Finite:
			{
				switch (from.extent)
				{
					case Extent::Finite:
						return BoundingVolume{ OrientedBox<T>::Lerp(from.obb, to.obb, interpolation) };

					case Extent::Infinite:
						return Infinite();

					case Extent::Null:
					{
						Box<T> destBox = to.obb.localBox;
						destBox.Scale(interpolation);

						return BoundingVolume{ destBox };
					}
				}

				// If we arrive here, the extent is invalid
				NazaraErrorFmt("invalid extent type (From) ({0:#x})", UnderlyingCast(from.extent));
				return Null();
			}

			case Extent::Infinite:
				return Infinite(); // A little bit of infinity is already too much ;)

			case Extent::Null:
			{
				switch (from.extent)
				{
					case Extent::Finite:
					{
						Box<T> fromBox = from.obb.localBox;
						fromBox.Scale(T(1.0) - interpolation);

						return BoundingVolume{ fromBox };
					}

					case Extent::Infinite:
						return Infinite();

					case Extent::Null:
						return Null();
				}

				// If we arrive here, the extent is invalid
				NazaraErrorFmt("invalid extent type (From) ({0:#x})", UnderlyingCast(from.extent));
				return Null();
			}
		}

		// If we arrive here, the extent is invalid
		NazaraErrorFmt("invalid extent type (To) ({0:#x})", UnderlyingCast(to.extent));
		return Null();
	}

	/*!
	* \brief Shorthand for the bounding volume (Extent::Null)
	* \return A bounding volume with Extent::Null
	*/
	template<typename T>
	constexpr BoundingVolume<T> BoundingVolume<T>::Null()
	{
		return BoundingVolume(Extent::Null);
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
		if (!Serialize(context, static_cast<UInt8>(boundingVolume.extent)))
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

		if (extend > UnderlyingCast(Extent::Max))
			return false;

		boundingVolume->extent = static_cast<Extent>(extend);

		if (!Unserialize(context, &boundingVolume->aabb))
			return false;

		if (!Unserialize(context, &boundingVolume->obb))
			return false;

		return true;
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
		switch (volume.extent)
		{
			case Nz::Extent::Finite:
				out << "BoundingVolume(localBox=" << volume.obb.localBox << ')';
				break;

			case Nz::Extent::Infinite:
				out << "BoundingVolume(Infinite)";
				break;

			case Nz::Extent::Null:
				out << "BoundingVolume(Null)";
				break;
		}

		return out;
	}
}

