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
				std::size_t firstVertex;
				std::size_t vertexCount;
			};

			struct Mesh
			{
				std::vector<Face> faces;
				std::vector<FaceVertex> vertices;
				String name;
				std::size_t material;
			};

			OBJParser() = default;
			~OBJParser() = default;

			inline const String* GetMaterials() const;
			inline unsigned int GetMaterialCount() const;
			inline const Mesh* GetMeshes() const;
			inline unsigned int GetMeshCount() const;
			inline const String& GetMtlLib() const;
			inline const Vector3f* GetNormals() const;
			inline unsigned int GetNormalCount() const;
			inline const Vector4f* GetPositions() const;
			inline unsigned int GetPositionCount() const;
			inline const Vector3f* GetTexCoords() const;
			inline unsigned int GetTexCoordCount() const;

			bool Parse(Stream& stream, std::size_t reservedVertexCount = 100);

		private:
			bool Advance(bool required = true);
			template<typename T> void Emit(const T& text) const;
			inline void EmitLine() const;
			template<typename T> void EmitLine(const T& line) const;
			inline void Error(const String& message);
			inline void Flush() const;
			inline void Warning(const String& message);
			inline void UnrecognizedLine(bool error = false);

			std::vector<Mesh> m_meshes;
			std::vector<String> m_materials;
			std::vector<Vector3f> m_normals;
			std::vector<Vector4f> m_positions;
			std::vector<Vector3f> m_texCoords;
			mutable Stream* m_currentStream;
			String m_currentLine;
			String m_mtlLib;
			mutable StringStream m_outputStream;
			bool m_keepLastLine;
			unsigned int m_lineCount;
	};
}

#include <Nazara/Utility/Formats/OBJParser.inl>

#endif // NAZARA_FORMATS_OBJPARSER_HPP
