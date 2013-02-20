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
NzAxisAlignedBox<T>::NzAxisAlignedBox() :
extend(nzExtend_Null)
{
}

template<typename T>
NzAxisAlignedBox<T>::NzAxisAlignedBox(nzExtend Extend)
{
	Set(Extend);
}

template<typename T>
NzAxisAlignedBox<T>::NzAxisAlignedBox(T X, T Y, T Z, T Width, T Height, T Depth)
{
	Set(X, Y, Z, Width, Height, Depth);
}

template<typename T>
NzAxisAlignedBox<T>::NzAxisAlignedBox(const NzCube<T>& Cube)
{
	Set(Cube);
}

template<typename T>
NzAxisAlignedBox<T>::NzAxisAlignedBox(const NzVector3<T>& vec1, const NzVector3<T>& vec2)
{
	Set(vec1, vec2);
}

template<typename T>
template<typename U>
NzAxisAlignedBox<T>::NzAxisAlignedBox(const NzAxisAlignedBox<U>& box)
{
	Set(box);
}

template<typename T>
bool NzAxisAlignedBox<T>::Contains(T x, T y, T z) const
{
	switch (extend)
	{
		case nzExtend_Finite:
			return cube.Contains(x, y, z);

		case nzExtend_Infinite:
			return true;

		case nzExtend_Null:
			return false;
	}

	// Si nous arrivons ici c'est que l'extend est invalide
	NazaraError("Invalid extend type (0x" + NzString::Number(extend, 16) + ')');
	return false;
}

template<typename T>
bool NzAxisAlignedBox<T>::Contains(const NzAxisAlignedBox& box) const
{
	if (extend == nzExtend_Null || box.extend == nzExtend_Null)
		return false;
	else if (extend == nzExtend_Infinite || box.extend == nzExtend_Infinite)
		return true;

	return cube.Contains(box.cube);
}

template<typename T>
bool NzAxisAlignedBox<T>::Contains(const NzVector3<T>& vector) const
{
	switch (extend)
	{
		case nzExtend_Finite:
			return cube.Contains(vector);

		case nzExtend_Infinite:
			return true;

		case nzExtend_Null:
			return false;
	}

	// Si nous arrivons ici c'est que l'extend est invalide
	NazaraError("Invalid extend type (0x" + NzString::Number(extend, 16) + ')');
	return false;
}

template<typename T>
NzAxisAlignedBox<T>& NzAxisAlignedBox<T>::ExtendTo(T X, T Y, T Z)
{
	switch (extend)
	{
		case nzExtend_Finite:
			cube.ExtendTo(X, Y, Z);
			return *this;

		case nzExtend_Infinite:
			return *this; // Rien à faire

		case nzExtend_Null:
			return Set(cube.Set(X, Y, Z, F(0.0), F(0.0), F(0.0)));;
	}

	// Si nous arrivons ici c'est que l'extend est invalide
	NazaraError("Invalid extend type (0x" + NzString::Number(extend, 16) + ')');
	return *this;
}

template<typename T>
NzAxisAlignedBox<T>& NzAxisAlignedBox<T>::ExtendTo(const NzAxisAlignedBox& box)
{
	switch (extend)
	{
		case nzExtend_Finite:
			switch (box.extend)
			{
				case nzExtend_Finite:
					cube.ExtendTo(box.cube);
					return *this;

				case nzExtend_Infinite:
					return MakeInfinite();

				case nzExtend_Null:
					return *this;
			}

			// Si nous arrivons ici c'est que l'extend est invalide
			NazaraError("Invalid extend type (0x" + NzString::Number(box.extend, 16) + ')');
			return *this;

		case nzExtend_Infinite:
			return *this; // Rien à faire

		case nzExtend_Null:
			return Set(box);
	}

	// Si nous arrivons ici c'est que l'extend est invalide
	NazaraError("Invalid extend type (0x" + NzString::Number(extend, 16) + ')');
	return *this;
}

template<typename T>
NzAxisAlignedBox<T>& NzAxisAlignedBox<T>::ExtendTo(const NzVector3<T>& vector)
{
	return ExtendTo(vector.x, vector.y, vector.z);
}

