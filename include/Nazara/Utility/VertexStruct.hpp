// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_VERTEXSTRUCT_HPP
#define NAZARA_VERTEXSTRUCT_HPP

#include <Nazara/Math/Vector2.hpp>
#include <Nazara/Math/Vector3.hpp>

struct NzVertexStruct_XY
{
	NzVector2f position;
};

struct NzVertexStruct_XY_Color : public NzVertexStruct_XY
{
	NzVector3f color;
};

/////////////////////////////////////////

struct NzVertexStruct_XYZ
{
	NzVector3f position;
};

struct NzVertexStruct_XYZ_Color : public NzVertexStruct_XYZ
{
	NzVector3f color;
};

struct NzVertexStruct_XYZ_Normal : public NzVertexStruct_XYZ
{
	NzVector3f normal;
};

struct NzVertexStruct_XYZ_Normal_Color : public NzVertexStruct_XYZ_Normal
{
	NzVector3f color;
};

/////////////////////////////////////////

struct NzVertexStruct_XYZ_UV : public NzVertexStruct_XYZ
{
	NzVector2f uv;
};

struct NzVertexStruct_XYZ_UV_Color : public NzVertexStruct_XYZ_UV
{
	NzVector3f color;
};

struct NzVertexStruct_XYZ_Normal_UV : public NzVertexStruct_XYZ_Normal
{
	NzVector2f uv;
};

struct NzVertexStruct_XYZ_Normal_UV_Color : public NzVertexStruct_XYZ_Normal_UV
{
	NzVector3f color;
};

struct NzVertexStruct_XYZ_Normal_UV_Tangent : public NzVertexStruct_XYZ_Normal_UV
{
	NzVector3f tangent;
};

struct NzVertexStruct_XYZ_Normal_UV_Tangent_Color : public NzVertexStruct_XYZ_Normal_UV_Tangent
{
	NzVector3f color;
};

/////////////////////////////////////////

struct NzVertexStruct_XYZ_UV_UV2 : public NzVertexStruct_XYZ_UV
{
	NzVector2f uv2;
};

struct NzVertexStruct_XYZ_UV_UV2_Color : public NzVertexStruct_XYZ_UV_UV2
{
	NzVector3f color;
};

/////////////////////////////////////////

struct NzVertexStruct_XYZ_Normal_UV_UV2 : public NzVertexStruct_XYZ_Normal_UV
{
	NzVector2f uv2;
};

struct NzVertexStruct_XYZ_Normal_UV_UV2_Color : public NzVertexStruct_XYZ_Normal_UV_UV2
{
	NzVector3f color;
};

struct NzVertexStruct_XYZ_Normal_UV_UV2_Tangent : public NzVertexStruct_XYZ_Normal_UV_UV2
{
	NzVector3f tangent;
};

/////////////////////////////////////////

struct NzVertexStruct_XYZ_Normal_UV_UV2_Tangent_Color : public NzVertexStruct_XYZ_Normal_UV_UV2_Tangent
{
	NzVector3f color;
};

#endif // NAZARA_VERTEXSTRUCT_HPP
