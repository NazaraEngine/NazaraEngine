// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#ifndef NAZARA_LOADERS_MD2_CONSTANTS_HPP
#define NAZARA_LOADERS_MD2_CONSTANTS_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Math/Vector3.hpp>

namespace Nz
{
	struct MD2_Header
	{
		UInt32 ident;         // nombre magique : "IDP2"
		UInt32 version;       // version du format : 8

		UInt32 skinwidth;     // largeur texture
		UInt32 skinheight;    // hauteur texture

		UInt32 framesize;     // taille d'une frame en octets

		UInt32 num_skins;     // nombre de skins
		UInt32 num_vertices;  // nombre de vertices par frame
		UInt32 num_st;        // nombre de coordonnées de texture
		UInt32 num_tris;      // nombre de triangles
		UInt32 num_glcmds;    // nombre de commandes opengl
		UInt32 num_frames;    // nombre de frames

		UInt32 offset_skins;  // offset données skins
		UInt32 offset_st;     // offset données coordonnées de texture
		UInt32 offset_tris;   // offset données triangles
		UInt32 offset_frames; // offset données frames
		UInt32 offset_glcmds; // offset données commandes OpenGL
		UInt32 offset_end;    // offset fin de fichier
	};

	static_assert(sizeof(MD2_Header) == 17*sizeof(UInt32), "MD2_Header must be packed");

	struct MD2_Vertex
	{
		UInt8 x, y, z;
		UInt8 n;
	};

	static_assert(sizeof(MD2_Vertex) == 4*sizeof(UInt8), "MD2_Vertex must be packed");

	struct MD2_TexCoord
	{
		Int16 u, v;
	};

	static_assert(sizeof(MD2_TexCoord) == 2*sizeof(Int16), "MD2_TexCoord must be packed");

	struct MD2_Triangle
	{
		UInt16 vertices[3];
		UInt16 texCoords[3];
	};

	static_assert(sizeof(MD2_Triangle) == 2*3*sizeof(UInt16), "MD2_Triangle must be packed");

	extern const UInt32 md2Ident;
	extern const Vector3f md2Normals[162];
}

#endif // NAZARA_LOADERS_MD2_CONSTANTS_HPP
