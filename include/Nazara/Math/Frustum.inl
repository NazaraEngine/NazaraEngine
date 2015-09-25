// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Mathematics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Sources:
// http://www.crownandcutlass.com/features/technicaldetails/frustum.html
// http://www.lighthouse3d.com/tutorials/view-frustum-culling/

#include <Nazara/Core/StringStream.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <cstring>
#include <Nazara/Core/Debug.hpp>

#define F(a) static_cast<T>(a)

namespace Nz
{
	template<typename T>
	template<typename U>
	Frustum<T>::Frustum(const Frustum<U>& frustum)
	{
		Set(frustum);
	}

	template<typename T>
	Frustum<T>& Frustum<T>::Build(T angle, T ratio, T zNear, T zFar, const Vector3<T>& eye, const Vector3<T>& target, const Vector3<T>& up)
	{
		#if NAZARA_MATH_ANGLE_RADIAN
		angle /= F(2.0);
		#else
		angle = NzDegreeToRadian(angle/F(2.0));
		#endif

		T tangent = std::tan(angle);
		T nearH = zNear * tangent;
		T nearW = nearH * ratio;
		T farH = zFar * tangent;
		T farW = farH * ratio;

		Vector3<T> f = Vector3<T>::Normalize(target - eye);
		Vector3<T> u(up.GetNormal());
		Vector3<T> s = Vector3<T>::Normalize(f.CrossProduct(u));
		u = s.CrossProduct(f);

		Vector3<T> nc = eye + f * zNear;
		Vector3<T> fc = eye + f * zFar;

		// Calcul du frustum
		m_corners[BoxCorner_FarLeftBottom] = fc - u*farH - s*farW;
		m_corners[BoxCorner_FarLeftTop] = fc + u*farH - s*farW;
		m_corners[BoxCorner_FarRightTop] = fc + u*farH + s*farW;
		m_corners[BoxCorner_FarRightBottom] = fc - u*farH + s*farW;

		m_corners[BoxCorner_NearLeftBottom] = nc - u*nearH - s*nearW;
		m_corners[BoxCorner_NearLeftTop] = nc + u*nearH - s*nearW;
		m_corners[BoxCorner_NearRightTop] = nc + u*nearH + s*nearW;
		m_corners[BoxCorner_NearRightBottom] = nc - u*nearH + s*nearW;

		// Construction des plans du frustum
		m_planes[FrustumPlane_Bottom].Set(m_corners[BoxCorner_NearLeftBottom], m_corners[BoxCorner_NearRightBottom], m_corners[BoxCorner_FarRightBottom]);
		m_planes[FrustumPlane_Far].Set(m_corners[BoxCorner_FarRightTop], m_corners[BoxCorner_FarLeftTop], m_corners[BoxCorner_FarLeftBottom]);
		m_planes[FrustumPlane_Left].Set(m_corners[BoxCorner_NearLeftTop], m_corners[BoxCorner_NearLeftBottom], m_corners[BoxCorner_FarLeftBottom]);
		m_planes[FrustumPlane_Near].Set(m_corners[BoxCorner_NearLeftTop], m_corners[BoxCorner_NearRightTop], m_corners[BoxCorner_NearRightBottom]);
		m_planes[FrustumPlane_Right].Set(m_corners[BoxCorner_NearRightBottom], m_corners[BoxCorner_NearRightTop], m_corners[BoxCorner_FarRightBottom]);
		m_planes[FrustumPlane_Top].Set(m_corners[BoxCorner_NearRightTop], m_corners[BoxCorner_NearLeftTop], m_corners[BoxCorner_FarLeftTop]);

		return *this;
	}

	template<typename T>
	bool Frustum<T>::Contains(const BoundingVolume<T>& volume) const
	{
		switch (volume.extend)
		{
			case Extend_Finite:
			{
				IntersectionSide side = Intersect(volume.aabb);
				switch (side)
				{
					case IntersectionSide_Inside:
						return true;

					case IntersectionSide_Intersecting:
						return Contains(volume.obb);

					case IntersectionSide_Outside:
						return false;
				}

				NazaraError("Invalid intersection side (0x" + String::Number(side, 16) + ')');
				return false;
			}

			case Extend_Infinite:
				return true;

			case Extend_Null:
				return false;
		}

		NazaraError("Invalid extend type (0x" + String::Number(volume.extend, 16) + ')');
		return false;
	}

	template<typename T>
	bool Frustum<T>::Contains(const Box<T>& box) const
	{
		// http://www.lighthouse3d.com/tutorials/view-frustum-culling/geometric-approach-testing-boxes-ii/
		for(unsigned int i = 0; i <= FrustumPlane_Max; i++)
		{
			if (m_planes[i].Distance(box.GetPositiveVertex(m_planes[i].normal)) < F(0.0))
				return false;
		}

		return true;
	}

