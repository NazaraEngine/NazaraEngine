// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <algorithm>
#include <Nazara/Core/Debug.hpp>

template<typename T>
NzRect<T>::NzRect()
{
}

template<typename T>
NzRect<T>::NzRect(T X, T Y, T Width, T Height) :
x(X),
y(Y),
width(Width),
height(Height)
{
}

template<typename T>
NzRect<T>::NzRect(T vec[4]) :
x(vec[0]),
y(vec[1]),
width(vec[2]),
height(vec[3])
{
}

template<typename T>
template<typename U>
NzRect<T>::NzRect(const NzRect<U>& rect) :
x(static_cast<T>(rect.x)),
y(static_cast<T>(rect.y)),
width(static_cast<T>(rect.width)),
height(static_cast<T>(rect.height))
{
}

template<typename T>
bool NzRect<T>::Contains(T X, T Y) const
{
	return X >= x && X < x+width &&
	       Y >= y && Y < y+height;
}

template<typename T>
bool NzRect<T>::Contains(const NzVector2<T>& point) const
{
	return Contains(point.x, point.y);
}

template<typename T>
bool NzRect<T>::Contains(const NzRect<T>& rect) const
{
	return Contains(rect.x, rect.y) &&
		   Contains(rect.x + rect.width, rect.y + rect.height);
}

template<typename T>
void NzRect<T>::ExtendTo(const NzVector2<T>& point)
{
	x = std::min(x, point.x);
	y = std::min(y, point.y);
	width = std::max(x+width, point.x)-x;
	height = std::max(y+height, point.y)-y;
}

template<typename T>
void NzRect<T>::ExtendTo(const NzRect& rect)
{
	x = std::min(x, rect.x);
	y = std::min(y, rect.y);
	width = std::max(x+width, rect.x+rect.width)-x;
	height = std::max(x+height, rect.y+rect.height)-y;
}

template<typename T>
NzVector2<T> NzRect<T>::GetCenter() const
{
	return NzVector2<T>((x+width)/2, (y+height)/2);
}

template<typename T>
bool NzRect<T>::Intersect(const NzRect& rect) const
{
	NzRect intersection; // Optimisé par le compilateur
	return Intersect(rect, intersection);
}

template<typename T>
bool NzRect<T>::Intersect(const NzRect& rect, NzRect& intersection) const
{
	T left = std::max(x, rect.x);
	T right = std::min(x+width, rect.x+rect.width);
	T top = std::max(y, rect.y);
	T bottom = std::min(y+height, rect.y+rect.height);

	if (left < right && top < bottom)
	{
		intersection.x = left;
		intersection.y = top;
		intersection.width = right-left;
		intersection.height = bottom-top;

		return true;
	}
	else
		return false;
}

template<typename T>
bool NzRect<T>::IsValid() const
{
	return width > 0 && height > 0;
}

template<typename T>
NzString NzRect<T>::ToString() const
{
	NzStringStream ss;

	return ss << "Rect(" << x << ", " << y << ", " << width << ", " << height << ')';
}

template<typename T>
NzRect<T>::operator NzString() const
{
	return ToString();
}

template<typename T>
T& NzRect<T>::operator[](unsigned int i)
{
	if (i >= 4)
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Index out of range (" << i << " >= 4)";

		throw std::domain_error(ss.ToString());
	}

	return *(&x+i);
}

template<typename T>
T NzRect<T>::operator[](unsigned int i) const
{
	if (i >= 4)
	{
		NzStringStream ss;
		ss << __FILE__ << ':' << __LINE__ << ": Index out of range (" << i << " >= 4)";

		throw std::domain_error(ss.ToString());
	}

	return *(&x+i);
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzRect<T>& rect)
{
	return out << rect.ToString();
}

#include <Nazara/Core/DebugOff.hpp>
