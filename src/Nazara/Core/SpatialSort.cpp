// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/SpatialSort.hpp>
#include <Nazara/Math/Plane.hpp>

namespace Nz
{
	void SpatialSort::Append(SparsePtr<const Nz::Vector3f> vertices, UInt32 vertexCount, UInt32 indexOffset, bool sort)
	{
		m_vertices.reserve(m_vertices.size() + vertexCount);

		for (UInt32 i = 0; i < vertexCount; ++i)
		{
			Vertex& vertex = m_vertices.emplace_back();
			vertex.position = vertices[i];
			vertex.index = indexOffset + i;
		}

		m_isSorted = false;

		if (sort)
			Sort();
	}

	void SpatialSort::FindPositions(const Vector3f& position, float tolerance, std::vector<UInt32>& results) const
	{
		NazaraAssertMsg(m_isSorted, "Spatial sort is not sorted");

		Planef plane(m_planeNormal, 0.0f); //< TODO: Would another distance help?

		float dist = plane.SignedDistance(position);
		float minDist = dist - tolerance;
		float maxDist = dist + tolerance;

		float toleranceSq = tolerance * tolerance;

		auto it = std::lower_bound(m_vertices.begin(), m_vertices.end(), minDist, [](const Vertex& vert, float dist) { return vert.distance < dist; });
		while (it != m_vertices.end())
		{
			const Vertex& vert = *it;
			if (vert.distance >= maxDist)
				break;

			if (Vector3f::SquaredDistance(vert.position, position) <= toleranceSq)
				results.push_back(vert.index);

			++it;
		}
	}

	void SpatialSort::Sort()
	{
		NazaraAssertMsg(!m_isSorted, "Spatial sort is already sorted");

		Nz::Planef plane(m_planeNormal, 0.0f); //< FIXME: Would another distance help?

		for (Vertex& vertex : m_vertices)
			vertex.distance = plane.SignedDistance(vertex.position);

		std::sort(m_vertices.begin(), m_vertices.end(), [](const Vertex& lhs, const Vertex& rhs) { return lhs.distance < rhs.distance; });
		m_isSorted = true;
	}
}
