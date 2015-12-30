// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <algorithm>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)

namespace Nz
{

	/*!
	* \class Nz::BoundingVolume<T>
	* \brief Math class that represents a bounding volume, a combination of a box and an oriented box
	*
	* \remark You need to call Update not to have undefined behaviour
	*/

	/*!
	* \brief Constructs a BoundingVolume<T> object by default
	*
	* \remark extend is set to Extend_Null, aabb and obb are uninitialized
	*/

	template<typename T>
	BoundingVolume<T>::BoundingVolume() :
	extend(Extend_Null)
	{
	}

	/*!
	* \brief Constructs a BoundingVolume<T> object from Extend
	* \param Extend Extend of the volume part of enumeration Extend
	*
	* \remark Aabb and obb are uninitialized
	*/

	template<typename T>
	BoundingVolume<T>::BoundingVolume(Extend Extend)
	{
		Set(Extend);
	}

	/*!
	* \brief Constructs a BoundingVolume<T> object from its position and sizes
	*
	* \param X X component of position
	* \param Y Y component of position
	* \param Z Z component of position
	* \param Width Width of the box (following X)
	* \param Height Height of the box (following Y)
	* \param Depth Depth of the box (following Z)
	*
	* \remark Aabb is uninitialized
	*/

	template<typename T>
	BoundingVolume<T>::BoundingVolume(T X, T Y, T Z, T Width, T Height, T Depth)
	{
		Set(X, Y, Z, Width, Height, Depth);
	}

	/*!
	* \brief Constructs a BoundingVolume<T> object from a box
	*
	* \param box Box<T> object
	*
	* \remark Aabb is uninitialized
	*/

	template<typename T>
	BoundingVolume<T>::BoundingVolume(const Box<T>& box)
	{
		Set(box);
	}

	/*!
	* \brief Constructs a BoundingVolume<T> object from an oriented box
	*
	* \param orientedBox OrientedBox<T> object
	*
	* \remark Aabb is uninitialized
	*/

	template<typename T>
	BoundingVolume<T>::BoundingVolume(const OrientedBox<T>& orientedBox)
	{
		Set(orientedBox);
	}

	/*!
	* \brief Constructs a BoundingVolume<T> object from two vectors representing point of the space
	* (X, Y, Z) will be the components minimum of the two vectors and the (width, height, depth) will be the components maximum - minimum
	*
	* \param vec1 First point
	* \param vec2 Second point
	*
	* \remark Aabb is uninitialized
	*/

	template<typename T>
	BoundingVolume<T>::BoundingVolume(const Vector3<T>& vec1, const Vector3<T>& vec2)
	{
		Set(vec1, vec2);
	}

	/*!
	* \brief Constructs a BoundingVolume<T> object from another type of BoundingVolume
	*
	* \param volume BoundingVolume of type U to convert to type T
	*/

	template<typename T>
	template<typename U>
	BoundingVolume<T>::BoundingVolume(const BoundingVolume<U>& volume)
	{
		Set(volume);
	}

	/*!
	* \brief Checks whether the volume is finite
	* \return true if extend is Extend_Finite
	*/

	template<typename T>
	bool BoundingVolume<T>::IsFinite() const
	{
		return extend == Extend_Finite;
	}

	/*!
	* \brief Checks whether the volume is infinite
	* \return true if extend is Extend_Infinite
	*/

	template<typename T>
	bool BoundingVolume<T>::IsInfinite() const
	{
		return extend == Extend_Infinite;
	}

	/*!
	* \brief Checks whether the volume is null
	* \return true if extend is Extend_Null
	*/

	template<typename T>
	bool BoundingVolume<T>::IsNull() const
	{
		return extend == Extend_Null;
	}

	/*!
	* \brief Makes the bounding volume infinite
	* \return A reference to this bounding volume with Extend_Infinite for extend
	*
	* \see Infinite
	*/

	template<typename T>
	BoundingVolume<T>& BoundingVolume<T>::MakeInfinite()
	{
		extend = Extend_Infinite;

		return *this;
	}

	/*!
	* \brief Makes the bounding volume null
	* \return A reference to this bounding volume with Extend_Null for extend
	*
	* \see Null
	*/

	template<typename T>
	BoundingVolume<T>& BoundingVolume<T>::MakeNull()
	{
		extend = Extend_Null;

		return *this;
	}

	/*!
	* \brief Sets the extend of the bounding volume from Extend
	* \return A reference to this bounding volume
	*
	* \param Extend New extend
	*
	* \remark This method is meant to be called with Extend_Infinite or Extend_Null
	*/

	template<typename T>
	BoundingVolume<T>& BoundingVolume<T>::Set(Extend Extend)
	{
		extend = Extend;

		return *this;
	}

	/*!
	* \brief Sets the components of the bounding volume
	* \return A reference to this bounding volume
	*
	* \param X X position
	* \param Y Y position
	* \param Z Z position
	* \param Width Width of the oriented box (following X)
	* \param Height Height of the oriented box (following Y)
	* \param Depth Depth of the oriented box (following Z)
	*/

	template<typename T>
	BoundingVolume<T>& BoundingVolume<T>::Set(T X, T Y, T Z, T Width, T Height, T Depth)
	{
		obb.Set(X, Y, Z, Width, Height, Depth);
		extend = Extend_Finite;

		return *this;
	}

