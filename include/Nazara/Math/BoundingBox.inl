// Copyright (C) 2012 Jérôme Leclercq
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
NzBoundingBox<T>::NzBoundingBox() :
extend(nzExtend_Null)
{
}

template<typename T>
NzBoundingBox<T>::NzBoundingBox(nzExtend Extend)
{
	Set(Extend);
}

template<typename T>
NzBoundingBox<T>::NzBoundingBox(T X, T Y, T Z, T Width, T Height, T Depth)
{
	Set(X, Y, Z, Width, Height, Depth);
}

template<typename T>
NzBoundingBox<T>::NzBoundingBox(const NzCube<T>& Cube)
{
	Set(Cube);
}

template<typename T>
NzBoundingBox<T>::NzBoundingBox(const NzVector3<T>& vec1, const NzVector3<T>& vec2)
{
	Set(vec1, vec2);
}

template<typename T>
template<typename U>
NzBoundingBox<T>::NzBoundingBox(const NzBoundingBox<U>& box)
{
	Set(box);
}

template<typename T>
bool NzBoundingBox<T>::IsFinite() const
{
	return extend == nzExtend_Finite;
}

template<typename T>
bool NzBoundingBox<T>::IsInfinite() const
{
	return extend == nzExtend_Infinite;
}

template<typename T>
bool NzBoundingBox<T>::IsNull() const
{
	return extend == nzExtend_Null;
}

template<typename T>
NzBoundingBox<T>& NzBoundingBox<T>::MakeInfinite()
{
	extend = nzExtend_Infinite;

	return *this;
}

template<typename T>
NzBoundingBox<T>& NzBoundingBox<T>::MakeNull()
{
	extend = nzExtend_Null;

	return *this;
}

template<typename T>
NzBoundingBox<T>& NzBoundingBox<T>::Set(nzExtend Extend)
{
	extend = Extend;

	return *this;
}

template<typename T>
NzBoundingBox<T>& NzBoundingBox<T>::Set(T X, T Y, T Z, T Width, T Height, T Depth)
{
	obb.Set(X, Y, Z, Width, Height, Depth);
	extend = nzExtend_Finite;

	return *this;
}

template<typename T>
NzBoundingBox<T>& NzBoundingBox<T>::Set(const NzBoundingBox<T>& box)
{
	obb.Set(box.obb); // Seul l'OBB est importante pour la suite

	return *this;
}

template<typename T>
NzBoundingBox<T>& NzBoundingBox<T>::Set(const NzCube<T>& Cube)
{
	obb.Set(Cube);
	extend = nzExtend_Finite;

	return *this;
}

template<typename T>
NzBoundingBox<T>& NzBoundingBox<T>::Set(const NzVector3<T>& vec1, const NzVector3<T>& vec2)
{
	obb.Set(vec1, vec2);
	extend = nzExtend_Finite;

	return *this;
}

template<typename T>
template<typename U>
NzBoundingBox<T>& NzBoundingBox<T>::Set(const NzBoundingBox<U>& box)
{
	obb.Set(box.obb);
	extend = box.extend;

	return *this;
}

template<typename T>
NzString NzBoundingBox<T>::ToString() const
{
	switch (extend)
	{
		case nzExtend_Finite:
			return "BoundingBox(localCube=" + obb.localCube.ToString() + ')';

		case nzExtend_Infinite:
			return "BoundingBox(Infinite)";

		case nzExtend_Null:
			return "BoundingBox(Null)";
	}

	// Si nous arrivons ici c'est que l'extend est invalide
	NazaraError("Invalid extend type (0x" + NzString::Number(extend, 16) + ')');
	return "BoundingBox(ERROR)";
}

template<typename T>
void NzBoundingBox<T>::Update(const NzMatrix4<T>& transformMatrix)
{
	aabb.Set(obb.localCube);
	aabb.Transform(transformMatrix);
	obb.Update(transformMatrix);
}

template<typename T>
NzBoundingBox<T> NzBoundingBox<T>::operator*(T scalar) const
{
	NzBoundingBox box(*this);
	box *= scalar;

	return box;
}

template<typename T>
NzBoundingBox<T>& NzBoundingBox<T>::operator*=(T scalar)
{
	obb *= scalar;

	return *this;
}

template<typename T>
bool NzBoundingBox<T>::operator==(const NzBoundingBox& box) const
{
	if (extend == box.extend)
		return obb == box.obb;
	else
		return false;
}

template<typename T>
bool NzBoundingBox<T>::operator!=(const NzBoundingBox& box) const
{
	return !operator==(box);
}

template<typename T>
NzBoundingBox<T> NzBoundingBox<T>::Infinite()
{
	NzBoundingBox box;
	box.MakeInfinite();

	return box;
}

template<typename T>
NzBoundingBox<T> NzBoundingBox<T>::Lerp(const NzBoundingBox& from, const NzBoundingBox& to, T interpolation)
{
	#ifdef NAZARA_DEBUG
	if (interpolation < 0.f || interpolation > 1.f)
	{
		NazaraError("Interpolation must be in range [0..1] (Got " + NzString::Number(interpolation) + ')');
		return Null();
	}
	#endif

	if (NzNumberEquals(interpolation, 0.f))
		return from;

	if (NzNumberEquals(interpolation, 1.f))
		return to;

	switch (to.extend)
	{
		case nzExtend_Finite:
		{
			switch (from.extend)
			{
				case nzExtend_Finite:
				{
					NzBoundingBox box;
					box.Set(NzOrientedCube<T>::Lerp(from.obb, to.obb, interpolation));

					return box;
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
	NazaraError("Invalid extend type (To) (0x" + NzString::Number(from.extend, 16) + ')');
	return Null();
}

template<typename T>
NzBoundingBox<T> NzBoundingBox<T>::Null()
{
	NzBoundingBox box;
	box.MakeNull();

	return box;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzBoundingBox<T>& box)
{
	out << box.ToString();
	return out;
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
