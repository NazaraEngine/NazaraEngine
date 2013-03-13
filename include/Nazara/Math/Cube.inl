// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Basic.hpp>
#include <algorithm>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)

template<typename T>
NzCube<T>::NzCube(T X, T Y, T Z, T Width, T Height, T Depth)
{
	Set(X, Y, Z, Width, Height, Depth);
}

template<typename T>
NzCube<T>::NzCube(const NzRect<T>& rect)
{
	Set(rect);
}

template<typename T>
NzCube<T>::NzCube(const NzVector3<T>& vec1, const NzVector3<T>& vec2)
{
	Set(vec1, vec2);
}

template<typename T>
NzCube<T>::NzCube(const T vec[6])
{
	Set(vec);
}

template<typename T>
template<typename U>
NzCube<T>::NzCube(const NzCube<U>& cube)
{
	Set(cube);
}

template<typename T>
bool NzCube<T>::Contains(T X, T Y, T Z) const
{
	return X >= x && X < x+width &&
	       Y >= y && Y < y+height &&
	       Z >= z && Z < z+depth;
}

template<typename T>
bool NzCube<T>::Contains(const NzVector3<T>& point) const
{
	return Contains(point.x, point.y, point.z);
}

template<typename T>
bool NzCube<T>::Contains(const NzCube<T>& cube) const
{
	return Contains(cube.x, cube.y, cube.z) &&
		   Contains(cube.x + cube.width, cube.y + cube.height, cube.z + cube.depth);
}

template<typename T>
NzCube<T>& NzCube<T>::ExtendTo(T X, T Y, T Z)
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
NzCube<T>& NzCube<T>::ExtendTo(const NzVector3<T>& point)
{
	return ExtendTo(point.x, point.y, point.z);
}

template<typename T>
NzCube<T>& NzCube<T>::ExtendTo(const NzCube& cube)
{
	width = std::max(x + width, cube.x + cube.width);
	height = std::max(y + height, cube.y + cube.height);
	depth = std::max(z + depth, cube.z + cube.depth);

	x = std::min(x, cube.x);
	y = std::min(y, cube.y);
	z = std::min(z, cube.z);

	width -= x;
	height -= y;
	depth -= z;

	return *this;
}

template<typename T>
NzVector3<T> NzCube<T>::GetCorner(nzCorner corner) const
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
NzSphere<T> NzCube<T>::GetBoundingSphere() const
{
	return NzSphere<T>(GetCenter(), GetRadius());
}

template<typename T>
NzVector3<T> NzCube<T>::GetCenter() const
{
	return GetPosition() + F(0.5)*GetSize();
}

template<typename T>
NzVector3<T> NzCube<T>::GetNegativeVertex(const NzVector3<T>& normal) const
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
NzVector3<T> NzCube<T>::GetPosition() const
{
	return NzVector3<T>(x, y, z);
}

template<typename T>
NzVector3<T> NzCube<T>::GetPositiveVertex(const NzVector3<T>& normal) const
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
T NzCube<T>::GetRadius() const
{
	return std::sqrt(GetSquaredRadius());
}

template<typename T>
NzVector3<T> NzCube<T>::GetSize() const
{
	return NzVector3<T>(width, height, depth);
}

template<typename T>
T NzCube<T>::GetSquaredRadius() const
{
	NzVector3<T> size(GetSize());
	size *= F(0.5); // La taille étant relative à la position (minimum) du cube et non pas à son centre

	return size.GetSquaredLength();
}

