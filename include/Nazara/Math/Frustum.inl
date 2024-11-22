// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Math module"
// For conditions of distribution and use, see copyright notice in Export.hpp

// Sources:
// http://www.crownandcutlass.com/features/technicaldetails/frustum.html
// http://www.lighthouse3d.com/tutorials/view-frustum-culling/

#include <NazaraUtils/EnumArray.hpp>
#include <NazaraUtils/MathUtils.hpp>
#include <cstring>
#include <sstream>

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
	constexpr Frustum<T>::Frustum(const EnumArray<FrustumPlane, Plane<T>>& planes) :
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
	constexpr Frustum<T>::Frustum(const Frustum<U>& frustum)
	{
		for (auto&& [planeEnum, plane] : m_planes.iter_kv())
			plane = Frustum(frustum.GetPlane(planeEnum));
	}

	template<typename T>
	constexpr bool Frustum<T>::ApproxEqual(const Frustum& frustum, T maxDifference) const
	{
		for (auto&& [planeEnum, plane] : m_planes.iter_kv())
		{
			if (!plane.ApproxEqual(frustum.GetPlane(planeEnum), maxDifference))
				return false;
		}

		return true;
	}

	/*!
	* \brief Computes the position of a frustum corner
	* \return The corner position
	*
	* \param corner Which corner to compute
	*/
	template<typename T>
	constexpr Vector3<T> Frustum<T>::ComputeCorner(BoxCorner corner) const
	{
		switch (corner)
		{
			case BoxCorner::LeftBottomFar:   return Plane<T>::Intersect(GetPlane(FrustumPlane::Far),  GetPlane(FrustumPlane::Left),  GetPlane(FrustumPlane::Bottom));
			case BoxCorner::LeftTopFar:      return Plane<T>::Intersect(GetPlane(FrustumPlane::Far),  GetPlane(FrustumPlane::Left),  GetPlane(FrustumPlane::Top));
			case BoxCorner::RightBottomFar:  return Plane<T>::Intersect(GetPlane(FrustumPlane::Far),  GetPlane(FrustumPlane::Right), GetPlane(FrustumPlane::Bottom));
			case BoxCorner::RightTopFar:     return Plane<T>::Intersect(GetPlane(FrustumPlane::Far),  GetPlane(FrustumPlane::Right), GetPlane(FrustumPlane::Top));
			case BoxCorner::LeftBottomNear:  return Plane<T>::Intersect(GetPlane(FrustumPlane::Near), GetPlane(FrustumPlane::Left),  GetPlane(FrustumPlane::Bottom));
			case BoxCorner::LeftTopNear:     return Plane<T>::Intersect(GetPlane(FrustumPlane::Near), GetPlane(FrustumPlane::Left),  GetPlane(FrustumPlane::Top));
			case BoxCorner::RightBottomNear: return Plane<T>::Intersect(GetPlane(FrustumPlane::Near), GetPlane(FrustumPlane::Right), GetPlane(FrustumPlane::Bottom));
			case BoxCorner::RightTopNear:    return Plane<T>::Intersect(GetPlane(FrustumPlane::Near), GetPlane(FrustumPlane::Right), GetPlane(FrustumPlane::Top));
		}

		NazaraError("invalid frustum corner");
		return Vector3<T>();
	}

	template<typename T>
	constexpr EnumArray<BoxCorner, Vector3<T>> Frustum<T>::ComputeCorners() const
	{
		return {
			ComputeCorner(BoxCorner::LeftBottomFar),
			ComputeCorner(BoxCorner::LeftTopFar),
			ComputeCorner(BoxCorner::RightBottomFar),
			ComputeCorner(BoxCorner::RightTopFar),
			ComputeCorner(BoxCorner::LeftBottomNear),
			ComputeCorner(BoxCorner::LeftTopNear),
			ComputeCorner(BoxCorner::RightBottomNear),
			ComputeCorner(BoxCorner::RightTopNear)
		};
	}

	/*!
	* \brief Checks whether or not a bounding volume is contained in the frustum
	* \return true if the bounding volume is entirely in the frustum
	*
	* \param volume Volume to check
	*
	* \remark If volume is infinite, true is returned
	* \remark If volume is null, false is returned
	* \remark If enumeration of the volume is not defined in Extent, a NazaraError is thrown and false is returned
	* \remark If enumeration of the intersection is not defined in IntersectionSide, a NazaraError is thrown and false is returned. This should not never happen for a user of the library
	*/
	template<typename T>
	constexpr bool Frustum<T>::Contains(const BoundingVolume<T>& volume) const
	{
		switch (volume.extent)
		{
			case Extent::Finite:
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

				NazaraError("invalid intersection side ({0:#x})", UnderlyingCast(side));
				return false;
			}

			case Extent::Infinite:
				return true;

			case Extent::Null:
				return false;
		}

		NazaraError("invalid extent type ({0:#x})", UnderlyingCast(volume.extent));
		return false;
	}

	/*!
	* \brief Checks whether or not a box is contained in the frustum
	* \return true if the box is entirely in the frustum
	*
	* \param box Box to check
	*/
	template<typename T>
	constexpr bool Frustum<T>::Contains(const Box<T>& box) const
	{
		// https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
		// https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling

		Vector3<T> center = box.GetCenter();
		Vector3<T> extents = box.GetLengths() * T(0.5);

		for (const auto& plane : m_planes)
		{
			Vector3<T> projectedExtents = extents * plane.normal.GetAbs();
			float radius = projectedExtents.x + projectedExtents.y + projectedExtents.z;

			float distance = plane.SignedDistance(center);
			if (distance < T(radius))
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
	constexpr bool Frustum<T>::Contains(const OrientedBox<T>& orientedbox) const
	{
		return Contains(orientedbox.GetCorners().data(), 8);
	}

	/*!
	* \brief Checks whether or not a sphere is contained in the frustum
	* \return true if the sphere is entirely in the frustum
	*
	* \param sphere Sphere to check
	*/
	template<typename T>
	constexpr bool Frustum<T>::Contains(const Sphere<T>& sphere) const
	{
		for (const auto& plane : m_planes)
		{
			if (plane.SignedDistance(sphere.GetPosition()) < -sphere.radius)
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
	constexpr bool Frustum<T>::Contains(const Vector3<T>& point) const
	{
		for (const auto& plane : m_planes)
		{
			if (plane.SignedDistance(point) < T(0.0))
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
	constexpr bool Frustum<T>::Contains(const Vector3<T>* points, std::size_t pointCount) const
	{
		for (const auto& plane : m_planes)
		{
			for (std::size_t i = 0; i < pointCount; ++i)
			{
				if (plane.SignedDistance(points[i]) < T(0.0))
					return false;
			}
		}

		return true;
	}

	template<typename T>
	constexpr Box<T> Frustum<T>::GetAABB() const
	{
		EnumArray<BoxCorner, Vector3<T>> corners = ComputeCorners();

		Vector3f max = corners.front();
		Vector3f min = corners.front();
		for (std::size_t i = 1; i < corners.size(); ++i)
		{
			max.Maximize(corners[static_cast<BoxCorner>(i)]);
			min.Minimize(corners[static_cast<BoxCorner>(i)]);
		}

		return Box<T>::FromExtents(min, max);
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
	constexpr const Plane<T>& Frustum<T>::GetPlane(FrustumPlane plane) const
	{
		NazaraAssert(plane <= FrustumPlane::Max, "invalid plane");
		return m_planes[plane];
	}

	template<typename T>
	constexpr const EnumArray<FrustumPlane, Plane<T>>& Frustum<T>::GetPlanes() const
	{
		return m_planes;
	}

	/*!
	* \brief Checks whether or not a bounding volume intersects with the frustum
	* \return IntersectionSide How the bounding volume is intersecting with the frustum
	*
	* \param volume Volume to check
	*
	* \remark If volume is infinite, IntersectionSide::Intersecting is returned
	* \remark If volume is null, IntersectionSide::Outside is returned
	* \remark If enumeration of the volume is not defined in Extent, a NazaraError is thrown and IntersectionSide::Outside is returned
	* \remark If enumeration of the intersection is not defined in IntersectionSide, a NazaraError is thrown and IntersectionSide::Outside is returned. This should not never happen for a user of the library
	*/
	template<typename T>
	constexpr IntersectionSide Frustum<T>::Intersect(const BoundingVolume<T>& volume) const
	{
		switch (volume.extent)
		{
			case Extent::Finite:
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

				NazaraError("invalid intersection side ({0:#x})", UnderlyingCast(side));
				return IntersectionSide::Outside;
			}

			case Extent::Infinite:
				return IntersectionSide::Intersecting; // We can not contain infinity

			case Extent::Null:
				return IntersectionSide::Outside;
		}

		NazaraError("invalid extent type ({0:#x})", UnderlyingCast(volume.extent));
		return IntersectionSide::Outside;
	}

	/*!
	* \brief Checks whether or not a box intersects with the frustum
	* \return IntersectionSide How the box is intersecting with the frustum
	*
	* \param box Box to check
	*/
	template<typename T>
	constexpr IntersectionSide Frustum<T>::Intersect(const Box<T>& box) const
	{
		// https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
		// https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling
		IntersectionSide side = IntersectionSide::Inside;

		Vector3<T> center = box.GetCenter();
		Vector3<T> extents = box.GetLengths() * T(0.5);

		for (const auto& plane : m_planes)
		{
			Vector3<T> projectedExtents = extents * plane.normal.GetAbs();
			float radius = projectedExtents.x + projectedExtents.y + projectedExtents.z;

			float distance = plane.SignedDistance(center);

			if (distance < T(-radius))
				return IntersectionSide::Outside;
			else if (distance < T(radius))
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
	constexpr IntersectionSide Frustum<T>::Intersect(const OrientedBox<T>& orientedbox) const
	{
		return Intersect(orientedbox.GetCorners().data(), 8);
	}

	/*!
	* \brief Checks whether or not a sphere intersects with the frustum
	* \return IntersectionSide How the sphere is intersecting with the frustum
	*
	* \param sphere Sphere to check
	*/
	template<typename T>
	constexpr IntersectionSide Frustum<T>::Intersect(const Sphere<T>& sphere) const
	{
		// http://www.lighthouse3d.com/tutorials/view-frustum-culling/geometric-approach-testing-points-and-spheres/
		IntersectionSide side = IntersectionSide::Inside;

		for (const auto& plane : m_planes)
		{
			T distance = plane.SignedDistance(sphere.GetPosition());
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
	constexpr IntersectionSide Frustum<T>::Intersect(const Vector3<T>* points, std::size_t pointCount) const
	{
		IntersectionSide side = IntersectionSide::Inside;

		for (const auto& plane : m_planes)
		{
			bool outside = true;
			for (std::size_t i = 0; i < pointCount; ++i)
			{
				// If at least one point is outside of the frustum, we're intersecting
				if (plane.SignedDistance(points[i]) < T(0.0))
					side = IntersectionSide::Intersecting;
				else
					outside = false;
			}

			// But if no point is intersecting on this plane, then it's outside
			if (outside)
				return IntersectionSide::Outside;
		}

		return side;
	}

	template<typename T>
	constexpr Frustum<T> Frustum<T>::Reduce(T nearFactor, T farFactor) const
	{
		EnumArray<FrustumPlane, Plane<T>> planes = m_planes;
		planes[FrustumPlane::Near].distance = Lerp(m_planes[FrustumPlane::Near].distance, -m_planes[FrustumPlane::Far].distance, nearFactor);
		planes[FrustumPlane::Far].distance = Lerp(-m_planes[FrustumPlane::Near].distance, m_planes[FrustumPlane::Far].distance, farFactor);

		return Frustum<T>(planes);
	}

	template<typename T>
	template<typename F>
	constexpr void Frustum<T>::Split(std::initializer_list<T> splitFactors, F&& callback) const
	{
		return Split(splitFactors.begin(), splitFactors.size(), std::forward<F>(callback));
	}

	template<typename T>
	template<typename F>
	constexpr void Frustum<T>::Split(const T* splitFactors, std::size_t factorCount, F&& callback) const
	{
		T previousFar = T(0.0);
		for (std::size_t i = 0; i < factorCount; ++i)
		{
			T farFactor = splitFactors[i];
			callback(previousFar, farFactor);
			previousFar = farFactor;
		}

		callback(previousFar, T(1.0));
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
	constexpr bool Frustum<T>::operator==(const Frustum& frustum) const
	{
		for (auto&& [planeEnum, plane] : m_planes.iter_kv())
		{
			if (!plane != frustum.GetPlane(planeEnum))
				return false;
		}

		return true;
	}

	template<typename T>
	constexpr bool Frustum<T>::operator!=(const Frustum& frustum) const
	{
		return !operator==(frustum);
	}

	template<typename T>
	constexpr bool Frustum<T>::ApproxEqual(const Frustum& lhs, const Frustum& rhs, T maxDifference)
	{
		return lhs.ApproxEqual(rhs, maxDifference);
	}

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
		Vector3<T> u = Vector3<T>::Normalize(up);
		Vector3<T> s = Vector3<T>::Normalize(f.CrossProduct(u));
		u = s.CrossProduct(f);

		Vector3<T> nc = eye + f * zNear;
		Vector3<T> fc = eye + f * zFar;

		// Computing the frustum
		EnumArray<BoxCorner, Vector3<T>> corners;
		corners[BoxCorner::LeftBottomFar]  = fc - u * farH - s * farW;
		corners[BoxCorner::LeftTopFar]     = fc + u * farH - s * farW;
		corners[BoxCorner::RightTopFar]    = fc + u * farH + s * farW;
		corners[BoxCorner::RightBottomFar] = fc - u * farH + s * farW;

		corners[BoxCorner::LeftBottomNear]  = nc - u * nearH - s * nearW;
		corners[BoxCorner::LeftTopNear]     = nc + u * nearH - s * nearW;
		corners[BoxCorner::RightTopNear]    = nc + u * nearH + s * nearW;
		corners[BoxCorner::RightBottomNear] = nc - u * nearH + s * nearW;

		// Construction of frustum's planes

		EnumArray<FrustumPlane, Plane<T>> planes;
		planes[FrustumPlane::Bottom] = Plane(corners[BoxCorner::LeftBottomNear],  corners[BoxCorner::RightBottomNear], corners[BoxCorner::RightBottomFar]);
		planes[FrustumPlane::Far]    = Plane(corners[BoxCorner::RightTopFar],     corners[BoxCorner::LeftTopFar],      corners[BoxCorner::LeftBottomFar]);
		planes[FrustumPlane::Left]   = Plane(corners[BoxCorner::LeftTopNear],     corners[BoxCorner::LeftBottomNear],  corners[BoxCorner::LeftBottomFar]);
		planes[FrustumPlane::Near]   = Plane(corners[BoxCorner::LeftTopNear],     corners[BoxCorner::RightTopNear],    corners[BoxCorner::RightBottomNear]);
		planes[FrustumPlane::Right]  = Plane(corners[BoxCorner::RightBottomNear], corners[BoxCorner::RightTopNear],    corners[BoxCorner::RightBottomFar]);
		planes[FrustumPlane::Top]    = Plane(corners[BoxCorner::RightTopNear],    corners[BoxCorner::LeftTopNear],     corners[BoxCorner::LeftTopFar]);

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
		EnumArray<FrustumPlane, Plane<T>> planes;
		planes[FrustumPlane::Left].normal.x = viewProjMatrix(3, 0) + viewProjMatrix(0, 0);
		planes[FrustumPlane::Left].normal.y = viewProjMatrix(3, 1) + viewProjMatrix(0, 1);
		planes[FrustumPlane::Left].normal.z = viewProjMatrix(3, 2) + viewProjMatrix(0, 2);
		planes[FrustumPlane::Left].distance = viewProjMatrix(3, 3) + viewProjMatrix(0, 3);

		planes[FrustumPlane::Right].normal.x = viewProjMatrix(3, 0) - viewProjMatrix(0, 0);
		planes[FrustumPlane::Right].normal.y = viewProjMatrix(3, 1) - viewProjMatrix(0, 1);
		planes[FrustumPlane::Right].normal.z = viewProjMatrix(3, 2) - viewProjMatrix(0, 2);
		planes[FrustumPlane::Right].distance = viewProjMatrix(3, 3) - viewProjMatrix(0, 3);

		planes[FrustumPlane::Bottom].normal.x = viewProjMatrix(3, 0) - viewProjMatrix(1, 0);
		planes[FrustumPlane::Bottom].normal.y = viewProjMatrix(3, 1) - viewProjMatrix(1, 1);
		planes[FrustumPlane::Bottom].normal.z = viewProjMatrix(3, 2) - viewProjMatrix(1, 2);
		planes[FrustumPlane::Bottom].distance = viewProjMatrix(3, 3) - viewProjMatrix(1, 3);

		planes[FrustumPlane::Top].normal.x = viewProjMatrix(3, 0) + viewProjMatrix(1, 0);
		planes[FrustumPlane::Top].normal.y = viewProjMatrix(3, 1) + viewProjMatrix(1, 1);
		planes[FrustumPlane::Top].normal.z = viewProjMatrix(3, 2) + viewProjMatrix(1, 2);
		planes[FrustumPlane::Top].distance = viewProjMatrix(3, 3) + viewProjMatrix(1, 3);

		planes[FrustumPlane::Near].normal.x = viewProjMatrix(2, 0);
		planes[FrustumPlane::Near].normal.y = viewProjMatrix(2, 1);
		planes[FrustumPlane::Near].normal.z = viewProjMatrix(2, 2);
		planes[FrustumPlane::Near].distance = viewProjMatrix(2, 3);

		planes[FrustumPlane::Far].normal.x = viewProjMatrix(3, 0) - viewProjMatrix(2, 0);
		planes[FrustumPlane::Far].normal.y = viewProjMatrix(3, 1) - viewProjMatrix(2, 1);
		planes[FrustumPlane::Far].normal.z = viewProjMatrix(3, 2) - viewProjMatrix(2, 2);
		planes[FrustumPlane::Far].distance = viewProjMatrix(3, 3) - viewProjMatrix(2, 3);

		for (auto& plane : planes)
			plane.Normalize();

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
		for (const auto& plane : frustum.m_planes)
		{
			if (!Serialize(context, plane))
				return false;
		}

		return true;
	}

	/*!
	* \brief Deserializes a Frustum
	* \return true if successfully deserialized
	*
	* \param context Serialization context
	* \param matrix Output frustum
	*/
	template<typename T>
	bool Deserialize(SerializationContext& context, Frustum<T>* frustum, TypeTag<Frustum<T>>)
	{
		for (auto& plane : frustum->m_planes)
		{
			if (!Deserialize(context, &plane))
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
		           << "        Far: "    << frustum.GetPlane(Nz::FrustumPlane::Far) << ",\n"
		           << "        Left: "   << frustum.GetPlane(Nz::FrustumPlane::Left) << ",\n"
		           << "        Near: "   << frustum.GetPlane(Nz::FrustumPlane::Near) << ",\n"
		           << "        Right: "  << frustum.GetPlane(Nz::FrustumPlane::Right) << ",\n"
		           << "        Top: "    << frustum.GetPlane(Nz::FrustumPlane::Top) << ")\n";
	}
}

