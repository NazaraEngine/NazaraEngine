// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Basic.hpp>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)

template<typename T>
NzOrientedCube<T>::NzOrientedCube(T X, T Y, T Z, T Width, T Height, T Depth)
{
	Set(X, Y, Z, Width, Height, Depth);
}

template<typename T>
NzOrientedCube<T>::NzOrientedCube(const NzCube<T>& cube)
{
	Set(cube);
}

template<typename T>
NzOrientedCube<T>::NzOrientedCube(const NzVector3<T>& vec1, const NzVector3<T>& vec2)
{
	Set(vec1, vec2);
}

template<typename T>
template<typename U>
NzOrientedCube<T>::NzOrientedCube(const NzOrientedCube<U>& orientedCube)
{
	Set(orientedCube);
}

template<typename T>
const NzVector3<T>& NzOrientedCube<T>::GetCorner(nzCorner corner) const
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
bool NzOrientedCube<T>::IsValid() const
{
	return localCube.IsValid();
}

template<typename T>
NzOrientedCube<T>& NzOrientedCube<T>::MakeZero()
{
	localCube.MakeZero();

	return *this;
}

template<typename T>
NzOrientedCube<T>& NzOrientedCube<T>::Set(T X, T Y, T Z, T Width, T Height, T Depth)
{
	localCube.Set(X, Y, Z, Width, Height, Depth);

	return *this;
}

template<typename T>
NzOrientedCube<T>& NzOrientedCube<T>::Set(const NzCube<T>& cube)
{
	localCube.Set(cube);

	return *this;
}

template<typename T>
NzOrientedCube<T>& NzOrientedCube<T>::Set(const NzOrientedCube& orientedCube)
{
	std::memcpy(this, &orientedCube, sizeof(NzOrientedCube));

	return *this;
}

template<typename T>
NzOrientedCube<T>& NzOrientedCube<T>::Set(const NzVector3<T>& vec1, const NzVector3<T>& vec2)
{
	localCube.Set(vec1, vec2);

	return *this;
}

template<typename T>
template<typename U>
NzOrientedCube<T>& NzOrientedCube<T>::Set(const NzOrientedCube<U>& orientedCube)
{
	for (unsigned int i = 0; i <= nzCorner_Max; ++i)
		m_corners[i].Set(orientedCube.m_corners[i]);

	localCube = orientedCube.localCube;

	return *this;
}

template<typename T>
NzString NzOrientedCube<T>::ToString() const
{
	NzStringStream ss;

	return ss << "OrientedCube(FLB: " << m_corners[nzCorner_FarLeftBottom].ToString() << "\n"
	          << "             FLT: " << m_corners[nzCorner_FarLeftTop].ToString() << "\n"
	          << "             FRB: " << m_corners[nzCorner_FarRightBottom].ToString() << "\n"
	          << "             FRT: " << m_corners[nzCorner_FarRightTop].ToString() << "\n"
	          << "             NLB: " << m_corners[nzCorner_NearLeftBottom].ToString() << "\n"
	          << "             NLT: " << m_corners[nzCorner_NearLeftTop].ToString() << "\n"
	          << "             NRB: " << m_corners[nzCorner_NearRightBottom].ToString() << "\n"
	          << "             NRT: " << m_corners[nzCorner_NearRightTop].ToString() << ")\n";
}

template<typename T>
void NzOrientedCube<T>::Update(const NzMatrix4<T>& transformMatrix)
{
	for (unsigned int i = 0; i <= nzCorner_Max; ++i)
		m_corners[i] = transformMatrix.Transform(localCube.GetCorner(static_cast<nzCorner>(i)));
}

template<typename T>
NzOrientedCube<T>::operator NzVector3<T>*()
{
	return &m_corners[0];
}

template<typename T>
NzOrientedCube<T>::operator const NzVector3<T>*() const
{
	return &m_corners[0];
}

template<typename T>
NzVector3<T>& NzOrientedCube<T>::operator()(unsigned int i)
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

	return &m_corners[i];
}

template<typename T>
NzVector3<T> NzOrientedCube<T>::operator()(unsigned int i) const
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

	return &m_corners[i];
}

template<typename T>
NzOrientedCube<T> NzOrientedCube<T>::operator*(T scalar) const
{
	NzOrientedCube box(*this);
	box *= scalar;

	return box;
}

template<typename T>
NzOrientedCube<T>& NzOrientedCube<T>::operator*=(T scalar)
{
	localCube *= scalar;

	return *this;
}

template<typename T>
bool NzOrientedCube<T>::operator==(const NzOrientedCube& box) const
{
	return localCube == box.localCube;
}

template<typename T>
bool NzOrientedCube<T>::operator!=(const NzOrientedCube& box) const
{
	return !operator==(box);
}

template<typename T>
NzOrientedCube<T> NzOrientedCube<T>::Lerp(const NzOrientedCube& from, const NzOrientedCube& to, T interpolation)
{
	NzOrientedCube orientedCube;
	orientedCube.Set(NzCube<T>::Lerp(from.localCube, to.localCube, interpolation));

	return orientedCube;
}

template<typename T>
NzOrientedCube<T> NzOrientedCube<T>::Zero()
{
	NzOrientedCube orientedCube;
	orientedCube.MakeZero();

	return orientedCube;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzOrientedCube<T>& orientedCube)
{
	return out << orientedCube.ToString();
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
