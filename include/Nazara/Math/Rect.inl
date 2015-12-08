// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <algorithm>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)

namespace Nz
{
	template<typename T>
	Rect<T>::Rect(T Width, T Height)
	{
		Set(Width, Height);
	}

	template<typename T>
	Rect<T>::Rect(T X, T Y, T Width, T Height)
	{
		Set(X, Y, Width, Height);
	}

	template<typename T>
	Rect<T>::Rect(const T vec[4])
	{
		Set(vec);
	}

	template<typename T>
	Rect<T>::Rect(const Vector2<T>& lengths)
	{
		Set(lengths);
	}

	template<typename T>
	Rect<T>::Rect(const Vector2<T>& vec1, const Vector2<T>& vec2)
	{
		Set(vec1, vec2);
	}

	template<typename T>
	template<typename U>
	Rect<T>::Rect(const Rect<U>& rect)
	{
		Set(rect);
	}

	template<typename T>
	bool Rect<T>::Contains(T X, T Y) const
	{
		return X >= x && X <= x+width &&
			   Y >= y && Y <= y+height;
	}

	template<typename T>
	bool Rect<T>::Contains(const Vector2<T>& point) const
	{
		return Contains(point.x, point.y);
	}

	template<typename T>
	bool Rect<T>::Contains(const Rect<T>& rect) const
	{
		return Contains(rect.x, rect.y) &&
			   Contains(rect.x + rect.width, rect.y + rect.height);
	}

	template<typename T>
	Rect<T>& Rect<T>::ExtendTo(T X, T Y)
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
	Rect<T>& Rect<T>::ExtendTo(const Vector2<T>& point)
	{
		return ExtendTo(point.x, point.y);
	}

	template<typename T>
	Rect<T>& Rect<T>::ExtendTo(const Rect& rect)
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
	Vector2<T> Rect<T>::GetCenter() const
	{
		return GetPosition() + GetLengths() / F(2.0);
	}

	template<typename T>
	Vector2<T> Rect<T>::GetCorner(RectCorner corner) const
	{
		switch (corner)
		{
			case RectCorner_LeftBottom:
				return Vector2<T>(x, y + height);

			case RectCorner_LeftTop:
				return Vector2<T>(x, y);

			case RectCorner_RightBottom:
				return Vector2<T>(x + width, y + height);

			case RectCorner_RightTop:
				return Vector2<T>(x + width, y);
		}

		NazaraError("Corner not handled (0x" + String::Number(corner, 16) + ')');
		return Vector2<T>();
	}

	template<typename T>
	Vector2<T> Rect<T>::GetLengths() const
	{
		return Vector2<T>(width, height);
	}

	template<typename T>
	Vector2<T> Rect<T>::GetMaximum() const
	{
		return GetPosition() + GetLengths();
	}

	template<typename T>
	Vector2<T> Rect<T>::GetMinimum() const
	{
		///DOC: Alias de GetPosition()
		return GetPosition();
	}

	template<typename T>
	Vector2<T> Rect<T>::GetNegativeVertex(const Vector2<T>& normal) const
	{
		Vector2<T> neg(GetPosition());

		if (normal.x < F(0.0))
			neg.x += width;

		if (normal.y < F(0.0))
			neg.y += height;

		return neg;
	}

	template<typename T>
	Vector2<T> Rect<T>::GetPosition() const
	{
		return Vector2<T>(x, y);
	}

	template<typename T>
	Vector2<T> Rect<T>::GetPositiveVertex(const Vector2<T>& normal) const
	{
		Vector2<T> pos(GetPosition());

		if (normal.x > F(0.0))
			pos.x += width;

		if (normal.y > F(0.0))
			pos.y += height;

		return pos;
	}

	template<typename T>
	bool Rect<T>::Intersect(const Rect& rect, Rect* intersection) const
	{
		T left = std::max(x, rect.x);
		T right = std::min(x + width, rect.x + rect.width);
		if (left >= right)
			return false;

		T top = std::max(y, rect.y);
		T bottom = std::min(y + height, rect.y + rect.height);
		if (top >= bottom)
			return false;

		if (intersection)
		{
			intersection->x = left;
			intersection->y = top;
			intersection->width = right - left;
			intersection->height = bottom - top;
		}

		return true;
	}

	template<typename T>
	bool Rect<T>::IsValid() const
	{
		return width > F(0.0) && height > F(0.0);
	}

	template<typename T>
	Rect<T>& Rect<T>::MakeZero()
	{
		x = F(0.0);
		y = F(0.0);
		width = F(0.0);
		height = F(0.0);

		return *this;
	}

	template<typename T>
	Rect<T>& Rect<T>::Set(T Width, T Height)
	{
		x = F(0.0);
		y = F(0.0);
		width = Width;
		height = Height;

		return *this;
	}

	template<typename T>
	Rect<T>& Rect<T>::Set(T X, T Y, T Width, T Height)
	{
		x = X;
		y = Y;
		width = Width;
		height = Height;

		return *this;
	}

