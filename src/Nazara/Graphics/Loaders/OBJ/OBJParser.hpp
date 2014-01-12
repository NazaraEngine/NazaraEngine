// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_LOADERS_OBJ_OBJPARSER_HPP
#define NAZARA_LOADERS_OBJ_OBJPARSER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <vector>

class NzOBJParser
{
	public:
		struct FaceVertex
		{
			int normal;
			int position;
			int texCoord;
		};

		struct Face
		{
			std::vector<FaceVertex> vertices;
		};

		struct Mesh
		{
			std::vector<Face> faces;
			NzString name;
			unsigned int material;
		};

		NzOBJParser(NzInputStream& stream$);
		~NzOBJParser();

		const NzString* GetMaterials() const;
		unsigned int GetMaterialCount() const;
		const Mesh* GetMeshes() const;
		unsigned int GetMeshCount() const;
		const NzString& GetMtlLib() const;
		const NzVector3f* GetNormals() const;
		unsigned int GetNormalCount() const;
		const NzVector4f* GetPositions() const;
		unsigned int GetPositionCount() const;
		const NzVector3f* GetTexCoords() const;
		unsigned int GetTexCoordCount() const;

		bool Parse();

	private:
		bool Advance(bool required = true);
		void Error(const NzString& message);
		void Warning(const NzString& message);
		void UnrecognizedLine(bool error = false);

		std::vector<Mesh> m_meshes;
		std::vector<NzString> m_materials;
		std::vector<NzVector3f> m_normals;
		std::vector<NzVector4f> m_positions;
		std::vector<NzVector3f> m_texCoords;
		NzInputStream& m_stream;
		NzString m_currentLine;
		NzString m_mtlLib;
		bool m_keepLastLine;
		unsigned int m_lineCount;
		unsigned int m_streamFlags;
};

#endif // NAZARA_LOADERS_OBJ_OBJPARSER_HPP