template<typename T>
NzVector3<T> NzAxisAlignedBox<T>::GetCorner(nzCorner corner) const
{
	switch (extend)
	{
		case nzExtend_Finite:
			return cube.GetCorner(corner);

		case nzExtend_Infinite:
			// Il est possible de renvoyer un vecteur avec des flottants infinis dont le signe dépend du coin
			// Bien que ça soit plus juste mathématiquement, je ne vois pas l'intérêt...
			NazaraError("Infinite AABB has no corner");
			return NzVector3<T>();

		case nzExtend_Null:
			return NzVector3<T>::Zero();
	}

	// Si nous arrivons ici c'est que l'extend est invalide
	NazaraError("Invalid extend type (0x" + NzString::Number(extend, 16) + ')');
	return NzVector3<T>();
}

template<typename T>
NzCube<T> NzAxisAlignedBox<T>::GetCube() const
{
	return cube;
}

template<typename T>
nzExtend NzAxisAlignedBox<T>::GetExtend() const
{
	return extend;
}

template<typename T>
NzVector3<T> NzAxisAlignedBox<T>::GetNegativeVertex(const NzVector3<T>& normal) const
{
	switch (extend)
	{
		case nzExtend_Finite:
			return cube.GetNegativeVertex(normal);

		case nzExtend_Infinite:
			// Il est possible de renvoyer un vecteur avec des flottants infinis dont le signe dépend de la normale
			// Bien que ça soit plus juste mathématiquement, je ne vois pas l'intérêt...
			NazaraError("Infinite AABB has no negative vertex");
			return NzVector3<T>();

		case nzExtend_Null:
			return NzVector3<T>::Zero();
	}

	// Si nous arrivons ici c'est que l'extend est invalide
	NazaraError("Invalid extend type (0x" + NzString::Number(extend, 16) + ')');
	return NzVector3<T>();
}

template<typename T>
NzVector3<T> NzAxisAlignedBox<T>::GetPosition() const
{
	switch (extend)
	{
		case nzExtend_Finite:
			return cube.GetPosition();

		case nzExtend_Infinite:
			// Il est possible de renvoyer un vecteur avec des flottants infinis
			// Bien que ça soit plus juste mathématiquement, je ne vois pas l'intérêt...
			NazaraError("Infinite AABB has no position");
			return NzVector3<T>();

		case nzExtend_Null:
			return NzVector3<T>::Zero();
	}

	// Si nous arrivons ici c'est que l'extend est invalide
	NazaraError("Invalid extend type (0x" + NzString::Number(extend, 16) + ')');
	return NzVector3<T>();
}

template<typename T>
NzVector3<T> NzAxisAlignedBox<T>::GetPositiveVertex(const NzVector3<T>& normal) const
{
	switch (extend)
	{
		case nzExtend_Finite:
			return cube.GetPositiveVertex(normal);

		case nzExtend_Infinite:
			// Il est possible de renvoyer un vecteur avec des flottants infinis dont le signe dépend de la normale
			// Bien que ça soit plus juste mathématiquement, je ne vois pas l'intérêt...
			NazaraError("Infinite AABB has no corner");
			return NzVector3<T>();

		case nzExtend_Null:
			return NzVector3<T>::Zero();
	}

	// Si nous arrivons ici c'est que l'extend est invalide
	NazaraError("Invalid extend type (0x" + NzString::Number(extend, 16) + ')');
	return NzVector3<T>();
}

template<typename T>
NzVector3<T> NzAxisAlignedBox<T>::GetSize() const
{
	switch (extend)
	{
		case nzExtend_Finite:
			return cube.GetSize();

		case nzExtend_Infinite:
			// Il est possible de renvoyer un vecteur avec des flottants infinis
			// Bien que ça soit plus juste mathématiquement, je ne vois pas l'intérêt...
			NazaraError("Infinite AABB has no size");
			return NzVector3<T>();

		case nzExtend_Null:
			return NzVector3<T>::Zero();
	}

	// Si nous arrivons ici c'est que l'extend est invalide
	NazaraError("Invalid extend type (0x" + NzString::Number(extend, 16) + ')');
	return NzVector3<T>();
}

