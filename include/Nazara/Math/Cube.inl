// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <algorithm>
#include <Nazara/Core/Debug.hpp>

template<typename T>
NzCube<T>::NzCube()
{
}

template<typename T>
NzCube<T>::NzCube(T X, T Y, T Z, T Width, T Height, T Depth) :
x(X),
y(Y),
z(Z),
width(Width),
height(Height),
depth(Depth)
{
}

template<typename T>
NzCube<T>::NzCube(T vec[6]) :
x(vec[0]),
y(vec[1]),
z(vec[2]),
width(vec[3]),
height(vec[4]),
depth(vec[5])
{
}

template<typename T>
NzCube<T>::NzCube(const NzRect<T>& rect) :
x(rect.x),
y(rect.y),
z(0),
width(rect.width),
height(rect.height),
depth(1)
{
}

template<typename T>
template<typename U>
NzCube<T>::NzCube(const NzCube<U>& rect) :
x(static_cast<T>(rect.x)),
y(static_cast<T>(rect.y)),
z(static_cast<T>(rect.z)),
width(static_cast<T>(rect.width)),
height(static_cast<T>(rect.height)),
depth(static_cast<T>(rect.depth))
{
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
	return NzVector3<T>((x+width)/2, (y+height)/2, (z+depth)/2);
}

template<typename T>
bool NzCube<T>::Intersect(const NzCube& rect) const
{
	NzCube intersection; // Optimisé par le compilateur
	return Intersect(rect, intersection);
}

template<typename T>
bool NzCube<T>::Intersect(const NzCube& rect, NzCube& intersection) const
{
	T left = std::max(x, rect.x);
	T right = std::min(x+width, rect.x+rect.width);
	T top = std::max(y, rect.y);
	T bottom = std::min(y+height, rect.y+rect.height);
	T up = std::max(z, rect.z);
	T down = std::min(z+depth, rect.z+rect.depth);

	if (left < right && top < bottom && up < down)
	{
		intersection.x = left;
		intersection.y = top;
		intersection.z = up;
		intersection.width = right-left;
		intersection.height = bottom-top;
		intersection.depth = down-up;

		return true;
	}
	else
		return false;
}

template<typename T>
bool NzCube<T>::IsValid() const
{
	return width > 0 && height > 0 && depth > 0;
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
	if (i >= 6)
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Index out of range (" << i << " >= 4)";

		throw std::domain_error(ss.ToString());
	}

	return *(&x+i);
}

template<typename T>
T NzCube<T>::operator[](unsigned int i) const
{
	if (i >= 6)
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Index out of range (" << i << " >= 4)";

		throw std::domain_error(ss.ToString());
	}

	return *(&x+i);
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzCube<T>& rect)
{
	return out << rect.ToString();
}

#include <Nazara/Core/DebugOff.hpp>
