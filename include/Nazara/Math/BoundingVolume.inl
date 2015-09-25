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
	template<typename T>
	BoundingVolume<T>::BoundingVolume() :
	extend(Extend_Null)
	{
	}

	template<typename T>
	BoundingVolume<T>::BoundingVolume(Extend Extend)
	{
		Set(Extend);
	}

	template<typename T>
	BoundingVolume<T>::BoundingVolume(T X, T Y, T Z, T Width, T Height, T Depth)
	{
		Set(X, Y, Z, Width, Height, Depth);
	}

	template<typename T>
	BoundingVolume<T>::BoundingVolume(const Box<T>& box)
	{
		Set(box);
	}

	template<typename T>
	BoundingVolume<T>::BoundingVolume(const OrientedBox<T>& orientedBox)
	{
		Set(orientedBox);
	}

	template<typename T>
	BoundingVolume<T>::BoundingVolume(const Vector3<T>& vec1, const Vector3<T>& vec2)
	{
		Set(vec1, vec2);
	}

	template<typename T>
	template<typename U>
	BoundingVolume<T>::BoundingVolume(const BoundingVolume<U>& volume)
	{
		Set(volume);
	}

	template<typename T>
	bool BoundingVolume<T>::IsFinite() const
	{
		return extend == Extend_Finite;
	}

	template<typename T>
	bool BoundingVolume<T>::IsInfinite() const
	{
		return extend == Extend_Infinite;
	}

	template<typename T>
	bool BoundingVolume<T>::IsNull() const
	{
		return extend == Extend_Null;
	}

	template<typename T>
	BoundingVolume<T>& BoundingVolume<T>::MakeInfinite()
	{
		extend = Extend_Infinite;

		return *this;
	}

	template<typename T>
	BoundingVolume<T>& BoundingVolume<T>::MakeNull()
	{
		extend = Extend_Null;

		return *this;
	}

	template<typename T>
	BoundingVolume<T>& BoundingVolume<T>::Set(Extend Extend)
	{
		extend = Extend;

		return *this;
	}

	template<typename T>
	BoundingVolume<T>& BoundingVolume<T>::Set(T X, T Y, T Z, T Width, T Height, T Depth)
	{
		obb.Set(X, Y, Z, Width, Height, Depth);
		extend = Extend_Finite;

		return *this;
	}

	template<typename T>
	BoundingVolume<T>& BoundingVolume<T>::Set(const BoundingVolume<T>& volume)
	{
		obb.Set(volume.obb); // Seul l'OBB est importante pour la suite
		extend = volume.extend;

		return *this;
	}

	template<typename T>
	BoundingVolume<T>& BoundingVolume<T>::Set(const Box<T>& box)
	{
		obb.Set(box);
		extend = Extend_Finite;

		return *this;
	}

	template<typename T>
	BoundingVolume<T>& BoundingVolume<T>::Set(const OrientedBox<T>& orientedBox)
	{
		obb.Set(orientedBox);
		extend = Extend_Finite;

		return *this;
	}

	template<typename T>
	BoundingVolume<T>& BoundingVolume<T>::Set(const Vector3<T>& vec1, const Vector3<T>& vec2)
	{
		obb.Set(vec1, vec2);
		extend = Extend_Finite;

		return *this;
	}

	template<typename T>
	template<typename U>
	BoundingVolume<T>& BoundingVolume<T>::Set(const BoundingVolume<U>& volume)
	{
		obb.Set(volume.obb);
		extend = volume.extend;

		return *this;
	}

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

	template<typename T>
	void BoundingVolume<T>::Update(const Matrix4<T>& transformMatrix)
	{
		obb.Update(transformMatrix);

		aabb.Set(obb(0), obb(1));
		for (unsigned int i = 2; i < 8; ++i)
			aabb.ExtendTo(obb(i));
	}

	template<typename T>
	void BoundingVolume<T>::Update(const Vector3<T>& translation)
	{
		obb.Update(translation);

		aabb.Set(obb(0), obb(1));
		for (unsigned int i = 2; i < 8; ++i)
			aabb.ExtendTo(obb(i));
	}

	template<typename T>
	BoundingVolume<T> BoundingVolume<T>::operator*(T scalar) const
	{
		BoundingVolume volume(*this);
		volume *= scalar;

		return volume;
	}

	template<typename T>
	BoundingVolume<T>& BoundingVolume<T>::operator*=(T scalar)
	{
		obb *= scalar;

		return *this;
	}

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

	template<typename T>
	bool BoundingVolume<T>::operator!=(const BoundingVolume& volume) const
	{
		return !operator==(volume);
	}

	template<typename T>
	BoundingVolume<T> BoundingVolume<T>::Infinite()
	{
		BoundingVolume volume;
		volume.MakeInfinite();

		return volume;
	}

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

				// Si nous arrivons ici c'est que l'extend est invalide
				NazaraError("Invalid extend type (From) (0x" + String::Number(from.extend, 16) + ')');
				return Null();
			}

			case Extend_Infinite:
				return Infinite(); // Un petit peu d'infini est infini quand même ;)

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

				// Si nous arrivons ici c'est que l'extend est invalide
				NazaraError("Invalid extend type (From) (0x" + String::Number(from.extend, 16) + ')');
				return Null();
			}
		}

		// Si nous arrivons ici c'est que l'extend est invalide
		NazaraError("Invalid extend type (To) (0x" + String::Number(to.extend, 16) + ')');
		return Null();
	}

	template<typename T>
	BoundingVolume<T> BoundingVolume<T>::Null()
	{
		BoundingVolume volume;
		volume.MakeNull();

		return volume;
	}

	template<typename T>
	std::ostream& operator<<(std::ostream& out, const BoundingVolume<T>& volume)
	{
		out << volume.ToString();
		return out;
}
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
