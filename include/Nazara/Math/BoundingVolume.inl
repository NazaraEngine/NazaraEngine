// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Math/Basic.hpp>
#include <algorithm>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)

template<typename T>
NzBoundingVolume<T>::NzBoundingVolume() :
extend(nzExtend_Null)
{
}

template<typename T>
NzBoundingVolume<T>::NzBoundingVolume(nzExtend Extend)
{
	Set(Extend);
}

template<typename T>
NzBoundingVolume<T>::NzBoundingVolume(T X, T Y, T Z, T Width, T Height, T Depth)
{
	Set(X, Y, Z, Width, Height, Depth);
}

template<typename T>
NzBoundingVolume<T>::NzBoundingVolume(const NzBox<T>& box)
{
	Set(box);
}

template<typename T>
NzBoundingVolume<T>::NzBoundingVolume(const NzOrientedBox<T>& orientedBox)
{
	Set(orientedBox);
}

template<typename T>
NzBoundingVolume<T>::NzBoundingVolume(const NzVector3<T>& vec1, const NzVector3<T>& vec2)
{
	Set(vec1, vec2);
}

template<typename T>
template<typename U>
NzBoundingVolume<T>::NzBoundingVolume(const NzBoundingVolume<U>& volume)
{
	Set(volume);
}

template<typename T>
bool NzBoundingVolume<T>::IsFinite() const
{
	return extend == nzExtend_Finite;
}

template<typename T>
bool NzBoundingVolume<T>::IsInfinite() const
{
	return extend == nzExtend_Infinite;
}

template<typename T>
bool NzBoundingVolume<T>::IsNull() const
{
	return extend == nzExtend_Null;
}

template<typename T>
NzBoundingVolume<T>& NzBoundingVolume<T>::MakeInfinite()
{
	extend = nzExtend_Infinite;

	return *this;
}

template<typename T>
NzBoundingVolume<T>& NzBoundingVolume<T>::MakeNull()
{
	extend = nzExtend_Null;

	return *this;
}

template<typename T>
NzBoundingVolume<T>& NzBoundingVolume<T>::Set(nzExtend Extend)
{
	extend = Extend;

	return *this;
}

template<typename T>
NzBoundingVolume<T>& NzBoundingVolume<T>::Set(T X, T Y, T Z, T Width, T Height, T Depth)
{
	obb.Set(X, Y, Z, Width, Height, Depth);
	extend = nzExtend_Finite;

	return *this;
}

template<typename T>
NzBoundingVolume<T>& NzBoundingVolume<T>::Set(const NzBoundingVolume<T>& volume)
{
	obb.Set(volume.obb); // Seul l'OBB est importante pour la suite
	extend = volume.extend;

	return *this;
}

template<typename T>
NzBoundingVolume<T>& NzBoundingVolume<T>::Set(const NzBox<T>& box)
{
	obb.Set(box);
	extend = nzExtend_Finite;

	return *this;
}

template<typename T>
NzBoundingVolume<T>& NzBoundingVolume<T>::Set(const NzOrientedBox<T>& orientedBox)
{
	obb.Set(orientedBox);
	extend = nzExtend_Finite;

	return *this;
}

template<typename T>
NzBoundingVolume<T>& NzBoundingVolume<T>::Set(const NzVector3<T>& vec1, const NzVector3<T>& vec2)
{
	obb.Set(vec1, vec2);
	extend = nzExtend_Finite;

	return *this;
}

template<typename T>
template<typename U>
NzBoundingVolume<T>& NzBoundingVolume<T>::Set(const NzBoundingVolume<U>& volume)
{
	obb.Set(volume.obb);
	extend = volume.extend;

	return *this;
}

template<typename T>
NzString NzBoundingVolume<T>::ToString() const
{
	switch (extend)
	{
		case nzExtend_Finite:
			return "BoundingVolume(localBox=" + obb.localBox.ToString() + ')';

		case nzExtend_Infinite:
			return "BoundingVolume(Infinite)";

		case nzExtend_Null:
			return "BoundingVolume(Null)";
	}

	// Si nous arrivons ici c'est que l'extend est invalide
	NazaraError("Invalid extend type (0x" + NzString::Number(extend, 16) + ')');
	return "BoundingVolume(ERROR)";
}

template<typename T>
void NzBoundingVolume<T>::Update(const NzMatrix4<T>& transformMatrix)
{
	obb.Update(transformMatrix);

	aabb.Set(obb(0), obb(1));
	for (unsigned int i = 2; i < 8; ++i)
		aabb.ExtendTo(obb(i));
}

template<typename T>
NzBoundingVolume<T> NzBoundingVolume<T>::operator*(T scalar) const
{
	NzBoundingVolume volume(*this);
	volume *= scalar;

	return volume;
}

template<typename T>
NzBoundingVolume<T>& NzBoundingVolume<T>::operator*=(T scalar)
{
	obb *= scalar;

	return *this;
}

template<typename T>
bool NzBoundingVolume<T>::operator==(const NzBoundingVolume& volume) const
{
	if (extend == volume.extend)
		return obb == volume.obb;
	else
		return false;
}

template<typename T>
bool NzBoundingVolume<T>::operator!=(const NzBoundingVolume& volume) const
{
	return !operator==(volume);
}

template<typename T>
NzBoundingVolume<T> NzBoundingVolume<T>::Infinite()
{
	NzBoundingVolume volume;
	volume.MakeInfinite();

	return volume;
}

template<typename T>
NzBoundingVolume<T> NzBoundingVolume<T>::Lerp(const NzBoundingVolume& from, const NzBoundingVolume& to, T interpolation)
{
	#ifdef NAZARA_DEBUG
	if (interpolation < F(0.0) || interpolation > F(1.0))
	{
		NazaraError("Interpolation must be in range [0..1] (Got " + NzString::Number(interpolation) + ')');
		return Null();
	}
	#endif

	if (NzNumberEquals(interpolation, F(0.0)))
		return from;

	if (NzNumberEquals(interpolation, F(1.0)))
		return to;

	switch (to.extend)
	{
		case nzExtend_Finite:
		{
			switch (from.extend)
			{
				case nzExtend_Finite:
				{
					NzBoundingVolume volume;
					volume.Set(NzOrientedBox<T>::Lerp(from.obb, to.obb, interpolation));

					return volume;
				}

				case nzExtend_Infinite:
					return Infinite();

				case nzExtend_Null:
					return from.obb * interpolation;
			}

			// Si nous arrivons ici c'est que l'extend est invalide
			NazaraError("Invalid extend type (From) (0x" + NzString::Number(from.extend, 16) + ')');
			return Null();
		}

		case nzExtend_Infinite:
			return Infinite(); // Un petit peu d'infini est infini quand même ;)

		case nzExtend_Null:
		{
			switch (from.extend)
			{
				case nzExtend_Finite:
					return from.obb * (F(1.0) - interpolation);

				case nzExtend_Infinite:
					return Infinite();

				case nzExtend_Null:
					return Null();
			}

			// Si nous arrivons ici c'est que l'extend est invalide
			NazaraError("Invalid extend type (From) (0x" + NzString::Number(from.extend, 16) + ')');
			return Null();
		}
	}

	// Si nous arrivons ici c'est que l'extend est invalide
	NazaraError("Invalid extend type (To) (0x" + NzString::Number(to.extend, 16) + ')');
	return Null();
}

template<typename T>
NzBoundingVolume<T> NzBoundingVolume<T>::Null()
{
	NzBoundingVolume volume;
	volume.MakeNull();

	return volume;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzBoundingVolume<T>& volume)
{
	out << volume.ToString();
	return out;
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