	/*!
	* \brief Sets the components of the bounding volume from another bounding volume
	* \return A reference to this bounding volume
	*
	* \param volume The other bounding volume
	*/

	template<typename T>
	BoundingVolume<T>& BoundingVolume<T>::Set(const BoundingVolume<T>& volume)
	{
		obb.Set(volume.obb); // Only OBB is important for the moment
		extend = volume.extend;

		return *this;
	}

	/*!
	* \brief Sets the components of the bounding volume from a box
	* \return A reference to this bounding volume
	*
	* \param box Box<T> object
	*/

	template<typename T>
	BoundingVolume<T>& BoundingVolume<T>::Set(const Box<T>& box)
	{
		obb.Set(box);
		extend = Extend_Finite;

		return *this;
	}

	/*!
	* \brief Sets the components of the bounding volume from an oriented box
	* \return A reference to this bounding volume
	*
	* \param orientedBox OrientedBox<T> object
	*/

	template<typename T>
	BoundingVolume<T>& BoundingVolume<T>::Set(const OrientedBox<T>& orientedBox)
	{
		obb.Set(orientedBox);
		extend = Extend_Finite;

		return *this;
	}

	/*!
	* \brief Sets a BoundingVolume<T> object from two vectors representing point of the space
	* (X, Y, Z) will be the components minimum of the two vectors and the (width, height, depth) will be the components maximum - minimum
	*
	* \param vec1 First point
	* \param vec2 Second point
	*/

	template<typename T>
	BoundingVolume<T>& BoundingVolume<T>::Set(const Vector3<T>& vec1, const Vector3<T>& vec2)
	{
		obb.Set(vec1, vec2);
		extend = Extend_Finite;

		return *this;
	}

	/*!
	* \brief Sets the components of the bounding volume from another type of BoundingVolume
	* \return A reference to this bounding volume
	*
	* \param volume BoundingVolume of type U to convert its components
	*/

	template<typename T>
	template<typename U>
	BoundingVolume<T>& BoundingVolume<T>::Set(const BoundingVolume<U>& volume)
	{
		obb.Set(volume.obb);
		extend = volume.extend;

		return *this;
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object: "BoundingVolume(localBox="")" if finite, or "BoundingVolume(Infinite)" or "BoundingVolume(Null)"
	*
	* \remark If enumeration is not defined in Extend, a NazaraError is thrown and "BoundingVolume(ERROR)" is returned
	*/

	template<typename T>
	String BoundingVolume<T>::ToString() const
	{
		switch (extend)
		{
			case Extend_Finite:
				return "BoundingVolume(localBox=" + obb.localBox.ToString() + ')';

			case Extend_Infinite:
				return "BoundingVolume(Infinite)";

			case Extend_Null:
				return "BoundingVolume(Null)";
		}

		// Si nous arrivons ici c'est que l'extend est invalide
		NazaraError("Invalid extend type (0x" + String::Number(extend, 16) + ')');
		return "BoundingVolume(ERROR)";
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

		aabb.Set(obb(0), obb(1));
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

		aabb.Set(obb(0), obb(1));
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
			if (extend == Extend_Finite)
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
	* \brief Shorthand for the bounding volume (Extend_Infinite)
	* \return A bounding volume with Extend_Infinite
	*
	* \see MakeInfinite
	*/

	template<typename T>
	BoundingVolume<T> BoundingVolume<T>::Infinite()
	{
		BoundingVolume volume;
		volume.MakeInfinite();

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
		#ifdef NAZARA_DEBUG
		if (interpolation < F(0.0) || interpolation > F(1.0))
		{
			NazaraError("Interpolation must be in range [0..1] (Got " + String::Number(interpolation) + ')');
			return Null();
		}
		#endif

		if (NumberEquals(interpolation, F(0.0)))
			return from;

		if (NumberEquals(interpolation, F(1.0)))
			return to;

		switch (to.extend)
		{
			case Extend_Finite:
			{
				switch (from.extend)
				{
					case Extend_Finite:
					{
						BoundingVolume volume;
						volume.Set(OrientedBox<T>::Lerp(from.obb, to.obb, interpolation));

						return volume;
					}

					case Extend_Infinite:
						return Infinite();

					case Extend_Null:
						return from.obb * interpolation;
				}

				// If we arrive here, the extend is invalid
				NazaraError("Invalid extend type (From) (0x" + String::Number(from.extend, 16) + ')');
				return Null();
			}

			case Extend_Infinite:
				return Infinite(); // A little bit of infinity is already too much ;)

			case Extend_Null:
			{
				switch (from.extend)
				{
					case Extend_Finite:
						return from.obb * (F(1.0) - interpolation);

					case Extend_Infinite:
						return Infinite();

					case Extend_Null:
						return Null();
				}

				// If we arrive here, the extend is invalid
				NazaraError("Invalid extend type (From) (0x" + String::Number(from.extend, 16) + ')');
				return Null();
			}
		}

		// If we arrive here, the extend is invalid
		NazaraError("Invalid extend type (To) (0x" + String::Number(to.extend, 16) + ')');
		return Null();
	}

	/*!
	* \brief Shorthand for the bounding volume (Extend_Null)
	* \return A bounding volume with Extend_Null
	*
	* \see MakeNull
	*/

	template<typename T>
	BoundingVolume<T> BoundingVolume<T>::Null()
	{
		BoundingVolume volume;
		volume.MakeNull();

		return volume;
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
	out << volume.ToString();
	return out;
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
