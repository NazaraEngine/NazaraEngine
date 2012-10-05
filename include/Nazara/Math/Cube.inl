// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <algorithm>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)

template<typename T>
NzCube<T>::NzCube()
{
}

template<typename T>
NzCube<T>::NzCube(T X, T Y, T Z, T Width, T Height, T Depth)
{
	Set(X, Y, Z, Width, Height, Depth);
}

template<typename T>
NzCube<T>::NzCube(const T vec[6])
{
	Set(vec);
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
template<typename U>
NzCube<T>::NzCube(const NzCube<U>& rect)
{
	Set(rect);
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
bool NzCube<T>::Contains(const NzCube<T>& rect) const
{
	return Contains(rect.x, rect.y, rect.z) &&
		   Contains(rect.x + rect.width, rect.y + rect.height, rect.z + rect.depth);
}

template<typename T>
void NzCube<T>::ExtendTo(const NzVector3<T>& point)
{
	x = std::min(x, point.x);
	y = std::min(y, point.y);
	z = std::min(z, point.z);
	width = std::max(x+width, point.x)-x;
	height = std::max(y+height, point.x)-y;
	depth = std::max(z+depth, point.x)-z;
}

template<typename T>
void NzCube<T>::ExtendTo(const NzCube& rect)
{
	x = std::min(x, rect.x);
	y = std::min(y, rect.y);
	z = std::min(y, rect.z);
	width = std::max(x+width, rect.x+rect.width)-x;
	height = std::max(x+height, rect.y+rect.height)-y;
	depth = std::max(x+depth, rect.z+rect.depth)-z;
}

template<typename T>
NzVector3<T> NzCube<T>::GetCenter() const
{
	return NzVector3<T>((x+width)/F(2.0), (y+height)/F(2.0), (z+depth)/F(2.0));
}

template<typename T>
bool NzCube<T>::Intersect(const NzCube& rect, NzCube* intersection) const
{
	T left = std::max(x, rect.x);
	T right = std::min(x+width, rect.x+rect.width);
	T top = std::max(y, rect.y);
	T bottom = std::min(y+height, rect.y+rect.height);
	T up = std::max(z, rect.z);
	T down = std::min(z+depth, rect.z+rect.depth);

	if (left < right && top < bottom && up < down)
	{
		if (intersection)
		{
			intersection->x = left;
			intersection->y = top;
			intersection->z = up;
			intersection->width = right-left;
			intersection->height = bottom-top;
			intersection->depth = down-up;
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
void NzCube<T>::Set(T X, T Y, T Z, T Width, T Height, T Depth)
{
	x = X;
	y = Y;
	z = Z;
	width = Width;
	height = Height;
	depth = Depth;
}

template<typename T>
void NzCube<T>::Set(const T rect[6])
{
	x = rect[0];
	y = rect[1];
	z = rect[2];
	width = rect[3];
	height = rect[4];
	depth = rect[5];
}

template<typename T>
void NzCube<T>::Set(const NzRect<T>& rect)
{
	x = rect.x;
	y = rect.y;
	z = 0;
	width = rect.width;
	height = rect.height;
	depth = 1;
}

template<typename T>
void NzCube<T>::Set(const NzVector3<T>& vec1, const NzVector3<T>& vec2)
{
	x = std::min(vec1.x, vec2.x);
	y = std::min(vec1.y, vec2.y);
	z = std::min(vec1.z, vec2.z);
	width = (vec2.x > vec1.x) ? vec2.x-vec1.x : vec1.x-vec2.x;
	height = (vec2.y > vec1.y) ? vec2.y-vec1.y : vec1.y-vec2.y;
	depth = (vec2.z > vec1.z) ? vec2.z-vec1.z : vec1.z-vec2.z;
}

template<typename T>
template<typename U>
void NzCube<T>::Set(const NzCube<U>& cube)
{
	x = F(cube.x);
	y = F(cube.y);
	z = F(cube.z);
	width = F(cube.width);
	height = F(cube.height);
	depth = F(cube.depth);
}

template<typename T>
NzString NzCube<T>::ToString() const
{
	NzStringStream ss;

	return ss << "Cube(" << x << ", " << y << ", " << z << ", " << width << ", " << height << ", " << depth << ')';
}

template<typename T>
NzCube<T>::operator NzString() const
{
	return ToString();
}

template<typename T>
T& NzCube<T>::operator[](unsigned int i)
{
	#if NAZARA_MATH_SAFE
	if (i >= 6)
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Index out of range (" << i << " >= 6)";

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
		ss << __FILE__ << ':' << __LINE__ << ": Index out of range (" << i << " >= 6)";

		throw std::domain_error(ss.ToString());
	}
	#endif

	return *(&x+i);
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzCube<T>& rect)
{
	return out << rect.ToString();
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
