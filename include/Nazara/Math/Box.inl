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
	Box<T>::Box(T Width, T Height, T Depth)
	{
		Set(Width, Height, Depth);
	}

	template<typename T>
	Box<T>::Box(T X, T Y, T Z, T Width, T Height, T Depth)
	{
		Set(X, Y, Z, Width, Height, Depth);
	}

	template<typename T>
	Box<T>::Box(const Rect<T>& rect)
	{
		Set(rect);
	}

	template<typename T>
	Box<T>::Box(const Vector3<T>& lengths)
	{
		Set(lengths);
	}

	template<typename T>
	Box<T>::Box(const Vector3<T>& vec1, const Vector3<T>& vec2)
	{
		Set(vec1, vec2);
	}

	template<typename T>
	Box<T>::Box(const T vec[6])
	{
		Set(vec);
	}

	template<typename T>
	template<typename U>
	Box<T>::Box(const Box<U>& box)
	{
		Set(box);
	}

	template<typename T>
	bool Box<T>::Contains(T X, T Y, T Z) const
	{
		return X >= x && X <= x+width &&
			   Y >= y && Y <= y+height &&
			   Z >= z && Z <= z+depth;
	}

	template<typename T>
	bool Box<T>::Contains(const Box<T>& box) const
	{
		return Contains(box.x, box.y, box.z) &&
			   Contains(box.x + box.width, box.y + box.height, box.z + box.depth);
	}

	template<typename T>
	bool Box<T>::Contains(const Vector3<T>& point) const
	{
		return Contains(point.x, point.y, point.z);
	}

	template<typename T>
	Box<T>& Box<T>::ExtendTo(T X, T Y, T Z)
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
	Box<T>& Box<T>::ExtendTo(const Box& box)
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
	Box<T>& Box<T>::ExtendTo(const Vector3<T>& point)
	{
		return ExtendTo(point.x, point.y, point.z);
	}

	template<typename T>
	Vector3<T> Box<T>::GetCorner(BoxCorner corner) const
	{
		switch (corner)
		{
			case BoxCorner_FarLeftBottom:
				return Vector3<T>(x, y, z);

			case BoxCorner_FarLeftTop:
				return Vector3<T>(x, y + height, z);

			case BoxCorner_FarRightBottom:
				return Vector3<T>(x + width, y, z);

			case BoxCorner_FarRightTop:
				return Vector3<T>(x + width, y + height, z);

			case BoxCorner_NearLeftBottom:
				return Vector3<T>(x, y, z + depth);

			case BoxCorner_NearLeftTop:
				return Vector3<T>(x, y + height, z + depth);

			case BoxCorner_NearRightBottom:
				return Vector3<T>(x + width, y, z + depth);

			case BoxCorner_NearRightTop:
				return Vector3<T>(x + width, y + height, z + depth);
		}

		NazaraError("Corner not handled (0x" + String::Number(corner, 16) + ')');
		return Vector3<T>();
	}

	template<typename T>
	Sphere<T> Box<T>::GetBoundingSphere() const
	{
		return Sphere<T>(GetCenter(), GetRadius());
	}

	template<typename T>
	Vector3<T> Box<T>::GetCenter() const
	{
		return GetPosition() + GetLengths()/F(2.0);
	}

	template<typename T>
	Vector3<T> Box<T>::GetLengths() const
	{
		return Vector3<T>(width, height, depth);
	}

	template<typename T>
	Vector3<T> Box<T>::GetMaximum() const
	{
		return GetPosition() + GetLengths();
	}

	template<typename T>
	Vector3<T> Box<T>::GetMinimum() const
	{
		///DOC: Alias de GetPosition()
		return GetPosition();
	}

	template<typename T>
	Vector3<T> Box<T>::GetNegativeVertex(const Vector3<T>& normal) const
	{
		Vector3<T> neg(GetPosition());

		if (normal.x < F(0.0))
			neg.x += width;

		if (normal.y < F(0.0))
			neg.y += height;

		if (normal.z < F(0.0))
			neg.z += depth;

		return neg;
	}

	template<typename T>
	Vector3<T> Box<T>::GetPosition() const
	{
		return Vector3<T>(x, y, z);
	}

	template<typename T>
	Vector3<T> Box<T>::GetPositiveVertex(const Vector3<T>& normal) const
	{
		Vector3<T> pos(GetPosition());

		if (normal.x > F(0.0))
			pos.x += width;

		if (normal.y > F(0.0))
			pos.y += height;

		if (normal.z > F(0.0))
			pos.z += depth;

		return pos;
	}

	template<typename T>
	T Box<T>::GetRadius() const
	{
		return std::sqrt(GetSquaredRadius());
	}

	template<typename T>
	Sphere<T> Box<T>::GetSquaredBoundingSphere() const
	{
		return Sphere<T>(GetCenter(), GetSquaredRadius());
	}

	template<typename T>
	T Box<T>::GetSquaredRadius() const
	{
		Vector3<T> size(GetLengths());
		size /= F(2.0); // La taille étant relative à la position (minimum) de la boite et non pas à son centre

		return size.GetSquaredLength();
	}

	template<typename T>
	bool Box<T>::Intersect(const Box& box, Box* intersection) const
	{
		T left = std::max(x, box.x);
		T right = std::min(x + width, box.x + box.width);
		if (left >= right)
			return false;

		T top = std::max(y, box.y);
		T bottom = std::min(y + height, box.y + box.height);
		if (top >= bottom)
			return false;

		T up = std::max(z, box.z);
		T down = std::min(z + depth, box.z + box.depth);
		if (up >= down)
			return false;

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

	template<typename T>
	bool Box<T>::IsValid() const
	{
		return width > F(0.0) && height > F(0.0) && depth > F(0.0);
	}

	template<typename T>
	Box<T>& Box<T>::MakeZero()
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
	Box<T>& Box<T>::Set(T Width, T Height, T Depth)
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
	Box<T>& Box<T>::Set(T X, T Y, T Z, T Width, T Height, T Depth)
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
	Box<T>& Box<T>::Set(const T box[6])
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
	Box<T>& Box<T>::Set(const Box& box)
	{
		std::memcpy(this, &box, sizeof(Box));

		return *this;
	}

	template<typename T>
	Box<T>& Box<T>::Set(const Rect<T>& rect)
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
	Box<T>& Box<T>::Set(const Vector3<T>& lengths)
	{
		return Set(lengths.x, lengths.y, lengths.z);
	}

	template<typename T>
	Box<T>& Box<T>::Set(const Vector3<T>& vec1, const Vector3<T>& vec2)
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
	Box<T>& Box<T>::Set(const Box<U>& box)
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
	String Box<T>::ToString() const
	{
		StringStream ss;

		return ss << "Box(" << x << ", " << y << ", " << z << ", " << width << ", " << height << ", " << depth << ')';
	}

	template<typename T>
	Box<T>& Box<T>::Transform(const Matrix4<T>& matrix, bool applyTranslation)
	{
		Vector3<T> center = matrix.Transform(GetCenter(), (applyTranslation) ? F(1.0) : F(0.0)); // Valeur multipliant la translation
		Vector3<T> halfSize = GetLengths()/F(2.0);

		halfSize.Set(std::abs(matrix(0,0))*halfSize.x + std::abs(matrix(1,0))*halfSize.y + std::abs(matrix(2,0))*halfSize.z,
					 std::abs(matrix(0,1))*halfSize.x + std::abs(matrix(1,1))*halfSize.y + std::abs(matrix(2,1))*halfSize.z,
					 std::abs(matrix(0,2))*halfSize.x + std::abs(matrix(1,2))*halfSize.y + std::abs(matrix(2,2))*halfSize.z);

		return Set(center - halfSize, center + halfSize);
	}

	template<typename T>
	Box<T>& Box<T>::Translate(const Vector3<T>& translation)
	{
		x += translation.x;
		y += translation.y;
		z += translation.z;

		return *this;
	}

	template<typename T>
	T& Box<T>::operator[](unsigned int i)
	{
		#if NAZARA_MATH_SAFE
		if (i >= 6)
		{
			StringStream ss;
			ss << "Index out of range: (" << i << " >= 6)";

			NazaraError(ss);
			throw std::domain_error(ss.ToString());
		}
		#endif

		return *(&x+i);
	}

	template<typename T>
	T Box<T>::operator[](unsigned int i) const
	{
		#if NAZARA_MATH_SAFE
		if (i >= 6)
		{
			StringStream ss;
			ss << "Index out of range: (" << i << " >= 6)";

			NazaraError(ss);
			throw std::domain_error(ss.ToString());
		}
		#endif

		return *(&x+i);
	}

	template<typename T>
	Box<T> Box<T>::operator*(T scalar) const
	{
		return Box(x, y, z, width*scalar, height*scalar, depth*scalar);
	}

	template<typename T>
	Box<T> Box<T>::operator*(const Vector3<T>& vec) const
	{
		return Box(x, y, z, width*vec.x, height*vec.y, depth*vec.z);
	}

	template<typename T>
	Box<T>& Box<T>::operator*=(T scalar)
	{
		width *= scalar;
		height *= scalar;
		depth *= scalar;

		return *this;
	}

	template<typename T>
	Box<T>& Box<T>::operator*=(const Vector3<T>& vec)
	{
		width *= vec.x;
		height *= vec.y;
		depth *= vec.z;

		return *this;
	}

	template<typename T>
	bool Box<T>::operator==(const Box& box) const
	{
		return NumberEquals(x, box.x) && NumberEquals(y, box.y) && NumberEquals(z, box.z) &&
			   NumberEquals(width, box.width) && NumberEquals(height, box.height) && NumberEquals(depth, box.depth);
	}

	template<typename T>
	bool Box<T>::operator!=(const Box& box) const
	{
		return !operator==(box);
	}

	template<typename T>
	Box<T> Box<T>::Lerp(const Box& from, const Box& to, T interpolation)
	{
		#ifdef NAZARA_DEBUG
		if (interpolation < F(0.0) || interpolation > F(1.0))
		{
			NazaraError("Interpolation must be in range [0..1] (Got " + String::Number(interpolation) + ')');
			return Zero();
		}
		#endif

		Box box;
		box.x = Nz::Lerp(from.x, to.x, interpolation);
		box.y = Nz::Lerp(from.y, to.y, interpolation);
		box.z = Nz::Lerp(from.z, to.z, interpolation);
		box.width = Nz::Lerp(from.width, to.width, interpolation);
		box.height = Nz::Lerp(from.height, to.height, interpolation);
		box.depth = Nz::Lerp(from.depth, to.depth, interpolation);

		return box;
	}

	template<typename T>
	Box<T> Box<T>::Zero()
	{
		Box box;
		box.MakeZero();

		return box;
	}
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nz::Box<T>& box)
{
	return out << box.ToString();
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
