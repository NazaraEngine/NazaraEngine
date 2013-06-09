// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Basic.hpp>
#include <algorithm>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)

template<typename T>
NzBox<T>::NzBox(T Width, T Height, T Depth)
{
	Set(Width, Height, Depth);
}

template<typename T>
NzBox<T>::NzBox(T X, T Y, T Z, T Width, T Height, T Depth)
{
	Set(X, Y, Z, Width, Height, Depth);
}

template<typename T>
NzBox<T>::NzBox(const NzRect<T>& rect)
{
	Set(rect);
}

template<typename T>
NzBox<T>::NzBox(const NzVector3<T>& size)
{
	Set(size);
}

template<typename T>
NzBox<T>::NzBox(const NzVector3<T>& vec1, const NzVector3<T>& vec2)
{
	Set(vec1, vec2);
}

template<typename T>
NzBox<T>::NzBox(const T vec[6])
{
	Set(vec);
}

template<typename T>
template<typename U>
NzBox<T>::NzBox(const NzBox<U>& box)
{
	Set(box);
}

template<typename T>
bool NzBox<T>::Contains(T X, T Y, T Z) const
{
	return X >= x && X < x+width &&
	       Y >= y && Y < y+height &&
	       Z >= z && Z < z+depth;
}

template<typename T>
bool NzBox<T>::Contains(const NzBox<T>& box) const
{
	return Contains(box.x, box.y, box.z) &&
		   Contains(box.x + box.width, box.y + box.height, box.z + box.depth);
}

template<typename T>
bool NzBox<T>::Contains(const NzVector3<T>& point) const
{
	return Contains(point.x, point.y, point.z);
}

template<typename T>
NzBox<T>& NzBox<T>::ExtendTo(T X, T Y, T Z)
{
	width = std::max(x + width, X);
	height = std::max(y + height, Y);
	depth = std::max(z + depth, Z);

	x = std::min(x, X);
	y = std::min(y, Y);
	z = std::min(z, Z);

	width -= x;
	height -= y;
	depth -= z;

	return *this;
}

template<typename T>
NzBox<T>& NzBox<T>::ExtendTo(const NzBox& box)
{
	width = std::max(x + width, box.x + box.width);
	height = std::max(y + height, box.y + box.height);
	depth = std::max(z + depth, box.z + box.depth);

	x = std::min(x, box.x);
	y = std::min(y, box.y);
	z = std::min(z, box.z);

	width -= x;
	height -= y;
	depth -= z;

	return *this;
}

template<typename T>
NzBox<T>& NzBox<T>::ExtendTo(const NzVector3<T>& point)
{
	return ExtendTo(point.x, point.y, point.z);
}

template<typename T>
NzVector3<T> NzBox<T>::GetCorner(nzCorner corner) const
{
	switch (corner)
	{
		case nzCorner_FarLeftBottom:
			return NzVector3f(x, y, z);

		case nzCorner_FarLeftTop:
			return NzVector3f(x, y + height, z);

		case nzCorner_FarRightBottom:
			return NzVector3f(x + width, y, z);

		case nzCorner_FarRightTop:
			return NzVector3f(x + width, y + height, z);

		case nzCorner_NearLeftBottom:
			return NzVector3f(x, y, z + depth);

		case nzCorner_NearLeftTop:
			return NzVector3f(x, y + height, z + depth);

		case nzCorner_NearRightBottom:
			return NzVector3f(x + width, y, z + depth);

		case nzCorner_NearRightTop:
			return NzVector3f(x + width, y + height, z + depth);
	}

	NazaraError("Corner not handled (0x" + NzString::Number(corner, 16) + ')');
	return NzVector3f();
}

template<typename T>
NzSphere<T> NzBox<T>::GetBoundingSphere() const
{
	return NzSphere<T>(GetCenter(), GetRadius());
}

template<typename T>
NzVector3<T> NzBox<T>::GetCenter() const
{
	return GetPosition() + F(0.5)*GetLengths();
}

template<typename T>
NzVector3<T> NzBox<T>::GetLengths() const
{
	return NzVector3<T>(width, height, depth);
}

template<typename T>
NzVector3<T> NzBox<T>::GetMaximum() const
{
	return GetPosition() + GetLengths();
}

template<typename T>
NzVector3<T> NzBox<T>::GetMinimum() const
{
	///DOC: Alias de GetPosition()
	return GetPosition();
}

