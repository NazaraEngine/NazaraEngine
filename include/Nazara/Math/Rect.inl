// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Basic.hpp>
#include <algorithm>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)

template<typename T>
NzRect<T>::NzRect(T Width, T Height)
{
	Set(Width, Height);
}

template<typename T>
NzRect<T>::NzRect(T X, T Y, T Width, T Height)
{
	Set(X, Y, Width, Height);
}

template<typename T>
NzRect<T>::NzRect(const T vec[4])
{
	Set(vec);
}

template<typename T>
NzRect<T>::NzRect(const NzVector2<T>& lengths)
{
	Set(lengths);
}

template<typename T>
NzRect<T>::NzRect(const NzVector2<T>& vec1, const NzVector2<T>& vec2)
{
	Set(vec1, vec2);
}

template<typename T>
template<typename U>
NzRect<T>::NzRect(const NzRect<U>& rect)
{
	Set(rect);
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
NzRect<T>& NzRect<T>::ExtendTo(T X, T Y)
{
	width = std::max(x + width, X);
	height = std::max(y + height, Y);

	x = std::min(x, X);
	y = std::min(y, Y);

	width -= x;
	height -= y;

	return *this;
}

template<typename T>
NzRect<T>& NzRect<T>::ExtendTo(const NzVector2<T>& point)
{
	return ExtendTo(point.x, point.y);
}

template<typename T>
NzRect<T>& NzRect<T>::ExtendTo(const NzRect& rect)
{
	width = std::max(x + width, rect.x + rect.width);
	height = std::max(y + height, rect.y + rect.height);

	x = std::min(x, rect.x);
	y = std::min(y, rect.y);

	width -= x;
	height -= y;

	return *this;
}

template<typename T>
NzVector2<T> NzRect<T>::GetCenter() const
{
	return GetPosition() + GetLengths() / F(2.0);
}

template<typename T>
NzVector2<T> NzRect<T>::GetLengths() const
{
	return NzVector2<T>(width, height);
}

template<typename T>
NzVector2<T> NzRect<T>::GetMaximum() const
{
	return GetPosition() + GetLengths();
}

template<typename T>
NzVector2<T> NzRect<T>::GetMinimum() const
{
	///DOC: Alias de GetPosition()
	return GetPosition();
}

template<typename T>
NzVector2<T> NzRect<T>::GetNegativeVertex(const NzVector2<T>& normal) const
{
	NzVector2<T> neg(GetPosition());

	if (normal.x < F(0.0))
		neg.x += width;

	if (normal.y < F(0.0))
		neg.y += height;

	return neg;
}

template<typename T>
NzVector2<T> NzRect<T>::GetPosition() const
{
	return NzVector2<T>(x, y);
}

template<typename T>
NzVector2<T> NzRect<T>::GetPositiveVertex(const NzVector2<T>& normal) const
{
	NzVector2<T> pos(GetPosition());

	if (normal.x > F(0.0))
		pos.x += width;

	if (normal.y > F(0.0))
		pos.y += height;

	return pos;
}

template<typename T>
bool NzRect<T>::Intersect(const NzRect& rect, NzRect* intersection) const
{
	T left = std::max(x, rect.x);
	T right = std::min(x+width, rect.x+rect.width);
	T top = std::max(y, rect.y);
	T bottom = std::min(y+height, rect.y+rect.height);

	if (left < right && top < bottom)
	{
		if (intersection)
		{
			intersection->x = left;
			intersection->y = top;
			intersection->width = right-left;
			intersection->height = bottom-top;
		}

		return true;
	}
	else
		return false;
}

template<typename T>
bool NzRect<T>::IsValid() const
{
	return width > F(0.0) && height > F(0.0);
}

template<typename T>
NzRect<T>& NzRect<T>::MakeZero()
{
	x = F(0.0);
	y = F(0.0);
	width = F(0.0);
	height = F(0.0);

	return *this;
}

template<typename T>
NzRect<T>& NzRect<T>::Set(T Width, T Height)
{
	x = F(0.0);
	y = F(0.0);
	width = Width;
	height = Height;

	return *this;
}

template<typename T>
NzRect<T>& NzRect<T>::Set(T X, T Y, T Width, T Height)
{
	x = X;
	y = Y;
	width = Width;
	height = Height;

	return *this;
}

template<typename T>
NzRect<T>& NzRect<T>::Set(const T rect[4])
{
	x = rect[0];
	y = rect[1];
	width = rect[2];
	height = rect[3];

	return *this;
}

template<typename T>
NzRect<T>& NzRect<T>::Set(const NzRect<T>& rect)
{
	std::memcpy(this, &rect, sizeof(NzRect));

	return *this;
}

template<typename T>
NzRect<T>& NzRect<T>::Set(const NzVector2<T>& lengths)
{
	return Set(lengths.x, lengths.y);
}

template<typename T>
NzRect<T>& NzRect<T>::Set(const NzVector2<T>& vec1, const NzVector2<T>& vec2)
{
	x = std::min(vec1.x, vec2.x);
	y = std::min(vec1.y, vec2.y);
	width = (vec2.x > vec1.x) ? vec2.x-vec1.x : vec1.x-vec2.x;
	height = (vec2.y > vec1.y) ? vec2.y-vec1.y : vec1.y-vec2.y;

	return *this;
}

template<typename T>
template<typename U>
NzRect<T>& NzRect<T>::Set(const NzRect<U>& rect)
{
	x = F(rect.x);
	y = F(rect.y);
	width = F(rect.width);
	height = F(rect.height);

	return *this;
}

template<typename T>
NzString NzRect<T>::ToString() const
{
	NzStringStream ss;

	return ss << "Rect(" << x << ", " << y << ", " << width << ", " << height << ')';
}

template<typename T>
NzRect<T>& NzRect<T>::Translate(const NzVector2<T>& translation)
{
	x += translation.x;
	y += translation.y;

	return *this;
}

template<typename T>
T& NzRect<T>::operator[](unsigned int i)
{
	#if NAZARA_MATH_SAFE
	if (i >= 4)
	{
		NzStringStream ss;
		ss << "Index out of range: (" << i << " >= 4)";

		NazaraError(ss);
		throw std::domain_error(ss.ToString());
	}
	#endif

	return *(&x+i);
}

template<typename T>
T NzRect<T>::operator[](unsigned int i) const
{
	#if NAZARA_MATH_SAFE
	if (i >= 4)
	{
		NzStringStream ss;
		ss << "Index out of range: (" << i << " >= 4)";

		NazaraError(ss);
		throw std::domain_error(ss.ToString());
	}
	#endif

	return *(&x+i);
}

template<typename T>
NzRect<T> NzRect<T>::operator*(T scalar) const
{
	return NzRect(x, y, width*scalar, height*scalar);
}

template<typename T>
NzRect<T> NzRect<T>::operator*(const NzVector2<T>& vec) const
{
	return NzRect(x, y, width*vec.x, height*vec.y);
}

template<typename T>
NzRect<T> NzRect<T>::operator/(T scalar) const
{
	return NzRect(x, y, width/scalar, height/scalar);
}

template<typename T>
NzRect<T> NzRect<T>::operator/(const NzVector2<T>& vec) const
{
	return NzRect(x, y, width/vec.x, height/vec.y);
}

template<typename T>
NzRect<T>& NzRect<T>::operator*=(T scalar)
{
	width *= scalar;
	height *= scalar;
	return *this;
}

template<typename T>
NzRect<T>& NzRect<T>::operator*=(const NzVector2<T>& vec)
{
	width *= vec.x;
	height *= vec.y;
	return *this;
}

template<typename T>
NzRect<T>& NzRect<T>::operator/=(T scalar)
{
	width /= scalar;
	height /= scalar;

	return *this;
}

template<typename T>
NzRect<T>& NzRect<T>::operator/=(const NzVector2<T>& vec)
{
	width /= vec.x;
	height /= vec.y;

	return *this;
}

template<typename T>
bool NzRect<T>::operator==(const NzRect& rect) const
{
	return NzNumberEquals(x, rect.x) && NzNumberEquals(y, rect.y) &&
	       NzNumberEquals(width, rect.width) &&  NzNumberEquals(height, rect.height);
}

template<typename T>
bool NzRect<T>::operator!=(const NzRect& rect) const
{
	return !operator==(rect);
}

template<typename T>
NzRect<T> NzRect<T>::Lerp(const NzRect& from, const NzRect& to, T interpolation)
{
	#ifdef NAZARA_DEBUG
	if (interpolation < F(0.0) || interpolation > F(1.0))
	{
		NazaraError("Interpolation must be in range [0..1] (Got " + NzString::Number(interpolation) + ')');
		return Zero();
	}
	#endif

	NzRect rect;
	rect.x = NzLerp(from.x, to.x, interpolation);
	rect.y = NzLerp(from.y, to.y, interpolation);
	rect.width = NzLerp(from.width, to.width, interpolation);
	rect.height = NzLerp(from.height, to.height, interpolation);

	return rect;
}

template<typename T>
NzRect<T> NzRect<T>::Zero()
{
	NzRect rect;
	rect.MakeZero();

	return rect;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzRect<T>& rect)
{
	return out << rect.ToString();
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
