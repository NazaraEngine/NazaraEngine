// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_LOADERS_MD2_CONSTANTS_HPP
#define NAZARA_LOADERS_MD2_CONSTANTS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector3.hpp>

struct MD2_Header
{
	nzUInt32 ident;         // nombre magique : "IDP2"
	nzUInt32 version;       // version du format : 8

	nzUInt32 skinwidth;     // largeur texture
	nzUInt32 skinheight;    // hauteur texture

	nzUInt32 framesize;     // taille d'une frame en octets

	nzUInt32 num_skins;     // nombre de skins
	nzUInt32 num_vertices;  // nombre de vertices par frame
	nzUInt32 num_st;        // nombre de coordonnées de texture
	nzUInt32 num_tris;      // nombre de triangles
	nzUInt32 num_glcmds;    // nombre de commandes opengl
	nzUInt32 num_frames;    // nombre de frames

	nzUInt32 offset_skins;  // offset données skins
	nzUInt32 offset_st;     // offset données coordonnées de texture
	nzUInt32 offset_tris;   // offset données triangles
	nzUInt32 offset_frames; // offset données frames
	nzUInt32 offset_glcmds; // offset données commandes OpenGL
	nzUInt32 offset_end;    // offset fin de fichier
};

static_assert(sizeof(MD2_Header) == 17*sizeof(nzUInt32), "MD2_Header must be packed");

struct MD2_Vertex
{
	nzUInt8 x, y, z;
	nzUInt8 n;
};

static_assert(sizeof(MD2_Vertex) == 4*sizeof(nzUInt8), "MD2_Vertex must be packed");

struct MD2_TexCoord
{
	nzInt16 u, v;
};

static_assert(sizeof(MD2_TexCoord) == 2*sizeof(nzInt16), "MD2_TexCoord must be packed");

struct MD2_Triangle
{
	nzUInt16 vertices[3];
	nzUInt16 texCoords[3];
};

static_assert(sizeof(MD2_Triangle) == 2*3*sizeof(nzUInt16), "MD2_Triangle must be packed");

extern const nzUInt32 md2Ident;
extern const NzVector3f md2Normals[162];

#endif // NAZARA_LOADERS_MD2_CONSTANTS_HPP
