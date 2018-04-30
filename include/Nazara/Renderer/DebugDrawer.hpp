// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEBUGDRAWER_HPP
#define NAZARA_DEBUGDRAWER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Math/BoundingVolume.hpp>
#include <Nazara/Math/Box.hpp>
#include <Nazara/Math/Frustum.hpp>
#include <Nazara/Math/OrientedBox.hpp>
#include <Nazara/Renderer/Config.hpp>

namespace Nz
{
	class Skeleton;
	class StaticMesh;

	class NAZARA_RENDERER_API DebugDrawer
	{
		public:
			static void Draw(const BoundingVolumef& volume);
			static void Draw(const Boxf& box);
			static void Draw(const Boxi& box);
			static void Draw(const Boxui& box);
			static void Draw(const Frustumf& frustum);
			static void Draw(const OrientedBoxf& orientedBox);
			static void Draw(const Skeleton* skeleton);
			static void Draw(const Vector3f& position, float size = 0.1f);
			static void DrawAxes(const Vector3f& position = Vector3f::Zero(), float size = 1.f);
			static void DrawBinormals(const StaticMesh* subMesh);
			static void DrawCone(const Vector3f& origin, const Quaternionf& rotation, float angle, float length);
			static void DrawLine(const Vector3f& p1, const Vector3f& p2);
			static void DrawPoints(const Vector3f* ptr, unsigned int pointCount);
			static void DrawNormals(const StaticMesh* subMesh, float normalLength = 0.01f);
			static void DrawTangents(const StaticMesh* subMesh);

			static void EnableDepthBuffer(bool depthBuffer);

			static float GetLineWidth();
			static float GetPointSize();
			static Color GetPrimaryColor();
			static Color GetSecondaryColor();

			static bool Initialize();
			static bool IsDepthBufferEnabled();

			static void SetLineWidth(float width);
			static void SetPointSize(float size);
			static void SetPrimaryColor(const Color& color);
			static void SetSecondaryColor(const Color& color);

			static void Uninitialize();
	};
}

#endif // NAZARA_DEBUG_DRAWER_HPP
