// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
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
		DrawLine(corners[BoxCorner::NearLeftBottom],  corners[BoxCorner::NearRightBottom], color);
		DrawLine(corners[BoxCorner::NearLeftBottom],  corners[BoxCorner::NearLeftTop],     color);
		DrawLine(corners[BoxCorner::NearLeftBottom],  corners[BoxCorner::FarLeftBottom],   color);
		DrawLine(corners[BoxCorner::FarRightTop],     corners[BoxCorner::FarLeftTop],      color);
		DrawLine(corners[BoxCorner::FarRightTop],     corners[BoxCorner::FarRightBottom],  color);
		DrawLine(corners[BoxCorner::FarRightTop],     corners[BoxCorner::NearRightTop],    color);
		DrawLine(corners[BoxCorner::FarLeftBottom],   corners[BoxCorner::FarRightBottom],  color);
		DrawLine(corners[BoxCorner::FarLeftBottom],   corners[BoxCorner::FarLeftTop],      color);
		DrawLine(corners[BoxCorner::NearLeftTop],     corners[BoxCorner::NearRightTop],    color);
		DrawLine(corners[BoxCorner::NearLeftTop],     corners[BoxCorner::FarLeftTop],      color);
		DrawLine(corners[BoxCorner::NearRightBottom], corners[BoxCorner::NearRightTop],    color);
		DrawLine(corners[BoxCorner::NearRightBottom], corners[BoxCorner::FarRightBottom],  color);
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

	inline void DebugDrawer::DrawPoint(const Vector3f& point, const Color& color, float boxSize)
	{
		return DrawBox(Boxf(point - Vector3f(boxSize * 0.5f), Vector3f(boxSize)), color);
	}
}

