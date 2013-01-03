// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Renderer module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_DEBUGDRAWER_HPP
#define NAZARA_DEBUGDRAWER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Color.hpp>
#include <Nazara/Math/Cube.hpp>
#include <Nazara/Utility/SubMesh.hpp>

class NzAxisAlignedBox;
class NzSkeleton;

class NAZARA_API NzDebugDrawer
{
	public:
		static void Draw(const NzAxisAlignedBox& aabb);
		static void Draw(const NzCubef& cube);
		static void Draw(const NzCubei& cube);
		static void Draw(const NzCubeui& cube);
		static void Draw(const NzSkeleton* skeleton);
		static void DrawNormals(const NzSubMesh* subMesh);

		static bool Initialize();

		static bool GetDepthTest();
		static float GetLineWidth();
		static float GetPointSize();
		static NzColor GetPrimaryColor();
		static NzColor GetSecondaryColor();

		static void SetDepthTest(bool shouldTest);
		static void SetLineWidth(float width);
		static void SetPointSize(float size);
		static void SetPrimaryColor(const NzColor& color);
		static void SetSecondaryColor(const NzColor& color);

		static void Uninitialize();
};

#endif // NAZARA_DEBUG_DRAWER_HPP
