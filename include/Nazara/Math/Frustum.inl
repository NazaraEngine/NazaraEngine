// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Sources:
// http://www.crownandcutlass.com/features/technicaldetails/frustum.html
// http://www.lighthouse3d.com/tutorials/view-frustum-culling/

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Basic.hpp>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)

template<typename T>
template<typename U>
NzFrustum<T>::NzFrustum(const NzFrustum<U>& frustum)
{
	Set(frustum);
}

template<typename T>
NzFrustum<T>& NzFrustum<T>::Build(T angle, T ratio, T zNear, T zFar, const NzVector3<T>& eye, const NzVector3<T>& target, const NzVector3<T>& up)
{
	#if NAZARA_MATH_ANGLE_RADIAN
	angle *= F(0.5);
	#else
	angle = NzDegreeToRadian(angle * F(0.5));
	#endif

	T tangent = std::tan(angle);
	T nearH = zNear * tangent;
	T nearW = nearH * ratio;
	T farH = zFar * tangent;
	T farW = farH * ratio;

	NzVector3<T> f = NzVector3<T>::Normalize(target - eye);
	NzVector3<T> u(up.GetNormal());
	NzVector3<T> s = NzVector3<T>::Normalize(f.CrossProduct(u));
	u = s.CrossProduct(f);

	NzVector3<T> nc = eye + f * zNear;
	NzVector3<T> fc = eye + f * zFar;

	// Calcul du frustum
	m_corners[nzCorner_FarLeftBottom] = fc - u*farH - s*farW;
	m_corners[nzCorner_FarLeftTop] = fc + u*farH - s*farW;
	m_corners[nzCorner_FarRightTop] = fc + u*farH + s*farW;
	m_corners[nzCorner_FarRightBottom] = fc - u*farH + s*farW;

	m_corners[nzCorner_NearLeftBottom] = nc - u*nearH - s*nearW;
	m_corners[nzCorner_NearLeftTop] = nc + u*nearH - s*nearW;
	m_corners[nzCorner_NearRightTop] = nc + u*nearH + s*nearW;
	m_corners[nzCorner_NearRightBottom] = nc - u*nearH + s*nearW;

	// Construction des plans du frustum
	m_planes[nzFrustumPlane_Bottom].Set(m_corners[nzCorner_NearLeftBottom], m_corners[nzCorner_NearRightBottom], m_corners[nzCorner_FarRightBottom]);
	m_planes[nzFrustumPlane_Far].Set(m_corners[nzCorner_FarRightTop], m_corners[nzCorner_FarLeftTop], m_corners[nzCorner_FarLeftBottom]);
	m_planes[nzFrustumPlane_Left].Set(m_corners[nzCorner_NearLeftTop], m_corners[nzCorner_NearLeftBottom], m_corners[nzCorner_FarLeftBottom]);
	m_planes[nzFrustumPlane_Near].Set(m_corners[nzCorner_NearLeftTop], m_corners[nzCorner_NearRightTop], m_corners[nzCorner_NearRightBottom]);
	m_planes[nzFrustumPlane_Right].Set(m_corners[nzCorner_NearRightBottom], m_corners[nzCorner_NearRightTop], m_corners[nzCorner_FarRightBottom]);
	m_planes[nzFrustumPlane_Top].Set(m_corners[nzCorner_NearRightTop], m_corners[nzCorner_NearLeftTop], m_corners[nzCorner_FarLeftTop]);

	return *this;
}

template<typename T>
bool NzFrustum<T>::Contains(const NzCube<T>& cube)
{
	// http://www.lighthouse3d.com/tutorials/view-frustum-culling/geometric-approach-testing-boxes-ii/
	for(unsigned int i = 0; i <= nzFrustumPlane_Max; i++)
	{
		if (m_planes[i].Distance(cube.GetPositiveVertex(m_planes[i].normal)) < F(0.0))
			return false;
	}

	return true;
}

template<typename T>
bool NzFrustum<T>::Contains(const NzSphere<T>& sphere)
{
	for(unsigned int i = 0; i <= nzFrustumPlane_Max; i++)
	{
		if (m_planes[i].Distance(sphere.GetPosition()) < -sphere.radius)
			return false;
	}

	return true;
}