	template<typename T>
	bool Frustum<T>::Contains(const OrientedBox<T>& orientedbox) const
	{
		return Contains(&orientedbox[0], 8);
	}

	template<typename T>
	bool Frustum<T>::Contains(const Sphere<T>& sphere) const
	{
		for(unsigned int i = 0; i <= FrustumPlane_Max; i++)
		{
			if (m_planes[i].Distance(sphere.GetPosition()) < -sphere.radius)
				return false;
		}

		return true;
	}

	template<typename T>
	bool Frustum<T>::Contains(const Vector3<T>& point) const
	{
		for(unsigned int i = 0; i <= FrustumPlane_Max; ++i)
		{
			if (m_planes[i].Distance(point) < F(0.0))
				return false;
		}

		return true;
	}

	template<typename T>
	bool Frustum<T>::Contains(const Vector3<T>* points, unsigned int pointCount) const
	{
		for (unsigned int i = 0; i <= FrustumPlane_Max; ++i)
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
	Frustum<T>& Frustum<T>::Extract(const Matrix4<T>& clipMatrix)
	{
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
		plane[3] *= -invLength;

		m_planes[FrustumPlane_Right].Set(plane);

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
		plane[3] *= -invLength;

		m_planes[FrustumPlane_Left].Set(plane);

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
		plane[3] *= -invLength;

		m_planes[FrustumPlane_Bottom].Set(plane);

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
		plane[3] *= -invLength;

		m_planes[FrustumPlane_Top].Set(plane);

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
		plane[3] *= -invLength;

		m_planes[FrustumPlane_Far].Set(plane);

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
		plane[3] *= -invLength;

		m_planes[FrustumPlane_Near].Set(plane);

		// Une fois les plans extraits, il faut extraire les points du frustum
		// Je me base sur cette page: http://www.gamedev.net/topic/393309-calculating-the-view-frustums-vertices/

		Matrix4<T> invClipMatrix;
		if (clipMatrix.GetInverse(&invClipMatrix))
		{
			Vector4<T> corner;

			// FarLeftBottom
			corner.Set(F(-1.0), F(-1.0), F(1.0));
			corner = invClipMatrix.Transform(corner);
			corner.Normalize();

			m_corners[BoxCorner_FarLeftBottom] = Vector3<T>(corner.x, corner.y, corner.z);

			// FarLeftTop
			corner.Set(F(-1.0), F(1.0), F(1.0));
			corner = invClipMatrix.Transform(corner);
			corner.Normalize();

			m_corners[BoxCorner_FarLeftTop] = Vector3<T>(corner.x, corner.y, corner.z);

			// FarRightBottom
			corner.Set(F(1.0), F(-1.0), F(1.0));
			corner = invClipMatrix.Transform(corner);
			corner.Normalize();

			m_corners[BoxCorner_FarRightBottom] = Vector3<T>(corner.x, corner.y, corner.z);

			// FarRightTop
			corner.Set(F(1.0), F(1.0), F(1.0));
			corner = invClipMatrix.Transform(corner);
			corner.Normalize();

			m_corners[BoxCorner_FarRightTop] = Vector3<T>(corner.x, corner.y, corner.z);

			// NearLeftBottom
			corner.Set(F(-1.0), F(-1.0), F(0.0));
			corner = invClipMatrix.Transform(corner);
			corner.Normalize();

			m_corners[BoxCorner_NearLeftBottom] = Vector3<T>(corner.x, corner.y, corner.z);

			// NearLeftTop
			corner.Set(F(-1.0), F(1.0), F(0.0));
			corner = invClipMatrix.Transform(corner);
			corner.Normalize();

			m_corners[BoxCorner_NearLeftTop] = Vector3<T>(corner.x, corner.y, corner.z);

			// NearRightBottom
			corner.Set(F(1.0), F(-1.0), F(0.0));
			corner = invClipMatrix.Transform(corner);
			corner.Normalize();

			m_corners[BoxCorner_NearRightBottom] = Vector3<T>(corner.x, corner.y, corner.z);

			// NearRightTop
			corner.Set(F(1.0), F(1.0), F(0.0));
			corner = invClipMatrix.Transform(corner);
			corner.Normalize();

			m_corners[BoxCorner_NearRightTop] = Vector3<T>(corner.x, corner.y, corner.z);
		}
		else
			NazaraWarning("Clip matrix is not invertible, failed to compute frustum corners");

		return *this;
	}

	template<typename T>
	Frustum<T>& Frustum<T>::Extract(const Matrix4<T>& view, const Matrix4<T>& projection)
	{
		return Extract(Matrix4<T>::Concatenate(view, projection));
	}

	template<typename T>
	const Vector3<T>& Frustum<T>::GetCorner(BoxCorner corner) const
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
	const Plane<T>& Frustum<T>::GetPlane(FrustumPlane plane) const
	{
		#ifdef NAZARA_DEBUG
		if (plane > FrustumPlane_Max)
		{
			NazaraError("Frustum plane not handled (0x" + String::Number(plane, 16) + ')');

			static Plane<T> dummy;
			return dummy;
		}
		#endif

		return m_planes[plane];
	}

	template<typename T>
	IntersectionSide Frustum<T>::Intersect(const BoundingVolume<T>& volume) const
	{
		switch (volume.extend)
		{
			case Extend_Finite:
			{
				IntersectionSide side = Intersect(volume.aabb);
				switch (side)
				{
					case IntersectionSide_Inside:
						return IntersectionSide_Inside;

					case IntersectionSide_Intersecting:
						return Intersect(volume.obb);

					case IntersectionSide_Outside:
						return IntersectionSide_Outside;
				}

				NazaraError("Invalid intersection side (0x" + String::Number(side, 16) + ')');
				return IntersectionSide_Outside;
			}

			case Extend_Infinite:
				return IntersectionSide_Intersecting; // On ne peut pas contenir l'infini

			case Extend_Null:
				return IntersectionSide_Outside;
		}

		NazaraError("Invalid extend type (0x" + String::Number(volume.extend, 16) + ')');
		return IntersectionSide_Outside;
	}

	template<typename T>
	IntersectionSide Frustum<T>::Intersect(const Box<T>& box) const
	{
		// http://www.lighthouse3d.com/tutorials/view-frustum-culling/geometric-approach-testing-boxes-ii/
		IntersectionSide side = IntersectionSide_Inside;

		for(unsigned int i = 0; i <= FrustumPlane_Max; i++)
		{
			if (m_planes[i].Distance(box.GetPositiveVertex(m_planes[i].normal)) < F(0.0))
				return IntersectionSide_Outside;
			else if (m_planes[i].Distance(box.GetNegativeVertex(m_planes[i].normal)) < F(0.0))
				side = IntersectionSide_Intersecting;
		}

		return side;
	}

	template<typename T>
	IntersectionSide Frustum<T>::Intersect(const OrientedBox<T>& orientedbox) const
	{
		return Intersect(&orientedbox[0], 8);
	}

	template<typename T>
	IntersectionSide Frustum<T>::Intersect(const Sphere<T>& sphere) const
	{
		// http://www.lighthouse3d.com/tutorials/view-frustum-culling/geometric-approach-testing-points-and-spheres/
		IntersectionSide side = IntersectionSide_Inside;

		for(unsigned int i = 0; i <= FrustumPlane_Max; i++)
		{
			T distance = m_planes[i].Distance(sphere.GetPosition());
			if (distance < -sphere.radius)
				return IntersectionSide_Outside;
			else if (distance < sphere.radius)
				side = IntersectionSide_Intersecting;
		}

		return side;
	}

	template<typename T>
	IntersectionSide Frustum<T>::Intersect(const Vector3<T>* points, unsigned int pointCount) const
	{
		unsigned int c = 0;

		for (unsigned int i = 0; i <= FrustumPlane_Max; ++i)
		{
			unsigned int j;
			for (j = 0; j < pointCount; j++ )
			{
				if (m_planes[i].Distance(points[j]) > F(0.0))
					break;
			}

			if (j == pointCount)
				return IntersectionSide_Outside;
			else
				c++;
		}

		return (c == 6) ? IntersectionSide_Inside : IntersectionSide_Intersecting;
	}

	template<typename T>
	Frustum<T>& Frustum<T>::Set(const Frustum& frustum)
	{
		std::memcpy(this, &frustum, sizeof(Frustum));

		return *this;
	}

	template<typename T>
	template<typename U>
	Frustum<T>& Frustum<T>::Set(const Frustum<U>& frustum)
	{
		for (unsigned int i = 0; i <= BoxCorner_Max; ++i)
			m_corners[i].Set(frustum.m_corners[i]);

		for (unsigned int i = 0; i <= FrustumPlane_Max; ++i)
			m_planes[i].Set(frustum.m_planes[i]);

		return *this;
	}

	template<typename T>
	String Frustum<T>::ToString() const
	{
		StringStream ss;

		return ss << "Frustum(Bottom: " << m_planes[FrustumPlane_Bottom].ToString() << "\n"
				  << "        Far: " << m_planes[FrustumPlane_Far].ToString() << "\n"
				  << "        Left: " << m_planes[FrustumPlane_Left].ToString() << "\n"
				  << "        Near: " << m_planes[FrustumPlane_Near].ToString() << "\n"
				  << "        Right: " << m_planes[FrustumPlane_Right].ToString() << "\n"
				  << "        Top: " << m_planes[FrustumPlane_Top].ToString() << ")\n";
	}
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Nz::Frustum<T>& frustum)
{
	return out << frustum.ToString();
}

#undef F

#include <Nazara/Core/DebugOff.hpp>
