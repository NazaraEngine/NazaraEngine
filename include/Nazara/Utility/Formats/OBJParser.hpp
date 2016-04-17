// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FORMATS_OBJPARSER_HPP
#define NAZARA_FORMATS_OBJPARSER_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Core/Stream.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <Nazara/Utility/Config.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_UTILITY_API OBJParser
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
				String name;
				unsigned int material;
			};

			OBJParser(Stream& stream$);
			~OBJParser();

			const String* GetMaterials() const;
			unsigned int GetMaterialCount() const;
			const Mesh* GetMeshes() const;
			unsigned int GetMeshCount() const;
			const String& GetMtlLib() const;
			const Vector3f* GetNormals() const;
			unsigned int GetNormalCount() const;
			const Vector4f* GetPositions() const;
			unsigned int GetPositionCount() const;
			const Vector3f* GetTexCoords() const;
			unsigned int GetTexCoordCount() const;

			bool Parse(std::size_t reservedVertexCount = 100);

		private:
			bool Advance(bool required = true);
			void Error(const String& message);
			void Warning(const String& message);
			void UnrecognizedLine(bool error = false);

			std::vector<Mesh> m_meshes;
			std::vector<String> m_materials;
			std::vector<Vector3f> m_normals;
			std::vector<Vector4f> m_positions;
			std::vector<Vector3f> m_texCoords;
			Stream& m_stream;
			String m_currentLine;
			String m_mtlLib;
			bool m_keepLastLine;
			unsigned int m_lineCount;
			unsigned int m_streamFlags;
	};
}

#endif // NAZARA_FORMATS_OBJPARSER_HPP