template<typename T>
bool NzFrustum<T>::Contains(const NzVector3<T>& point)
{
	for(unsigned int i = 0; i <= nzFrustumPlane_Max; ++i)
	{
		if (m_planes[i].Distance(point) < F(0.0))
			return false;
	}

	return true;
}

template<typename T>
bool NzFrustum<T>::Contains(const NzVector3<T>* points, unsigned int pointCount)
{
 	for (unsigned int i = 0; i <= nzFrustumPlane_Max; ++i)
	{
		unsigned int j;
		for (j = 0; j < pointCount; j++ )
		{
			if (m_planes[i].Distance(points[j]) > F(0.0))
				break;
		}

		if (j == pointCount)
			return false;
	}

	return true;
}

template<typename T>
NzFrustum<T>& NzFrustum<T>::Extract(const NzMatrix4<T>& clipMatrix)
{
	///TODO: Calculer les coins
	// http://www.crownandcutlass.com/features/technicaldetails/frustum.html
	T plane[4];
	T invLength;

	// Extract the numbers for the RIGHT plane
	plane[0] = clipMatrix[ 3] - clipMatrix[ 0];
	plane[1] = clipMatrix[ 7] - clipMatrix[ 4];
	plane[2] = clipMatrix[11] - clipMatrix[ 8];
	plane[3] = clipMatrix[15] - clipMatrix[12];

	// Normalize the result
	invLength = F(1.0) / std::sqrt(plane[0]*plane[0] + plane[1]*plane[1] + plane[2]*plane[2]);
	plane[0] *= invLength;
	plane[1] *= invLength;
	plane[2] *= invLength;
	plane[3] *= invLength;

	m_planes[nzFrustumPlane_Right].Set(plane);

	// Extract the numbers for the LEFT plane
	plane[0] = clipMatrix[ 3] + clipMatrix[ 0];
	plane[1] = clipMatrix[ 7] + clipMatrix[ 4];
	plane[2] = clipMatrix[11] + clipMatrix[ 8];
	plane[3] = clipMatrix[15] + clipMatrix[12];

	// Normalize the result
	invLength = F(1.0) / std::sqrt(plane[0]*plane[0] + plane[1]*plane[1] + plane[2]*plane[2]);
	plane[0] *= invLength;
	plane[1] *= invLength;
	plane[2] *= invLength;
	plane[3] *= invLength;

	m_planes[nzFrustumPlane_Left].Set(plane);

	// Extract the BOTTOM plane
	plane[0] = clipMatrix[ 3] + clipMatrix[ 1];
	plane[1] = clipMatrix[ 7] + clipMatrix[ 5];
	plane[2] = clipMatrix[11] + clipMatrix[ 9];
	plane[3] = clipMatrix[15] + clipMatrix[13];

	// Normalize the result
	invLength = F(1.0) / std::sqrt(plane[0]*plane[0] + plane[1]*plane[1] + plane[2]*plane[2]);
	plane[0] *= invLength;
	plane[1] *= invLength;
	plane[2] *= invLength;
	plane[3] *= invLength;

	m_planes[nzFrustumPlane_Bottom].Set(plane);

	// Extract the TOP plane
	plane[0] = clipMatrix[ 3] - clipMatrix[ 1];
	plane[1] = clipMatrix[ 7] - clipMatrix[ 5];
	plane[2] = clipMatrix[11] - clipMatrix[ 9];
	plane[3] = clipMatrix[15] - clipMatrix[13];

	// Normalize the result
	invLength = F(1.0) / std::sqrt(plane[0]*plane[0] + plane[1]*plane[1] + plane[2]*plane[2]);
	plane[0] *= invLength;
	plane[1] *= invLength;
	plane[2] *= invLength;
	plane[3] *= invLength;

	m_planes[nzFrustumPlane_Top].Set(plane);

	// Extract the FAR plane
	plane[0] = clipMatrix[ 3] - clipMatrix[ 2];
	plane[1] = clipMatrix[ 7] - clipMatrix[ 6];
	plane[2] = clipMatrix[11] - clipMatrix[10];
	plane[3] = clipMatrix[15] - clipMatrix[14];

	// Normalize the result
	invLength = F(1.0) / std::sqrt(plane[0]*plane[0] + plane[1]*plane[1] + plane[2]*plane[2]);
	plane[0] *= invLength;
	plane[1] *= invLength;
	plane[2] *= invLength;
	plane[3] *= invLength;

	m_planes[nzFrustumPlane_Far].Set(plane);

	// Extract the NEAR plane
	plane[0] = clipMatrix[ 3] + clipMatrix[ 2];
	plane[1] = clipMatrix[ 7] + clipMatrix[ 6];
	plane[2] = clipMatrix[11] + clipMatrix[10];
	plane[3] = clipMatrix[15] + clipMatrix[14];

	// Normalize the result
	invLength = F(1.0) / std::sqrt(plane[0]*plane[0] + plane[1]*plane[1] + plane[2]*plane[2]);
	plane[0] *= invLength;
	plane[1] *= invLength;
	plane[2] *= invLength;
	plane[3] *= invLength;

	m_planes[nzFrustumPlane_Near].Set(plane);

	// Une fois les plans extraits, il faut extraire les points du frustum
	// Je me base sur cette page: http://www.gamedev.net/topic/393309-calculating-the-view-frustums-vertices/

	NzMatrix4f invClipMatrix;
	clipMatrix.GetInverse(&invClipMatrix); // Nous n'avons plus besoin de la matrice originale

	NzVector4f corner;

	// FarLeftBottom
	corner.Set(F(-1.0), F(-1.0), F(1.0));
	corner = invClipMatrix.Transform(corner);
	corner.Normalize();

	m_corners[nzCorner_FarLeftBottom] = NzVector3<T>(corner.x, corner.y, corner.z);

	// FarLeftTop
	corner.Set(F(-1.0), F(1.0), F(1.0));
	corner = invClipMatrix.Transform(corner);
	corner.Normalize();

	m_corners[nzCorner_FarLeftTop] = NzVector3<T>(corner.x, corner.y, corner.z);

	// FarRightBottom
	corner.Set(F(1.0), F(-1.0), F(1.0));
	corner = invClipMatrix.Transform(corner);
	corner.Normalize();

	m_corners[nzCorner_FarRightBottom] = NzVector3<T>(corner.x, corner.y, corner.z);

	// FarRightTop
	corner.Set(F(1.0), F(1.0), F(1.0));
	corner = invClipMatrix.Transform(corner);
	corner.Normalize();

	m_corners[nzCorner_FarRightTop] = NzVector3<T>(corner.x, corner.y, corner.z);

	// NearLeftBottom
	corner.Set(F(-1.0), F(-1.0), F(0.0));
	corner = invClipMatrix.Transform(corner);
	corner.Normalize();

	m_corners[nzCorner_NearLeftBottom] = NzVector3<T>(corner.x, corner.y, corner.z);

	// NearLeftTop
	corner.Set(F(-1.0), F(1.0), F(0.0));
	corner = invClipMatrix.Transform(corner);
	corner.Normalize();

	m_corners[nzCorner_NearLeftTop] = NzVector3<T>(corner.x, corner.y, corner.z);

	// NearRightBottom
	corner.Set(F(1.0), F(-1.0), F(0.0));
	corner = invClipMatrix.Transform(corner);
	corner.Normalize();

	m_corners[nzCorner_NearRightBottom] = NzVector3<T>(corner.x, corner.y, corner.z);

	// NearRightTop
	corner.Set(F(1.0), F(1.0), F(0.0));
	corner = invClipMatrix.Transform(corner);
	corner.Normalize();

	m_corners[nzCorner_NearRightTop] = NzVector3<T>(corner.x, corner.y, corner.z);

	return *this;
}