	template<typename T>
	Rect<T>& Rect<T>::Set(const T rect[4])
	{
		x = rect[0];
		y = rect[1];
		width = rect[2];
		height = rect[3];

		return *this;
	}

	template<typename T>
	Rect<T>& Rect<T>::Set(const Rect<T>& rect)
	{
		std::memcpy(this, &rect, sizeof(Rect));

		return *this;
	}

	template<typename T>
	Rect<T>& Rect<T>::Set(const Vector2<T>& lengths)
	{
		return Set(lengths.x, lengths.y);
	}

	template<typename T>
	Rect<T>& Rect<T>::Set(const Vector2<T>& vec1, const Vector2<T>& vec2)
	{
		x = std::min(vec1.x, vec2.x);
		y = std::min(vec1.y, vec2.y);
		width = (vec2.x > vec1.x) ? vec2.x-vec1.x : vec1.x-vec2.x;
		height = (vec2.y > vec1.y) ? vec2.y-vec1.y : vec1.y-vec2.y;

		return *this;
	}

	template<typename T>
	template<typename U>
	Rect<T>& Rect<T>::Set(const Rect<U>& rect)
	{
		x = F(rect.x);
		y = F(rect.y);
		width = F(rect.width);
		height = F(rect.height);

		return *this;
	}

	template<typename T>
	String Rect<T>::ToString() const
	{
		StringStream ss;

		return ss << "Rect(" << x << ", " << y << ", " << width << ", " << height << ')';
	}

	template<typename T>
	Rect<T>& Rect<T>::Translate(const Vector2<T>& translation)
	{
		x += translation.x;
		y += translation.y;

		return *this;
	}

	template<typename T>
	T& Rect<T>::operator[](unsigned int i)
	{
		#if NAZARA_MATH_SAFE
		if (i >= 4)
		{
			StringStream ss;
			ss << "Index out of range: (" << i << " >= 4)";

			NazaraError(ss);
			throw std::domain_error(ss.ToString());
		}
		#endif

		return *(&x+i);
	}

	template<typename T>
	T Rect<T>::operator[](unsigned int i) const
	{
		#if NAZARA_MATH_SAFE
		if (i >= 4)
		{
			StringStream ss;
			ss << "Index out of range: (" << i << " >= 4)";

			NazaraError(ss);
			throw std::domain_error(ss.ToString());
		}
		#endif

		return *(&x+i);
	}

	template<typename T>
	Rect<T> Rect<T>::operator*(T scalar) const
	{
		return Rect(x, y, width*scalar, height*scalar);
	}

	template<typename T>
	Rect<T> Rect<T>::operator*(const Vector2<T>& vec) const
	{
		return Rect(x, y, width*vec.x, height*vec.y);
	}

	template<typename T>
	Rect<T> Rect<T>::operator/(T scalar) const
	{
		return Rect(x, y, width/scalar, height/scalar);
	}

	template<typename T>
	Rect<T> Rect<T>::operator/(const Vector2<T>& vec) const
	{
		return Rect(x, y, width/vec.x, height/vec.y);
	}

	template<typename T>
	Rect<T>& Rect<T>::operator*=(T scalar)
	{
		width *= scalar;
		height *= scalar;

		return *this;
	}

	template<typename T>
	Rect<T>& Rect<T>::operator*=(const Vector2<T>& vec)
	{
		width *= vec.x;
		height *= vec.y;

		return *this;
	}

	template<typename T>
	Rect<T>& Rect<T>::operator/=(T scalar)
	{
		width /= scalar;
		height /= scalar;

		return *this;
	}

	template<typename T>
	Rect<T>& Rect<T>::operator/=(const Vector2<T>& vec)
	{
		width /= vec.x;
		height /= vec.y;

		return *this;
	}

	template<typename T>
	bool Rect<T>::operator==(const Rect& rect) const
	{
		return NumberEquals(x, rect.x) && NumberEquals(y, rect.y) &&
			   NumberEquals(width, rect.width) && NumberEquals(height, rect.height);
	}

	template<typename T>
	bool Rect<T>::operator!=(const Rect& rect) const
	{
		return !operator==(rect);
	}

	template<typename T>
	Rect<T> Rect<T>::Lerp(const Rect& from, const Rect& to, T interpolation)
	{
		#ifdef NAZARA_DEBUG
		if (interpolation < F(0.0) || interpolation > F(1.0))
		{
			NazaraError("Interpolation must be in range [0..1] (Got " + String::Number(interpolation) + ')');
			return Zero();
		}
		#endif

		Rect rect;
		rect.x = Lerp(from.x, to.x, interpolation);
		rect.y = Lerp(from.y, to.y, interpolation);
		rect.width = Lerp(from.width, to.width, interpolation);
		rect.height = Lerp(from.height, to.height, interpolation);

		return rect;
	}

	template<typename T>
	Rect<T> Rect<T>::Zero()
	{
		Rect rect;
		rect.MakeZero();

		return rect;
	}
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nz::Rect<T>& rect)
{
	return out << rect.ToString();
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