template<typename T>
NzVector3<T> NzBox<T>::GetNegativeVertex(const NzVector3<T>& normal) const
{
	NzVector3<T> neg(GetPosition());

	if (normal.x < F(0.0))
		neg.x += width;

	if (normal.y < F(0.0))
		neg.y += height;

	if (normal.z < F(0.0))
		neg.z += depth;

	return neg;
}

template<typename T>
NzVector3<T> NzBox<T>::GetPosition() const
{
	return NzVector3<T>(x, y, z);
}

template<typename T>
NzVector3<T> NzBox<T>::GetPositiveVertex(const NzVector3<T>& normal) const
{
	NzVector3<T> pos(GetPosition());

	if (normal.x > F(0.0))
		pos.x += width;

	if (normal.y > F(0.0))
		pos.y += height;

	if (normal.z > F(0.0))
		pos.z += depth;

	return pos;
}

template<typename T>
T NzBox<T>::GetRadius() const
{
	return std::sqrt(GetSquaredRadius());
}

template<typename T>
T NzBox<T>::GetSquaredRadius() const
{
	NzVector3<T> size(GetLengths());
	size *= F(0.5); // La taille étant relative à la position (minimum) de la boite et non pas à son centre

	return size.GetSquaredLength();
}

template<typename T>
bool NzBox<T>::Intersect(const NzBox& box, NzBox* intersection) const
{
	T left = std::max(x, box.x);
	T right = std::min(x + width, box.x + box.width);
	T top = std::max(y, box.y);
	T bottom = std::min(y + height, box.y + box.height);
	T up = std::max(z, box.z);
	T down = std::min(z + depth, box.z + box.depth);

	if (left < right && top < bottom && up < down)
	{
		if (intersection)
		{
			intersection->x = left;
			intersection->y = top;
			intersection->z = up;
			intersection->width = right - left;
			intersection->height = bottom - top;
			intersection->depth = down - up;
		}

		return true;
	}
	else
		return false;
}

template<typename T>
bool NzBox<T>::IsValid() const
{
	return width > F(0.0) && height > F(0.0) && depth > F(0.0);
}

template<typename T>
NzBox<T>& NzBox<T>::MakeZero()
{
	x = F(0.0);
	y = F(0.0);
	z = F(0.0);
	width = F(0.0);
	height = F(0.0);
	depth = F(0.0);

	return *this;
}

template<typename T>
NzBox<T>& NzBox<T>::Set(T Width, T Height, T Depth)
{
	x = F(0.0);
	y = F(0.0);
	z = F(0.0);
	width = Width;
	height = Height;
	depth = Depth;

	return *this;
}

template<typename T>
NzBox<T>& NzBox<T>::Set(T X, T Y, T Z, T Width, T Height, T Depth)
{
	x = X;
	y = Y;
	z = Z;
	width = Width;
	height = Height;
	depth = Depth;

	return *this;
}

template<typename T>
NzBox<T>& NzBox<T>::Set(const T box[6])
{
	x = box[0];
	y = box[1];
	z = box[2];
	width = box[3];
	height = box[4];
	depth = box[5];

	return *this;
}

template<typename T>
NzBox<T>& NzBox<T>::Set(const NzBox& box)
{
	std::memcpy(this, &box, sizeof(NzBox));

	return *this;
}

template<typename T>
NzBox<T>& NzBox<T>::Set(const NzRect<T>& rect)
{
	x = rect.x;
	y = rect.y;
	z = F(0.0);
	width = rect.width;
	height = rect.height;
	depth = F(1.0);

	return *this;
}

template<typename T>
NzBox<T>& NzBox<T>::Set(const NzVector3<T>& size)
{
	return Set(size.x, size.y, size.z);
}

template<typename T>
NzBox<T>& NzBox<T>::Set(const NzVector3<T>& vec1, const NzVector3<T>& vec2)
{
	x = std::min(vec1.x, vec2.x);
	y = std::min(vec1.y, vec2.y);
	z = std::min(vec1.z, vec2.z);
	width = (vec2.x > vec1.x) ? vec2.x-vec1.x : vec1.x-vec2.x;
	height = (vec2.y > vec1.y) ? vec2.y-vec1.y : vec1.y-vec2.y;
	depth = (vec2.z > vec1.z) ? vec2.z-vec1.z : vec1.z-vec2.z;

	return *this;
}