template<typename T>
NzFrustum<T>& NzFrustum<T>::Extract(const NzMatrix4<T>& view, const NzMatrix4<T>& projection)
{
	NzMatrix4f clipMatrix(view);
	clipMatrix *= projection;

	return Extract(clipMatrix);
}

template<typename T>
const NzVector3<T>& NzFrustum<T>::GetCorner(nzCorner corner) const
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
const NzPlane<T>& NzFrustum<T>::GetPlane(nzFrustumPlane plane) const
{
	#ifdef NAZARA_DEBUG
	if (plane > nzFrustumPlane_Max)
	{
		NazaraError("Frustum plane not handled (0x" + NzString::Number(plane, 16) + ')');

		static NzVector3<T> dummy;
		return dummy;
	}
	#endif

	return m_planes[plane];
}

template<typename T>
nzIntersectionSide NzFrustum<T>::Intersect(const NzCube<T>& cube) const
{
	// http://www.lighthouse3d.com/tutorials/view-frustum-culling/geometric-approach-testing-boxes-ii/
	nzIntersectionSide side = nzIntersectionSide_Inside;

	for(unsigned int i = 0; i <= nzFrustumPlane_Max; i++)
	{
		if (m_planes[i].Distance(cube.GetPositiveVertex(m_planes[i].normal)) < F(0.0))
			return nzIntersectionSide_Outside;
		else if (m_planes[i].Distance(cube.GetNegativeVertex(m_planes[i].normal) < F(0.0)))
			side = nzIntersectionSide_Intersecting;
	}

	return side;
}

