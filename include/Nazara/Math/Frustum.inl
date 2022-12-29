// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Config.hpp

// Sources:
// http://www.crownandcutlass.com/features/technicaldetails/frustum.html
// http://www.lighthouse3d.com/tutorials/view-frustum-culling/

#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <cstring>
#include <sstream>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{

	/*!
	* \ingroup math
	* \class Nz::Frustum
	* \brief Math class that represents a frustum in the three dimensional vector space
	*
	* Frustums are used to determine what is inside the camera's field of view. They help speed up the rendering process
	*/

	/*!
	* \brief Constructs a Frustum by specifying its planes
	*
	* \param corners Corners 
	* \param planes Frustum of type U to convert to type T
	*/
	template<typename T>
	Frustum<T>::Frustum(const std::array<Plane<T>, FrustumPlaneCount>& planes) :
	m_planes(planes)
	{
	}

	/*!
	* \brief Constructs a Frustum object from another type of Frustum
	*
	* \param frustum Frustum of type U to convert to type T
	*/
	template<typename T>
	template<typename U>
	Frustum<T>::Frustum(const Frustum<U>& frustum)
	{
		for (std::size_t i = 0; i < FrustumPlaneCount; ++i)
			m_planes[i].Set(frustum.m_planes[i]);
	}

	/*!
	* \brief Computes the position of a frustum corner
	* \return The corner position
	*
	* \param corner Which corner to compute
	*/
	template<typename T>
	Vector3<T> Frustum<T>::ComputeCorner(BoxCorner corner) const
	{
		switch (corner)
		{
			case BoxCorner::FarLeftBottom:   return Plane<T>::Intersect(GetPlane(FrustumPlane::Far),  GetPlane(FrustumPlane::Left),  GetPlane(FrustumPlane::Bottom));
			case BoxCorner::FarLeftTop:      return Plane<T>::Intersect(GetPlane(FrustumPlane::Far),  GetPlane(FrustumPlane::Left),  GetPlane(FrustumPlane::Top));
			case BoxCorner::FarRightBottom:  return Plane<T>::Intersect(GetPlane(FrustumPlane::Far),  GetPlane(FrustumPlane::Right), GetPlane(FrustumPlane::Bottom));
			case BoxCorner::FarRightTop:     return Plane<T>::Intersect(GetPlane(FrustumPlane::Far),  GetPlane(FrustumPlane::Right), GetPlane(FrustumPlane::Top));
			case BoxCorner::NearLeftBottom:  return Plane<T>::Intersect(GetPlane(FrustumPlane::Near), GetPlane(FrustumPlane::Left),  GetPlane(FrustumPlane::Bottom));
			case BoxCorner::NearLeftTop:     return Plane<T>::Intersect(GetPlane(FrustumPlane::Near), GetPlane(FrustumPlane::Left),  GetPlane(FrustumPlane::Top));
			case BoxCorner::NearRightBottom: return Plane<T>::Intersect(GetPlane(FrustumPlane::Near), GetPlane(FrustumPlane::Right), GetPlane(FrustumPlane::Bottom));
			case BoxCorner::NearRightTop:    return Plane<T>::Intersect(GetPlane(FrustumPlane::Near), GetPlane(FrustumPlane::Right), GetPlane(FrustumPlane::Top));
		}

		NazaraError("invalid frustum corner");
		return Vector3<T>();
	}

	/*!
	* \brief Checks whether or not a bounding volume is contained in the frustum
	* \return true if the bounding volume is entirely in the frustum
	*
	* \param volume Volume to check
	*
	* \remark If volume is infinite, true is returned
	* \remark If volume is null, false is returned
	* \remark If enumeration of the volume is not defined in Extend, a NazaraError is thrown and false is returned
	* \remark If enumeration of the intersection is not defined in IntersectionSide, a NazaraError is thrown and false is returned. This should not never happen for a user of the library
	*/
	template<typename T>
	bool Frustum<T>::Contains(const BoundingVolume<T>& volume) const
	{
		switch (volume.extend)
		{
			case Extend::Finite:
			{
				IntersectionSide side = Intersect(volume.aabb);
				switch (side)
				{
					case IntersectionSide::Inside:
						return true;

					case IntersectionSide::Intersecting:
						return Contains(volume.obb);

					case IntersectionSide::Outside:
						return false;
				}

				NazaraError("Invalid intersection side (0x" + NumberToString(UnderlyingCast(side), 16) + ')');
				return false;
			}

			case Extend::Infinite:
				return true;

			case Extend::Null:
				return false;
		}

		NazaraError("Invalid extend type (0x" + NumberToString(UnderlyingCast(volume.extend), 16) + ')');
		return false;
	}

	/*!
	* \brief Checks whether or not a box is contained in the frustum
	* \return true if the box is entirely in the frustum
	*
	* \param box Box to check
	*/
	template<typename T>
	bool Frustum<T>::Contains(const Box<T>& box) const
	{
		// http://www.lighthouse3d.com/tutorials/view-frustum-culling/geometric-approach-testing-boxes-ii/
		for (unsigned int i = 0; i < FrustumPlaneCount; i++)
		{
			if (m_planes[i].Distance(box.GetPositiveVertex(m_planes[i].normal)) < T(0.0))
				return false;
		}

		return true;
	}

	/*!
	* \brief Checks whether or not an oriented box is contained in the frustum
	* \return true if the oriented box is entirely in the frustum
	*
	* \param orientedbox Oriented box to check
	*/

	template<typename T>
	bool Frustum<T>::Contains(const OrientedBox<T>& orientedbox) const
	{
		return Contains(orientedbox.GetCorners(), 8);
	}

	/*!
	* \brief Checks whether or not a sphere is contained in the frustum
	* \return true if the sphere is entirely in the frustum
	*
	* \param sphere Sphere to check
	*/

	template<typename T>
	bool Frustum<T>::Contains(const Sphere<T>& sphere) const
	{
		for (unsigned int i = 0; i < FrustumPlaneCount; i++)
		{
			if (m_planes[i].Distance(sphere.GetPosition()) < -sphere.radius)
				return false;
		}

		return true;
	}

	/*!
	* \brief Checks whether or not a Vector3 is contained in the frustum
	* \return true if the Vector3 is in the frustum
	*
	* \param point Vector3 which represents a point in the space
	*/

	template<typename T>
	bool Frustum<T>::Contains(const Vector3<T>& point) const
	{
		for (unsigned int i = 0; i < FrustumPlaneCount; ++i)
		{
			if (m_planes[i].Distance(point) < T(0.0))
				return false;
		}

		return true;
	}

	/*!
	* \brief Checks whether or not a set of Vector3 is contained in the frustum
	* \return true if the set of Vector3 is in the frustum
	*
	* \param points Pointer to Vector3 which represents a set of points in the space
	* \param pointCount Number of points to check
	*/

	template<typename T>
	bool Frustum<T>::Contains(const Vector3<T>* points, std::size_t pointCount) const
	{
		for (std::size_t i = 0; i < FrustumPlaneCount; ++i)
		{
			std::size_t j;
			for (j = 0; j < pointCount; j++ )
			{
				if (m_planes[i].Distance(points[j]) > T(0.0))
					break;
			}

			if (j == pointCount)
				return false;
		}

		return true;
	}

	/*!
	* \brief Gets the Plane for the face
	* \return The face of the frustum according to enum FrustumPlane
	*
	* \param plane Enumeration of type FrustumPlane
	*
	* \remark If enumeration is not defined in FrustumPlane and NAZARA_DEBUG defined, a NazaraError is thrown and a Plane uninitialised is returned
	*/

	template<typename T>
	const Plane<T>& Frustum<T>::GetPlane(FrustumPlane plane) const
	{
		NazaraAssert(UnderlyingCast(plane) < FrustumPlaneCount, "invalid plane");
		return m_planes[UnderlyingCast(plane)];
	}

	/*!
	* \brief Checks whether or not a bounding volume intersects with the frustum
	* \return IntersectionSide How the bounding volume is intersecting with the frustum
	*
	* \param volume Volume to check
	*
	* \remark If volume is infinite, IntersectionSide::Intersecting is returned
	* \remark If volume is null, IntersectionSide::Outside is returned
	* \remark If enumeration of the volume is not defined in Extend, a NazaraError is thrown and IntersectionSide::Outside is returned
	* \remark If enumeration of the intersection is not defined in IntersectionSide, a NazaraError is thrown and IntersectionSide::Outside is returned. This should not never happen for a user of the library
	*/

	template<typename T>
	IntersectionSide Frustum<T>::Intersect(const BoundingVolume<T>& volume) const
	{
		switch (volume.extend)
		{
			case Extend::Finite:
			{
				IntersectionSide side = Intersect(volume.aabb);
				switch (side)
				{
					case IntersectionSide::Inside:
						return IntersectionSide::Inside;

					case IntersectionSide::Intersecting:
						return Intersect(volume.obb);

					case IntersectionSide::Outside:
						return IntersectionSide::Outside;
				}

				NazaraError("Invalid intersection side (0x" + NumberToString(UnderlyingCast(side), 16) + ')');
				return IntersectionSide::Outside;
			}

			case Extend::Infinite:
				return IntersectionSide::Intersecting; // We can not contain infinity

			case Extend::Null:
				return IntersectionSide::Outside;
		}

		NazaraError("Invalid extend type (0x" + NumberToString(UnderlyingCast(volume.extend), 16) + ')');
		return IntersectionSide::Outside;
	}

	/*!
	* \brief Checks whether or not a box intersects with the frustum
	* \return IntersectionSide How the box is intersecting with the frustum
	*
	* \param box Box to check
	*/

	template<typename T>
	IntersectionSide Frustum<T>::Intersect(const Box<T>& box) const
	{
		// http://www.lighthouse3d.com/tutorials/view-frustum-culling/geometric-approach-testing-boxes-ii/
		IntersectionSide side = IntersectionSide::Inside;

		for (std::size_t i = 0; i < FrustumPlaneCount; i++)
		{
			if (m_planes[i].Distance(box.GetPositiveVertex(m_planes[i].normal)) < T(0.0))
				return IntersectionSide::Outside;
			else if (m_planes[i].Distance(box.GetNegativeVertex(m_planes[i].normal)) < T(0.0))
				side = IntersectionSide::Intersecting;
		}

		return side;
	}

	/*!
	* \brief Checks whether or not an oriented box intersects with the frustum
	* \return IntersectionSide How the oriented box is intersecting with the frustum
	*
	* \param oriented box OrientedBox to check
	*/

	template<typename T>
	IntersectionSide Frustum<T>::Intersect(const OrientedBox<T>& orientedbox) const
	{
		return Intersect(orientedbox.GetCorners(), 8);
	}

	/*!
	* \brief Checks whether or not a sphere intersects with the frustum
	* \return IntersectionSide How the sphere is intersecting with the frustum
	*
	* \param sphere Sphere to check
	*/

	template<typename T>
	IntersectionSide Frustum<T>::Intersect(const Sphere<T>& sphere) const
	{
		// http://www.lighthouse3d.com/tutorials/view-frustum-culling/geometric-approach-testing-points-and-spheres/
		IntersectionSide side = IntersectionSide::Inside;

		for (std::size_t i = 0; i < FrustumPlaneCount; i++)
		{
			T distance = m_planes[i].Distance(sphere.GetPosition());
			if (distance < -sphere.radius)
				return IntersectionSide::Outside;
			else if (distance < sphere.radius)
				side = IntersectionSide::Intersecting;
		}

		return side;
	}

	/*!
	* \brief Checks whether or not a set of Vector3 intersects with the frustum
	* \return IntersectionSide How the set of Vector3 is intersecting with the frustum
	*
	* \param points Pointer to Vector3 which represents a set of points in the space
	* \param pointCount Number of points to check
	*/

	template<typename T>
	IntersectionSide Frustum<T>::Intersect(const Vector3<T>* points, std::size_t pointCount) const
	{
		std::size_t c = 0;

		for (std::size_t i = 0; i < FrustumPlaneCount; ++i)
		{
			std::size_t j;
			for (j = 0; j < pointCount; j++ )
			{
				if (m_planes[i].Distance(points[j]) > T(0.0))
					break;
			}

			if (j == pointCount)
				return IntersectionSide::Outside;
			else
				c++;
		}

		return (c == 6) ? IntersectionSide::Inside : IntersectionSide::Intersecting;
	}

	/*!
	* \brief Gives a string representation
	* \return A string representation of the object: "Frustum(Plane ...)"
	*/
	template<typename T>
	std::string Frustum<T>::ToString() const
	{
		std::ostringstream ss;
		ss << *this;

		return ss.str();
	}


	/*!
	* \brief Builds the frustum object
	* \return A reference to this frustum which is the build up camera's field of view
	*
	* \param angle FOV angle
	* \param ratio Rendering ratio (typically 16/9 or 4/3)
	* \param zNear Distance where 'vision' begins
	* \param zFar Distance where 'vision' ends
	* \param eye Position of the camera
	* \param target Position of the target of the camera
	* \param up Direction of up vector according to the orientation of camera
	*/
	template<typename T>
	Frustum<T> Frustum<T>::Build(RadianAngle<T> angle, T ratio, T zNear, T zFar, const Vector3<T>& eye, const Vector3<T>& target, const Vector3<T>& up)
	{
		angle /= T(2.0);

		T tangent = angle.GetTan();
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

		// Computing the frustum
		std::array<Vector3<T>, BoxCornerCount> corners;
		corners[UnderlyingCast(BoxCorner::FarLeftBottom)]  = fc - u * farH - s * farW;
		corners[UnderlyingCast(BoxCorner::FarLeftTop)]     = fc + u * farH - s * farW;
		corners[UnderlyingCast(BoxCorner::FarRightTop)]    = fc + u * farH + s * farW;
		corners[UnderlyingCast(BoxCorner::FarRightBottom)] = fc - u * farH + s * farW;

		corners[UnderlyingCast(BoxCorner::NearLeftBottom)]  = nc - u * nearH - s * nearW;
		corners[UnderlyingCast(BoxCorner::NearLeftTop)]     = nc + u * nearH - s * nearW;
		corners[UnderlyingCast(BoxCorner::NearRightTop)]    = nc + u * nearH + s * nearW;
		corners[UnderlyingCast(BoxCorner::NearRightBottom)] = nc - u * nearH + s * nearW;

		// Construction of frustum's planes
		std::array<Plane<T>, FrustumPlaneCount> planes;
		planes[UnderlyingCast(FrustumPlane::Bottom)] = Plane(corners[UnderlyingCast(BoxCorner::NearLeftBottom)],  corners[UnderlyingCast(BoxCorner::NearRightBottom)], corners[UnderlyingCast(BoxCorner::FarRightBottom)]);
		planes[UnderlyingCast(FrustumPlane::Far)]    = Plane(corners[UnderlyingCast(BoxCorner::FarRightTop)],     corners[UnderlyingCast(BoxCorner::FarLeftTop)],      corners[UnderlyingCast(BoxCorner::FarLeftBottom)]);
		planes[UnderlyingCast(FrustumPlane::Left)]   = Plane(corners[UnderlyingCast(BoxCorner::NearLeftTop)],     corners[UnderlyingCast(BoxCorner::NearLeftBottom)],  corners[UnderlyingCast(BoxCorner::FarLeftBottom)]);
		planes[UnderlyingCast(FrustumPlane::Near)]   = Plane(corners[UnderlyingCast(BoxCorner::NearLeftTop)],     corners[UnderlyingCast(BoxCorner::NearRightTop)],    corners[UnderlyingCast(BoxCorner::NearRightBottom)]);
		planes[UnderlyingCast(FrustumPlane::Right)]  = Plane(corners[UnderlyingCast(BoxCorner::NearRightBottom)], corners[UnderlyingCast(BoxCorner::NearRightTop)],    corners[UnderlyingCast(BoxCorner::FarRightBottom)]);
		planes[UnderlyingCast(FrustumPlane::Top)]    = Plane(corners[UnderlyingCast(BoxCorner::NearRightTop)],    corners[UnderlyingCast(BoxCorner::NearLeftTop)],     corners[UnderlyingCast(BoxCorner::FarLeftTop)]);

		return Frustum(planes);
	}

	/*!
	* \brief Constructs the frustum from a Matrix4
	* \return A reference to this frustum which is the build up of projective matrix
	*
	* \param viewProjMatrix Matrix which represents the transformation of the frustum
	*/
	template<typename T>
	Frustum<T> Frustum<T>::Extract(const Matrix4<T>& viewProjMatrix)
	{
		// http://www.crownandcutlass.com/features/technicaldetails/frustum.html
		T plane[4];
		T invLength;

		std::array<Plane<T>, FrustumPlaneCount> planes;

		// Extract the numbers for the RIGHT plane
		plane[0] = viewProjMatrix[3] - viewProjMatrix[0];
		plane[1] = viewProjMatrix[7] - viewProjMatrix[4];
		plane[2] = viewProjMatrix[11] - viewProjMatrix[8];
		plane[3] = viewProjMatrix[15] - viewProjMatrix[12];

		// Normalize the result
		invLength = T(1.0) / std::sqrt(plane[0] * plane[0] + plane[1] * plane[1] + plane[2] * plane[2]);
		plane[0] *= invLength;
		plane[1] *= invLength;
		plane[2] *= invLength;
		plane[3] *= -invLength;

		planes[UnderlyingCast(FrustumPlane::Right)] = Plane<T>(plane);

		// Extract the numbers for the LEFT plane
		plane[0] = viewProjMatrix[3] + viewProjMatrix[0];
		plane[1] = viewProjMatrix[7] + viewProjMatrix[4];
		plane[2] = viewProjMatrix[11] + viewProjMatrix[8];
		plane[3] = viewProjMatrix[15] + viewProjMatrix[12];

		// Normalize the result
		invLength = T(1.0) / std::sqrt(plane[0] * plane[0] + plane[1] * plane[1] + plane[2] * plane[2]);
		plane[0] *= invLength;
		plane[1] *= invLength;
		plane[2] *= invLength;
		plane[3] *= -invLength;

		planes[UnderlyingCast(FrustumPlane::Left)] = Plane<T>(plane);

		// Extract the BOTTOM plane
		plane[0] = viewProjMatrix[3] + viewProjMatrix[1];
		plane[1] = viewProjMatrix[7] + viewProjMatrix[5];
		plane[2] = viewProjMatrix[11] + viewProjMatrix[9];
		plane[3] = viewProjMatrix[15] + viewProjMatrix[13];

		// Normalize the result
		invLength = T(1.0) / std::sqrt(plane[0] * plane[0] + plane[1] * plane[1] + plane[2] * plane[2]);
		plane[0] *= invLength;
		plane[1] *= invLength;
		plane[2] *= invLength;
		plane[3] *= -invLength;

		planes[UnderlyingCast(FrustumPlane::Bottom)] = Plane<T>(plane);

		// Extract the TOP plane
		plane[0] = viewProjMatrix[3] - viewProjMatrix[1];
		plane[1] = viewProjMatrix[7] - viewProjMatrix[5];
		plane[2] = viewProjMatrix[11] - viewProjMatrix[9];
		plane[3] = viewProjMatrix[15] - viewProjMatrix[13];

		// Normalize the result
		invLength = T(1.0) / std::sqrt(plane[0] * plane[0] + plane[1] * plane[1] + plane[2] * plane[2]);
		plane[0] *= invLength;
		plane[1] *= invLength;
		plane[2] *= invLength;
		plane[3] *= -invLength;

		planes[UnderlyingCast(FrustumPlane::Top)] = Plane<T>(plane);

		// Extract the FAR plane
		plane[0] = viewProjMatrix[3] - viewProjMatrix[2];
		plane[1] = viewProjMatrix[7] - viewProjMatrix[6];
		plane[2] = viewProjMatrix[11] - viewProjMatrix[10];
		plane[3] = viewProjMatrix[15] - viewProjMatrix[14];

		// Normalize the result
		invLength = T(1.0) / std::sqrt(plane[0] * plane[0] + plane[1] * plane[1] + plane[2] * plane[2]);
		plane[0] *= invLength;
		plane[1] *= invLength;
		plane[2] *= invLength;
		plane[3] *= -invLength;

		planes[UnderlyingCast(FrustumPlane::Far)] = Plane<T>(plane);

		// Extract the NEAR plane
		plane[0] = viewProjMatrix[3] + viewProjMatrix[2];
		plane[1] = viewProjMatrix[7] + viewProjMatrix[6];
		plane[2] = viewProjMatrix[11] + viewProjMatrix[10];
		plane[3] = viewProjMatrix[15] + viewProjMatrix[14];

		// Normalize the result
		invLength = T(1.0) / std::sqrt(plane[0] * plane[0] + plane[1] * plane[1] + plane[2] * plane[2]);
		plane[0] *= invLength;
		plane[1] *= invLength;
		plane[2] *= invLength;
		plane[3] *= -invLength;

		planes[UnderlyingCast(FrustumPlane::Near)] = Plane<T>(plane);

		return Frustum(planes);
	}

	/*!
	* \brief Serializes a Frustum
	* \return true if successfully serialized
	*
	* \param context Serialization context
	* \param matrix Input frustum
	*/
	template<typename T>
	bool Serialize(SerializationContext& context, const Frustum<T>& frustum, TypeTag<Frustum<T>>)
	{
		for (unsigned int i = 0; i < FrustumPlaneCount; ++i)
		{
			if (!Serialize(context, frustum.m_planes[i]))
				return false;
		}

		return true;
	}

	/*!
	* \brief Unserializes a Frustum
	* \return true if successfully unserialized
	*
	* \param context Serialization context
	* \param matrix Output frustum
	*/
	template<typename T>
	bool Unserialize(SerializationContext& context, Frustum<T>* frustum, TypeTag<Frustum<T>>)
	{
		for (unsigned int i = 0; i < FrustumPlaneCount; ++i)
		{
			if (!Unserialize(context, &frustum->m_planes[i]))
				return false;
		}

		return true;
	}

	/*!
	* \brief Output operator
	* \return The stream
	*
	* \param out The stream
	* \param frustum The frustum to output
	*/
	template<typename T>
	std::ostream& operator<<(std::ostream& out, const Nz::Frustum<T>& frustum)
	{
		return out << "Frustum(Bottom: " << frustum.GetPlane(Nz::FrustumPlane::Bottom) << ",\n"
			<< "        Far: " << frustum.GetPlane(Nz::FrustumPlane::Far) << ",\n"
			<< "        Left: " << frustum.GetPlane(Nz::FrustumPlane::Left) << ",\n"
			<< "        Near: " << frustum.GetPlane(Nz::FrustumPlane::Near) << ",\n"
			<< "        Right: " << frustum.GetPlane(Nz::FrustumPlane::Right) << ",\n"
			<< "        Top: " << frustum.GetPlane(Nz::FrustumPlane::Top) << ")\n";
	}
}

#include <Nazara/Core/DebugOff.hpp>
