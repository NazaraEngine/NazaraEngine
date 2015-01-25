// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VERTEXSTRUCT_HPP
#define NAZARA_VERTEXSTRUCT_HPP

#include <Nazara/Core/Color.hpp>
#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>

/******************************* Structures 2D *******************************/

struct NzVertexStruct_XY
{
	NzVector2f position;
};

struct NzVertexStruct_XY_Color : NzVertexStruct_XY
{
	NzColor color;
};

struct NzVertexStruct_XY_UV : NzVertexStruct_XY
{
	NzVector2f uv;
};

/******************************* Structures 3D *******************************/

struct NzVertexStruct_XYZ
{
	NzVector3f position;
};

struct NzVertexStruct_XYZ_Color : NzVertexStruct_XYZ
{
	NzColor color;
};

struct NzVertexStruct_XYZ_Color_UV : NzVertexStruct_XYZ_Color
{
	NzVector2f uv;
};

struct NzVertexStruct_XYZ_Normal : NzVertexStruct_XYZ
{
	NzVector3f normal;
};

struct NzVertexStruct_XYZ_Normal_UV : NzVertexStruct_XYZ_Normal
{
	NzVector2f uv;
};

struct NzVertexStruct_XYZ_Normal_UV_Tangent : NzVertexStruct_XYZ_Normal_UV
{
	NzVector3f tangent;
};

struct NzVertexStruct_XYZ_UV : NzVertexStruct_XYZ
{
	NzVector2f uv;
};

/************************* Structures 3D (+ Skinning) ************************/

struct NzVertexStruct_XYZ_Normal_UV_Tangent_Skinning : NzVertexStruct_XYZ_Normal_UV_Tangent
{
	nzInt32 weightCount;

	NzVector4f weights;
	NzVector4i32 jointIndexes;
};

#endif // NAZARA_VERTEXSTRUCT_HPP
