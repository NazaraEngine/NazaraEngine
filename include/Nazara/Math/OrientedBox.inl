// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Basic.hpp>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

///DOC: Pour que les coins soient valides, la méthode Update doit être appelée

#define F(a) static_cast<T>(a)

template<typename T>
NzOrientedBox<T>::NzOrientedBox(T X, T Y, T Z, T Width, T Height, T Depth)
{
	Set(X, Y, Z, Width, Height, Depth);
}

template<typename T>
NzOrientedBox<T>::NzOrientedBox(const NzBox<T>& box)
{
	Set(box);
}

template<typename T>
NzOrientedBox<T>::NzOrientedBox(const NzVector3<T>& vec1, const NzVector3<T>& vec2)
{
	Set(vec1, vec2);
}

template<typename T>
template<typename U>
NzOrientedBox<T>::NzOrientedBox(const NzOrientedBox<U>& orientedBox)
{
	Set(orientedBox);
}

template<typename T>
const NzVector3<T>& NzOrientedBox<T>::GetCorner(nzCorner corner) const
{
	#ifdef NAZARA_DEBUG
	if (corner > nzCorner_Max)
	{
		NazaraError("Corner not handled (0x" + NzString::Number(corner, 16) + ')');

		static NzVector3<T> dummy;
		return dummy;
	}
	#endif

	return m_corners[corner];
}

template<typename T>
bool NzOrientedBox<T>::IsValid() const
{
	return localBox.IsValid();
}

template<typename T>
NzOrientedBox<T>& NzOrientedBox<T>::MakeZero()
{
	localBox.MakeZero();

	return *this;
}

template<typename T>
NzOrientedBox<T>& NzOrientedBox<T>::Set(T X, T Y, T Z, T Width, T Height, T Depth)
{
	localBox.Set(X, Y, Z, Width, Height, Depth);

	return *this;
}

template<typename T>
NzOrientedBox<T>& NzOrientedBox<T>::Set(const NzBox<T>& box)
{
	localBox.Set(box);

	return *this;
}

template<typename T>
NzOrientedBox<T>& NzOrientedBox<T>::Set(const NzOrientedBox& orientedBox)
{
	std::memcpy(this, &orientedBox, sizeof(NzOrientedBox));

	return *this;
}

template<typename T>
NzOrientedBox<T>& NzOrientedBox<T>::Set(const NzVector3<T>& vec1, const NzVector3<T>& vec2)
{
	localBox.Set(vec1, vec2);

	return *this;
}

template<typename T>
template<typename U>
NzOrientedBox<T>& NzOrientedBox<T>::Set(const NzOrientedBox<U>& orientedBox)
{
	for (unsigned int i = 0; i <= nzCorner_Max; ++i)
		m_corners[i].Set(orientedBox.m_corners[i]);

	localBox = orientedBox.localBox;

	return *this;
}

template<typename T>
NzString NzOrientedBox<T>::ToString() const
{
	NzStringStream ss;

	return ss << "OrientedBox(FLB: " << m_corners[nzCorner_FarLeftBottom].ToString() << "\n"
	          << "            FLT: " << m_corners[nzCorner_FarLeftTop].ToString() << "\n"
	          << "            FRB: " << m_corners[nzCorner_FarRightBottom].ToString() << "\n"
	          << "            FRT: " << m_corners[nzCorner_FarRightTop].ToString() << "\n"
	          << "            NLB: " << m_corners[nzCorner_NearLeftBottom].ToString() << "\n"
	          << "            NLT: " << m_corners[nzCorner_NearLeftTop].ToString() << "\n"
	          << "            NRB: " << m_corners[nzCorner_NearRightBottom].ToString() << "\n"
	          << "            NRT: " << m_corners[nzCorner_NearRightTop].ToString() << ")\n";
}

template<typename T>
void NzOrientedBox<T>::Update(const NzMatrix4<T>& transformMatrix)
{
	for (unsigned int i = 0; i <= nzCorner_Max; ++i)
		m_corners[i] = transformMatrix.Transform(localBox.GetCorner(static_cast<nzCorner>(i)));
}

template<typename T>
NzOrientedBox<T>::operator NzVector3<T>*()
{
	return &m_corners[0];
}

template<typename T>
NzOrientedBox<T>::operator const NzVector3<T>*() const
{
	return &m_corners[0];
}

template<typename T>
NzVector3<T>& NzOrientedBox<T>::operator()(unsigned int i)
{
	#if NAZARA_MATH_SAFE
	if (i > nzCorner_Max)
	{
		NzStringStream ss;
		ss << "Index out of range: (" << i << " >= 3)";

		NazaraError(ss);
		throw std::out_of_range(ss.ToString());
	}
	#endif

	return m_corners[i];
}

template<typename T>
NzVector3<T> NzOrientedBox<T>::operator()(unsigned int i) const
{
	#if NAZARA_MATH_SAFE
	if (i > nzCorner_Max)
	{
		NzStringStream ss;
		ss << "Index out of range: (" << i << " >= 3)";

		NazaraError(ss);
		throw std::out_of_range(ss.ToString());
	}
	#endif

	return m_corners[i];
}

template<typename T>
NzOrientedBox<T> NzOrientedBox<T>::operator*(T scalar) const
{
	NzOrientedBox box(*this);
	box *= scalar;

	return box;
}

template<typename T>
NzOrientedBox<T>& NzOrientedBox<T>::operator*=(T scalar)
{
	localBox *= scalar;

	return *this;
}

template<typename T>
bool NzOrientedBox<T>::operator==(const NzOrientedBox& box) const
{
	return localBox == box.localBox;
}

template<typename T>
bool NzOrientedBox<T>::operator!=(const NzOrientedBox& box) const
{
	return !operator==(box);
}

template<typename T>
NzOrientedBox<T> NzOrientedBox<T>::Lerp(const NzOrientedBox& from, const NzOrientedBox& to, T interpolation)
{
	NzOrientedBox orientedBox;
	orientedBox.Set(NzBox<T>::Lerp(from.localBox, to.localBox, interpolation));

	return orientedBox;
}

template<typename T>
NzOrientedBox<T> NzOrientedBox<T>::Zero()
{
	NzOrientedBox orientedBox;
	orientedBox.MakeZero();

	return orientedBox;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzOrientedBox<T>& orientedBox)
{
	return out << orientedBox.ToString();
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
