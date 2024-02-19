// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#pragma once

#ifndef NAZARA_CORE_VERTEXSTRUCT_HPP
#define NAZARA_CORE_VERTEXSTRUCT_HPP

#include <Nazara/Core/Color.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>

namespace Nz
{
	/******************************* Structures 2D *******************************/

	struct VertexStruct_XY
	{
		Vector2f position;
	};

	struct VertexStruct_XY_Color : VertexStruct_XY
	{
		Color color;
	};

	struct VertexStruct_XY_UV : VertexStruct_XY
	{
		Vector2f uv;
	};

	struct VertexStruct_XY_Color_UV : VertexStruct_XY_Color
	{
		Vector2f uv;
	};

	/******************************* Structures 3D *******************************/

	struct VertexStruct_XYZ
	{
		Vector3f position;
	};

	struct VertexStruct_XYZ_Color : VertexStruct_XYZ
	{
		Color color;
	};

	struct VertexStruct_XYZ_Color_UV : VertexStruct_XYZ_Color
	{
		Vector2f uv;
	};

	struct VertexStruct_XYZ_Normal : VertexStruct_XYZ
	{
		Vector3f normal;
	};

	struct VertexStruct_XYZ_Normal_UV : VertexStruct_XYZ_Normal
	{
		Vector2f uv;
	};

	struct VertexStruct_XYZ_Normal_UV_Tangent : VertexStruct_XYZ_Normal_UV
	{
		Vector3f tangent;
	};

	struct VertexStruct_UV_SizeSinCos
	{
		Vector2f uv;
		Vector4f sizeSinCos; //< width, height, sin, cos
	};

	struct VertexStruct_UV_SizeSinCos_Color : VertexStruct_UV_SizeSinCos
	{
		Color color;
	};

	struct VertexStruct_XYZ_UV : VertexStruct_XYZ
	{
		Vector2f uv;
	};

	/************************* Structures 3D (+ Skinning) ************************/

	struct VertexStruct_XYZ_Normal_UV_Tangent_Skinning : VertexStruct_XYZ_Normal_UV_Tangent
	{
		Vector4f weights;
		Vector4i32 jointIndexes;
	};
}

#endif // NAZARA_CORE_VERTEXSTRUCT_HPP
