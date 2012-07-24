// Copyright (C) 2011 Jérôme Leclercq
// This file is part of the "Ungine".
// For conditions of distribution and use, see copyright notice in Core.h

#ifndef NAZARA_LOADERS_MD2_CONSTANTS_HPP
#define NAZARA_LOADERS_MD2_CONSTANTS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <vector>

struct md2_header
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

struct md2_vertex
{
	nzUInt8 x, y, z;
	nzUInt8 n;
};

struct md2_texCoord
{
	nzInt16 u, v;
};

struct md2_triangle
{
	nzUInt16 vertices[3];
	nzUInt16 texCoords[3];
};

struct md2_frame
{
	NzVector3f              scale;
	NzVector3f              translate;
	char                    name[16];
	std::vector<md2_vertex>	vertices;
};

extern const nzUInt32 md2Ident;
extern const NzVector3f md2Normals[162];

#endif // NAZARA_LOADERS_MD2_CONSTANTS_HPP
