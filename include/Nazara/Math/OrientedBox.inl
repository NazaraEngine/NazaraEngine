// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

///DOC: Pour que les coins soient valides, la méthode Update doit être appelée

#define F(a) static_cast<T>(a)

namespace Nz
{
	template<typename T>
	OrientedBox<T>::OrientedBox(T X, T Y, T Z, T Width, T Height, T Depth)
	{
		Set(X, Y, Z, Width, Height, Depth);
	}

	template<typename T>
	OrientedBox<T>::OrientedBox(const Box<T>& box)
	{
		Set(box);
	}

	template<typename T>
	OrientedBox<T>::OrientedBox(const Vector3<T>& vec1, const Vector3<T>& vec2)
	{
		Set(vec1, vec2);
	}

	template<typename T>
	template<typename U>
	OrientedBox<T>::OrientedBox(const OrientedBox<U>& orientedBox)
	{
		Set(orientedBox);
	}

	template<typename T>
	const Vector3<T>& OrientedBox<T>::GetCorner(BoxCorner corner) const
	{
		#ifdef NAZARA_DEBUG
		if (corner > BoxCorner_Max)
		{
			NazaraError("Corner not handled (0x" + String::Number(corner, 16) + ')');

			static Vector3<T> dummy;
			return dummy;
		}
		#endif

		return m_corners[corner];
	}

	template<typename T>
	bool OrientedBox<T>::IsValid() const
	{
		return localBox.IsValid();
	}

	template<typename T>
	OrientedBox<T>& OrientedBox<T>::MakeZero()
	{
		localBox.MakeZero();

		return *this;
	}

	template<typename T>
	OrientedBox<T>& OrientedBox<T>::Set(T X, T Y, T Z, T Width, T Height, T Depth)
	{
		localBox.Set(X, Y, Z, Width, Height, Depth);

		return *this;
	}

	template<typename T>
	OrientedBox<T>& OrientedBox<T>::Set(const Box<T>& box)
	{
		localBox.Set(box);

		return *this;
	}

	template<typename T>
	OrientedBox<T>& OrientedBox<T>::Set(const OrientedBox& orientedBox)
	{
		std::memcpy(this, &orientedBox, sizeof(OrientedBox));

		return *this;
	}

	template<typename T>
	OrientedBox<T>& OrientedBox<T>::Set(const Vector3<T>& vec1, const Vector3<T>& vec2)
	{
		localBox.Set(vec1, vec2);

		return *this;
	}

	template<typename T>
	template<typename U>
	OrientedBox<T>& OrientedBox<T>::Set(const OrientedBox<U>& orientedBox)
	{
		for (unsigned int i = 0; i <= BoxCorner_Max; ++i)
			m_corners[i].Set(orientedBox(i));

		localBox.Set(orientedBox.localBox);

		return *this;
	}

	template<typename T>
	String OrientedBox<T>::ToString() const
	{
		StringStream ss;

		return ss << "OrientedBox(FLB: " << m_corners[BoxCorner_FarLeftBottom].ToString() << "\n"
				  << "            FLT: " << m_corners[BoxCorner_FarLeftTop].ToString() << "\n"
				  << "            FRB: " << m_corners[BoxCorner_FarRightBottom].ToString() << "\n"
				  << "            FRT: " << m_corners[BoxCorner_FarRightTop].ToString() << "\n"
				  << "            NLB: " << m_corners[BoxCorner_NearLeftBottom].ToString() << "\n"
				  << "            NLT: " << m_corners[BoxCorner_NearLeftTop].ToString() << "\n"
				  << "            NRB: " << m_corners[BoxCorner_NearRightBottom].ToString() << "\n"
				  << "            NRT: " << m_corners[BoxCorner_NearRightTop].ToString() << ")\n";
	}

	template<typename T>
	void OrientedBox<T>::Update(const Matrix4<T>& transformMatrix)
	{
		for (unsigned int i = 0; i <= BoxCorner_Max; ++i)
			m_corners[i] = transformMatrix.Transform(localBox.GetCorner(static_cast<BoxCorner>(i)));
	}

	template<typename T>
	void OrientedBox<T>::Update(const Vector3<T>& translation)
	{
		for (unsigned int i = 0; i <= BoxCorner_Max; ++i)
			m_corners[i] = localBox.GetCorner(static_cast<BoxCorner>(i)) + translation;
	}

	template<typename T>
	OrientedBox<T>::operator Vector3<T>*()
	{
		return &m_corners[0];
	}

	template<typename T>
	OrientedBox<T>::operator const Vector3<T>*() const
	{
		return &m_corners[0];
	}

	template<typename T>
	Vector3<T>& OrientedBox<T>::operator()(unsigned int i)
	{
		#if NAZARA_MATH_SAFE
		if (i > BoxCorner_Max)
		{
			StringStream ss;
			ss << "Index out of range: (" << i << " >= " << BoxCorner_Max << ")";

			NazaraError(ss);
			throw std::out_of_range(ss.ToString());
		}
		#endif

		return m_corners[i];
	}

	template<typename T>
	Vector3<T> OrientedBox<T>::operator()(unsigned int i) const
	{
		#if NAZARA_MATH_SAFE
		if (i > BoxCorner_Max)
		{
			StringStream ss;
			ss << "Index out of range: (" << i << " >= " << BoxCorner_Max << ")";

			NazaraError(ss);
			throw std::out_of_range(ss.ToString());
		}
		#endif

		return m_corners[i];
	}

	template<typename T>
	OrientedBox<T> OrientedBox<T>::operator*(T scalar) const
	{
		OrientedBox box(*this);
		box *= scalar;

		return box;
	}

	template<typename T>
	OrientedBox<T>& OrientedBox<T>::operator*=(T scalar)
	{
		localBox *= scalar;

		return *this;
	}

	template<typename T>
	bool OrientedBox<T>::operator==(const OrientedBox& box) const
	{
		return localBox == box.localBox;
	}

	template<typename T>
	bool OrientedBox<T>::operator!=(const OrientedBox& box) const
	{
		return !operator==(box);
	}

	template<typename T>
	OrientedBox<T> OrientedBox<T>::Lerp(const OrientedBox& from, const OrientedBox& to, T interpolation)
	{
		OrientedBox orientedBox;
		orientedBox.Set(Box<T>::Lerp(from.localBox, to.localBox, interpolation));

		return orientedBox;
	}

	template<typename T>
	OrientedBox<T> OrientedBox<T>::Zero()
	{
		OrientedBox orientedBox;
		orientedBox.MakeZero();

		return orientedBox;
	}
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nz::OrientedBox<T>& orientedBox)
{
	return out << orientedBox.ToString();
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