template<typename T>
bool NzCube<T>::Intersect(const NzCube& cube, NzCube* intersection) const
{
	T left = std::max(x, cube.x);
	T right = std::min(x + width, cube.x + cube.width);
	T top = std::max(y, cube.y);
	T bottom = std::min(y + height, cube.y + cube.height);
	T up = std::max(z, cube.z);
	T down = std::min(z + depth, cube.z + cube.depth);

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
bool NzCube<T>::IsValid() const
{
	return width > F(0.0) && height > F(0.0) && depth > F(0.0);
}

template<typename T>
NzCube<T>& NzCube<T>::MakeZero()
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
NzCube<T>& NzCube<T>::Set(T X, T Y, T Z, T Width, T Height, T Depth)
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
NzCube<T>& NzCube<T>::Set(const T cube[6])
{
	x = cube[0];
	y = cube[1];
	z = cube[2];
	width = cube[3];
	height = cube[4];
	depth = cube[5];

	return *this;
}

template<typename T>
NzCube<T>& NzCube<T>::Set(const NzCube& cube)
{
	std::memcpy(this, &cube, sizeof(NzCube));

	return *this;
}

template<typename T>
NzCube<T>& NzCube<T>::Set(const NzRect<T>& rect)
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
NzCube<T>& NzCube<T>::Set(const NzVector3<T>& vec1, const NzVector3<T>& vec2)
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
NzCube<T>& NzCube<T>::Set(const NzCube<U>& cube)
{
	x = F(cube.x);
	y = F(cube.y);
	z = F(cube.z);
	width = F(cube.width);
	height = F(cube.height);
	depth = F(cube.depth);

	return *this;
}

template<typename T>
NzString NzCube<T>::ToString() const
{
	NzStringStream ss;

	return ss << "Cube(" << x << ", " << y << ", " << z << ", " << width << ", " << height << ", " << depth << ')';
}

template<typename T>
NzCube<T>& NzCube<T>::Transform(const NzMatrix4<T>& matrix, bool applyTranslation)
{
	NzVector3<T> center = matrix.Transform(GetCenter(), (applyTranslation) ? F(1.0) : F(0.0)); // Valeur multipliant la translation
	NzVector3<T> halfSize = GetSize() * F(0.5);

	halfSize.Set(std::fabs(matrix(0,0))*halfSize.x + std::fabs(matrix(1,0))*halfSize.y + std::fabs(matrix(2,0))*halfSize.z,
	             std::fabs(matrix(0,1))*halfSize.x + std::fabs(matrix(1,1))*halfSize.y + std::fabs(matrix(2,1))*halfSize.z,
	             std::fabs(matrix(0,2))*halfSize.x + std::fabs(matrix(1,2))*halfSize.y + std::fabs(matrix(2,2))*halfSize.z);

	return Set(center - halfSize, center + halfSize);
}

template<typename T>
T& NzCube<T>::operator[](unsigned int i)
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
T NzCube<T>::operator[](unsigned int i) const
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
NzCube<T> NzCube<T>::operator*(T scalar) const
{
	return NzCube(x, y, z, width*scalar, height*scalar, depth*scalar);
}

template<typename T>
NzCube<T>& NzCube<T>::operator*=(T scalar)
{
	width *= scalar;
	height *= scalar;
	depth *= scalar;
}

template<typename T>
bool NzCube<T>::operator==(const NzCube& cube) const
{
	return NzNumberEquals(x, cube.x) && NzNumberEquals(y, cube.y) && NzNumberEquals(z, cube.z) &&
	       NzNumberEquals(width, cube.width) &&  NzNumberEquals(height, cube.height) && NzNumberEquals(depth, cube.depth);
}

template<typename T>
bool NzCube<T>::operator!=(const NzCube& cube) const
{
	return !operator==(cube);
}

template<typename T>
NzCube<T> NzCube<T>::Lerp(const NzCube& from, const NzCube& to, T interpolation)
{
	#ifdef NAZARA_DEBUG
	if (interpolation < F(0.0) || interpolation > F(1.0))
	{
		NazaraError("Interpolation must be in range [0..1] (Got " + NzString::Number(interpolation) + ')');
		return Zero();
	}
	#endif

	NzCube cube;
	cube.x = NzLerp(from.x, to.x, interpolation);
	cube.y = NzLerp(from.y, to.y, interpolation);
	cube.z = NzLerp(from.z, to.z, interpolation);
	cube.width = NzLerp(from.width, to.width, interpolation);
	cube.height = NzLerp(from.height, to.height, interpolation);
	cube.depth = NzLerp(from.depth, to.depth, interpolation);

	return cube;
}

template<typename T>
NzCube<T> NzCube<T>::Zero()
{
	NzCube cube;
	cube.MakeZero();

	return cube;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzCube<T>& cube)
{
	return out << cube.ToString();
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