template<typename T>
template<typename U>
NzBox<T>& NzBox<T>::Set(const NzBox<U>& box)
{
	x = F(box.x);
	y = F(box.y);
	z = F(box.z);
	width = F(box.width);
	height = F(box.height);
	depth = F(box.depth);

	return *this;
}

template<typename T>
NzString NzBox<T>::ToString() const
{
	NzStringStream ss;

	return ss << "Box(" << x << ", " << y << ", " << z << ", " << width << ", " << height << ", " << depth << ')';
}

template<typename T>
NzBox<T>& NzBox<T>::Transform(const NzMatrix4<T>& matrix, bool applyTranslation)
{
	NzVector3<T> center = matrix.Transform(GetCenter(), (applyTranslation) ? F(1.0) : F(0.0)); // Valeur multipliant la translation
	NzVector3<T> halfSize = GetLengths() * F(0.5);

	halfSize.Set(std::fabs(matrix(0,0))*halfSize.x + std::fabs(matrix(1,0))*halfSize.y + std::fabs(matrix(2,0))*halfSize.z,
	             std::fabs(matrix(0,1))*halfSize.x + std::fabs(matrix(1,1))*halfSize.y + std::fabs(matrix(2,1))*halfSize.z,
	             std::fabs(matrix(0,2))*halfSize.x + std::fabs(matrix(1,2))*halfSize.y + std::fabs(matrix(2,2))*halfSize.z);

	return Set(center - halfSize, center + halfSize);
}

template<typename T>
T& NzBox<T>::operator[](unsigned int i)
{
	#if NAZARA_MATH_SAFE
	if (i >= 6)
	{
		NzStringStream ss;
		ss << "Index out of range: (" << i << " >= 6)";

		NazaraError(ss);
		throw std::domain_error(ss.ToString());
	}
	#endif

	return *(&x+i);
}

template<typename T>
T NzBox<T>::operator[](unsigned int i) const
{
	#if NAZARA_MATH_SAFE
	if (i >= 6)
	{
		NzStringStream ss;
		ss << "Index out of range: (" << i << " >= 6)";

		NazaraError(ss);
		throw std::domain_error(ss.ToString());
	}
	#endif

	return *(&x+i);
}

template<typename T>
NzBox<T> NzBox<T>::operator*(T scalar) const
{
	return NzBox(x, y, z, width*scalar, height*scalar, depth*scalar);
}

template<typename T>
NzBox<T> NzBox<T>::operator*(const NzVector3<T>& vec) const
{
	return NzBox(x, y, z, width*vec.x, height*vec.y, depth*vec.z);
}

template<typename T>
NzBox<T>& NzBox<T>::operator*=(T scalar)
{
	width *= scalar;
	height *= scalar;
	depth *= scalar;
}

template<typename T>
NzBox<T>& NzBox<T>::operator*=(const NzVector3<T>& vec)
{
	width *= vec.x;
	height *= vec.y;
	depth *= vec.z;
}

template<typename T>
bool NzBox<T>::operator==(const NzBox& box) const
{
	return NzNumberEquals(x, box.x) && NzNumberEquals(y, box.y) && NzNumberEquals(z, box.z) &&
	       NzNumberEquals(width, box.width) &&  NzNumberEquals(height, box.height) && NzNumberEquals(depth, box.depth);
}

template<typename T>
bool NzBox<T>::operator!=(const NzBox& box) const
{
	return !operator==(box);
}

template<typename T>
NzBox<T> NzBox<T>::Lerp(const NzBox& from, const NzBox& to, T interpolation)
{
	#ifdef NAZARA_DEBUG
	if (interpolation < F(0.0) || interpolation > F(1.0))
	{
		NazaraError("Interpolation must be in range [0..1] (Got " + NzString::Number(interpolation) + ')');
		return Zero();
	}
	#endif

	NzBox box;
	box.x = NzLerp(from.x, to.x, interpolation);
	box.y = NzLerp(from.y, to.y, interpolation);
	box.z = NzLerp(from.z, to.z, interpolation);
	box.width = NzLerp(from.width, to.width, interpolation);
	box.height = NzLerp(from.height, to.height, interpolation);
	box.depth = NzLerp(from.depth, to.depth, interpolation);

	return box;
}

template<typename T>
NzBox<T> NzBox<T>::Zero()
{
	NzBox box;
	box.MakeZero();

	return box;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzBox<T>& box)
{
	return out << box.ToString();
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