template<typename T>
bool NzAxisAlignedBox<T>::Intersect(const NzAxisAlignedBox& box, NzAxisAlignedBox* intersection) const
{
	switch (extend)
	{
		case nzExtend_Finite:
		{
			switch (box.extend)
			{
				case nzExtend_Finite:
				{
					if (cube.Intersect(box.cube, &intersection->cube))
					{
						intersection->extend = nzExtend_Finite;
						return true;
					}
					else
						return false;
				}

				case nzExtend_Infinite:
					intersection->Set(*this);
					return true;

				case nzExtend_Null:
					return false;
			}

			NazaraError("Invalid extend type (0x" + NzString::Number(box.extend, 16) + ')');
			return false;
		}

		case nzExtend_Infinite:
			if (!box.IsNull()) // Si l'AABB n'est pas nulle, c'est qu'elle est finie ou infinie
			{
				// Et dans ce cas, il y a toujous intersection équivalente à la seconde AABB
				intersection->Set(box);
				return true;
			}
			else
				return false;

		case nzExtend_Null:
			return false; // N'a jamais de collision avec quoi que ce soit
	}

	// Si nous arrivons ici c'est que l'extend est invalide
	NazaraError("Invalid extend type (0x" + NzString::Number(extend, 16) + ')');
	return false;
}

template<typename T>
bool NzAxisAlignedBox<T>::IsFinite() const
{
	return extend == nzExtend_Finite;
}

template<typename T>
bool NzAxisAlignedBox<T>::IsInfinite() const
{
	return extend == nzExtend_Infinite;
}

template<typename T>
bool NzAxisAlignedBox<T>::IsNull() const
{
	return extend == nzExtend_Null;
}

template<typename T>
NzAxisAlignedBox<T>& NzAxisAlignedBox<T>::MakeInfinite()
{
	extend = nzExtend_Infinite;

	return *this;
}

template<typename T>
NzAxisAlignedBox<T>& NzAxisAlignedBox<T>::MakeNull()
{
	extend = nzExtend_Infinite;

	return *this;
}

template<typename T>
NzAxisAlignedBox<T>& NzAxisAlignedBox<T>::Set(nzExtend Extend)
{
	extend = Extend;

	return *this;
}

template<typename T>
NzAxisAlignedBox<T>& NzAxisAlignedBox<T>::Set(T X, T Y, T Z, T Width, T Height, T Depth)
{
	cube.Set(X, Y, Z, Width, Height, Depth);
	extend = nzExtend_Finite;

	return *this;
}

template<typename T>
NzAxisAlignedBox<T>& NzAxisAlignedBox<T>::Set(const NzAxisAlignedBox<T>& box)
{
	std::memcpy(this, &box, sizeof(NzAxisAlignedBox));

	return *this;
}

template<typename T>
NzAxisAlignedBox<T>& NzAxisAlignedBox<T>::Set(const NzCube<T>& Cube)
{
	cube.Set(Cube);
	extend = nzExtend_Finite;

	return *this;
}

template<typename T>
NzAxisAlignedBox<T>& NzAxisAlignedBox<T>::Set(const NzVector3<T>& vec1, const NzVector3<T>& vec2)
{
	cube.Set(vec1, vec2);
	extend = nzExtend_Finite;

	return *this;
}

template<typename T>
template<typename U>
NzAxisAlignedBox<T>& NzAxisAlignedBox<T>::Set(const NzAxisAlignedBox<U>& box)
{
	cube.Set(box);
	extend = nzExtend_Finite;

	return *this;
}

template<typename T>
NzString NzAxisAlignedBox<T>::ToString() const
{
	switch (extend)
	{
		case nzExtend_Finite:
			return "NzAxisAlignedBox(min=" + cube.GetPosition().ToString() + ", max=" + (cube.GetPosition()+cube.GetSize()).ToString() + ')';

		case nzExtend_Infinite:
			return "NzAxisAlignedBox(Infinite)";

		case nzExtend_Null:
			return "NzAxisAlignedBox(Null)";
	}

	// Si nous arrivons ici c'est que l'extend est invalide
	NazaraError("Invalid extend type (0x" + NzString::Number(extend, 16) + ')');
	return "NzAxisAlignedBox(ERROR)";
}

