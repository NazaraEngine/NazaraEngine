// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Renderer/DebugDrawer.hpp>
#include <Nazara/Renderer/Debug.hpp>

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
}

#include <Nazara/Renderer/DebugOff.hpp>