template<typename T>
nzIntersectionSide NzFrustum<T>::Intersect(const NzSphere<T>& sphere) const
{
	// http://www.lighthouse3d.com/tutorials/view-frustum-culling/geometric-approach-testing-points-and-spheres/
	nzIntersectionSide side = nzIntersectionSide_Inside;

	for(unsigned int i = 0; i <= nzFrustumPlane_Max; i++)
	{
		T distance = m_planes[i].Distance(sphere.GetPosition());
		if (distance < -sphere.radius)
			return nzIntersectionSide_Outside;
		else if (distance < sphere.radius)
			side = nzIntersectionSide_Intersecting;
	}

	return side;
}

template<typename T>
nzIntersectionSide NzFrustum<T>::Intersect(const NzVector3<T>* points, unsigned int pointCount)
{
	unsigned int c = 0;

 	for (unsigned int i = 0; i <= nzFrustumPlane_Max; ++i)
	{
		unsigned int j;
		for (j = 0; j < pointCount; j++ )
		{
			if (m_planes[i].Distance(points[j]) > F(0.0))
				break;
		}

		if (j == pointCount)
			return nzIntersectionSide_Outside;
		else
			c++;
	}

	return (c == 6) ? nzIntersectionSide_Inside : nzIntersectionSide_Intersecting;
}

template<typename T>
NzFrustum<T>& NzFrustum<T>::Set(const NzFrustum& frustum)
{
	std::memcpy(this, &frustum, sizeof(NzFrustum));

	return *this;
}

template<typename T>
template<typename U>
NzFrustum<T>& NzFrustum<T>::Set(const NzFrustum<U>& frustum)
{
	for (unsigned int i = 0; i <= nzCorner_Max; ++i)
		m_corners[i].Set(frustum.m_corners[i]);

 	for (unsigned int i = 0; i <= nzFrustumPlane_Max; ++i)
		m_planes[i].Set(frustum.m_planes[i]);

	return *this;
}

template<typename T>
NzString NzFrustum<T>::ToString() const
{
	NzStringStream ss;

	return ss << "Frustum(Bottom: " << m_planes[nzFrustumPlane_Bottom] << "\n"
	          << "        Far: " << m_planes[nzFrustumPlane_Far] << "\n"
	          << "        Left: " << m_planes[nzFrustumPlane_Left] << "\n"
	          << "        Near: " << m_planes[nzFrustumPlane_Near] << "\n"
	          << "        Right: " << m_planes[nzFrustumPlane_Right] << "\n"
	          << "        Top: " << m_planes[nzFrustumPlane_Top] << ")\n";
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const NzFrustum<T>& frustum)
{
	return out << frustum.ToString();
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