template<typename T>
NzAxisAlignedBox<T>& NzAxisAlignedBox<T>::Transform(const NzMatrix4<T>& matrix, bool applyTranslation)
{
	if (extend != nzExtend_Finite)
		return *this; // Toute transformation d'une AABox autre que finie résultera en la même AABox

	NzVector3<T> center = matrix.Transform(cube.GetCenter(), (applyTranslation) ? F(1.0) : F(0.0)); // Valeur multipliant la translation
	NzVector3<T> halfSize = cube.GetSize() * F(0.5);

	halfSize.Set(std::fabs(matrix(0,0))*halfSize.x + std::fabs(matrix(1,0))*halfSize.y + std::fabs(matrix(2,0))*halfSize.z,
	             std::fabs(matrix(0,1))*halfSize.x + std::fabs(matrix(1,1))*halfSize.y + std::fabs(matrix(2,1))*halfSize.z,
	             std::fabs(matrix(0,2))*halfSize.x + std::fabs(matrix(1,2))*halfSize.y + std::fabs(matrix(2,2))*halfSize.z);

	cube.Set(center - halfSize, center + halfSize);

	return *this;
}

template<typename T>
NzAxisAlignedBox<T> NzAxisAlignedBox<T>::operator*(T scalar) const
{
	NzAxisAlignedBox box(*this);
	box *= scalar;

	return box;
}

template<typename T>
NzAxisAlignedBox<T>& NzAxisAlignedBox<T>::operator*=(T scalar)
{
	switch (extend)
	{
		case nzExtend_Finite:
			cube *= scalar;
			return *this;

		case nzExtend_Infinite:
			// L'infini multiplié par quoi que ce soit d'autre que zéro reste l'infini
			// (On ne se préoccupe pas de l'infini de signe négatif, car ça finirait par être équivalent)
			if (NzNumberEquals(scalar, F(0.0)))
				MakeNull();

			return *this;

		case nzExtend_Null:
			return *this; //
	}

	// Si nous arrivons ici c'est que l'extend est invalide
	NazaraError("Invalid extend type (0x" + NzString::Number(extend, 16) + ')');
	return NzVector3<T>();
}

template<typename T>
bool NzAxisAlignedBox<T>::operator==(const NzAxisAlignedBox& box) const
{
	if (extend == box.extend)
		return cube == box.cube;
	else
		return false;
}

template<typename T>
bool NzAxisAlignedBox<T>::operator!=(const NzAxisAlignedBox& box) const
{
	return !operator==(box);
}

template<typename T>
NzAxisAlignedBox<T> NzAxisAlignedBox<T>::Infinite()
{
	NzAxisAlignedBox box;
	box.MakeInfinite();

	return box;
}

template<typename T>
NzAxisAlignedBox<T> NzAxisAlignedBox<T>::Lerp(const NzAxisAlignedBox& from, const NzAxisAlignedBox& to, T interpolation)
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
					return NzCube<T>::Lerp(from.cube, to.cube, interpolation);

				case nzExtend_Infinite:
					return Infinite();

				case nzExtend_Null:
					return from.cube * interpolation;
			}

			// Si nous arrivons ici c'est que l'extend est invalide
			NazaraError("Invalid extend type (From AABB) (0x" + NzString::Number(from.extend, 16) + ')');
			return Null();
		}

		case nzExtend_Infinite:
			return Infinite(); // Un petit peu d'infini est infini quand même ;)

		case nzExtend_Null:
		{
			switch (from.extend)
			{
				case nzExtend_Finite:
					return from.cube * (F(1.0) - interpolation);

				case nzExtend_Infinite:
					return Infinite();

				case nzExtend_Null:
					return Null();
			}

			// Si nous arrivons ici c'est que l'extend est invalide
			NazaraError("Invalid extend type (From AABB) (0x" + NzString::Number(from.extend, 16) + ')');
			return Null();
		}
	}

	// Si nous arrivons ici c'est que l'extend est invalide
	NazaraError("Invalid extend type (To AABB) (0x" + NzString::Number(from.extend, 16) + ')');
	return Null();
}

template<typename T>
NzAxisAlignedBox<T> NzAxisAlignedBox<T>::Null()
{
	NzAxisAlignedBox box;
	box.MakeNull();

	return box;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzAxisAlignedBox<T>& box)
{
	out << box.ToString();
	return out;
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
