// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VERTEXSTRUCT_HPP
#define NAZARA_VERTEXSTRUCT_HPP

#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>

/******************************* Structures 2D *******************************/

struct NzVertexStruct_XY
{
	NzVector2f position;
};

struct NzVertexStruct_XY_UV : public NzVertexStruct_XY
{
	NzVector2f uv;
};

/******************************* Structures 3D *******************************/

struct NzVertexStruct_XYZ
{
	NzVector3f position;
};

struct NzVertexStruct_XYZ_Normal : public NzVertexStruct_XYZ
{
	NzVector3f normal;
};

struct NzVertexStruct_XYZ_Normal_UV : public NzVertexStruct_XYZ_Normal
{
	NzVector2f uv;
};

struct NzVertexStruct_XYZ_Normal_UV_Tangent : public NzVertexStruct_XYZ_Normal_UV
{
	NzVector3f tangent;
};

struct NzVertexStruct_XYZ_UV : public NzVertexStruct_XYZ
{
	NzVector2f uv;
};

/************************* Structures 3D (+ Skinning) ************************/

struct NzVertexStruct_XYZ_Normal_UV_Tangent_Skinning : public NzVertexStruct_XYZ_Normal_UV_Tangent
{
	nzInt32 weightCount;

	NzVector4f weights;
	NzVector4i32 jointIndexes;
};

#endif // NAZARA_VERTEXSTRUCT_HPP
