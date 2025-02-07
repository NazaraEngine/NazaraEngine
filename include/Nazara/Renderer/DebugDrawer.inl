// Copyright (C) 2025 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <NazaraUtils/EnumArray.hpp>

namespace Nz
{
	inline void DebugDrawer::DrawBox(const Boxf& box, const Color& color)
	{
		Vector3f max = box.GetMaximum();
		Vector3f min = box.GetMinimum();

		DrawLine({ min.x, min.y, min.z }, { max.x, min.y, min.z }, color);
		DrawLine({ min.x, min.y, min.z }, { min.x, max.y, min.z }, color);
		DrawLine({ min.x, min.y, min.z }, { min.x, min.y, max.z }, color);
		DrawLine({ max.x, max.y, max.z }, { min.x, max.y, max.z }, color);
		DrawLine({ max.x, max.y, max.z }, { max.x, min.y, max.z }, color);
		DrawLine({ max.x, max.y, max.z }, { max.x, max.y, min.z }, color);
		DrawLine({ min.x, min.y, max.z }, { max.x, min.y, max.z }, color);
		DrawLine({ min.x, min.y, max.z }, { min.x, max.y, max.z }, color);
		DrawLine({ min.x, max.y, min.z }, { max.x, max.y, min.z }, color);
		DrawLine({ min.x, max.y, min.z }, { min.x, max.y, max.z }, color);
		DrawLine({ max.x, min.y, min.z }, { max.x, max.y, min.z }, color);
		DrawLine({ max.x, min.y, min.z }, { max.x, min.y, max.z }, color);
	}

	inline void DebugDrawer::DrawBoxCorners(const EnumArray<BoxCorner, Vector3f>& corners, const Color& color)
	{
		DrawLine(corners[BoxCorner::LeftBottomNear],  corners[BoxCorner::RightBottomNear], color);
		DrawLine(corners[BoxCorner::LeftBottomNear],  corners[BoxCorner::LeftTopNear],     color);
		DrawLine(corners[BoxCorner::LeftBottomNear],  corners[BoxCorner::LeftBottomFar],   color);
		DrawLine(corners[BoxCorner::RightTopFar],     corners[BoxCorner::LeftTopFar],      color);
		DrawLine(corners[BoxCorner::RightTopFar],     corners[BoxCorner::RightBottomFar],  color);
		DrawLine(corners[BoxCorner::RightTopFar],     corners[BoxCorner::RightTopNear],    color);
		DrawLine(corners[BoxCorner::LeftBottomFar],   corners[BoxCorner::RightBottomFar],  color);
		DrawLine(corners[BoxCorner::LeftBottomFar],   corners[BoxCorner::LeftTopFar],      color);
		DrawLine(corners[BoxCorner::LeftTopNear],     corners[BoxCorner::RightTopNear],    color);
		DrawLine(corners[BoxCorner::LeftTopNear],     corners[BoxCorner::LeftTopFar],      color);
		DrawLine(corners[BoxCorner::RightBottomNear], corners[BoxCorner::RightTopNear],    color);
		DrawLine(corners[BoxCorner::RightBottomNear], corners[BoxCorner::RightBottomFar],  color);
	}

	inline void DebugDrawer::DrawFrustum(const Frustumf& frustum, const Color& color)
	{
		DrawBoxCorners(frustum.ComputeCorners(), color);
	}

	inline void DebugDrawer::DrawLine(const Vector3f& start, const Vector3f& end, const Color& color)
	{
		return DrawLine(start, end, color, color);
	}

	inline void DebugDrawer::DrawLine(const Vector3f& start, const Vector3f& end, const Color& startColor, const Color& endColor)
	{
		auto& startVertex = m_lineVertices.emplace_back();
		startVertex.color = startColor;
		startVertex.position = start;

		auto& endVertex = m_lineVertices.emplace_back();
		endVertex.color = endColor;
		endVertex.position = end;
	}

	inline void DebugDrawer::DrawLines(std::span<const Vector3f> positions, const Color& color)
	{
		std::size_t offset = m_lineVertices.size();
		m_lineVertices.resize(offset + positions.size());
		for (std::size_t i = 0; i < positions.size(); ++i)
		{
			auto& vertex = m_lineVertices[i];
			vertex.color = color;
			vertex.position = positions[i];
		}
	}

	inline void DebugDrawer::DrawLines(std::span<const UInt16> indices, std::span<const Vector3f> positions, const Color& color)
	{
		std::size_t offset = m_lineVertices.size();
		m_lineVertices.resize(offset + indices.size());
		for (std::size_t i = 0; i < indices.size(); ++i)
		{
			auto& vertex = m_lineVertices[i];
			vertex.color = color;
			vertex.position = positions[indices[i]];
		}
	}

	inline void DebugDrawer::DrawPoint(const Vector3f& point, const Color& color, float boxSize)
	{
		return DrawBox(Boxf(point - Vector3f(boxSize * 0.5f), Vector3f(boxSize)), color);
	}

	inline void DebugDrawer::DrawSphere(const Vector3f& point, float radius, const Color& color)
	{
		return DrawSphere(Spheref(point, radius), color);
	}
}
