// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_FORMATS_OBJPARSER_HPP
#define NAZARA_FORMATS_OBJPARSER_HPP

#include <Nazara/Prerequisites.hpp>
#include <Nazara/Math/Vector3.hpp>
#include <Nazara/Math/Vector4.hpp>
#include <Nazara/Utility/Config.hpp>
#include <vector>

namespace Nz
{
	class NAZARA_UTILITY_API OBJParser
	{
		public:
			struct Mesh;

			OBJParser() = default;
			~OBJParser() = default;

			inline void Clear();

			bool Check(Stream& stream);

			inline std::string* GetMaterials();
			inline const std::string* GetMaterials() const;
			inline std::size_t GetMaterialCount() const;
			inline Mesh* GetMeshes();
			inline const Mesh* GetMeshes() const;
			inline std::size_t GetMeshCount() const;
			inline const std::filesystem::path& GetMtlLib() const;
			inline Vector3f* GetNormals();
			inline const Vector3f* GetNormals() const;
			inline std::size_t GetNormalCount() const;
			inline Vector4f* GetPositions();
			inline const Vector4f* GetPositions() const;
			inline std::size_t GetPositionCount() const;
			inline Vector3f* GetTexCoords();
			inline const Vector3f* GetTexCoords() const;
			inline std::size_t GetTexCoordCount() const;

			bool Parse(Stream& stream, std::size_t reservedVertexCount = 100);

			bool Save(Stream& stream) const;

			inline std::string* SetMaterialCount(std::size_t materialCount);
			inline Mesh* SetMeshCount(std::size_t meshCount);
			inline void SetMtlLib(const std::filesystem::path& mtlLib);
			inline Vector3f* SetNormalCount(std::size_t normalCount);
			inline Vector4f* SetPositionCount(std::size_t positionCount);
			inline Vector3f* SetTexCoordCount(std::size_t texCoordCount);

			struct Face
			{
				std::size_t firstVertex;
				std::size_t vertexCount;
			};

			struct FaceVertex
			{
				std::size_t normal;
				std::size_t position;
				std::size_t texCoord;
			};

			struct Mesh
			{
				std::vector<Face> faces;
				std::vector<FaceVertex> vertices;
				std::string name;
				std::size_t material;
			};

		private:
			bool Advance(bool required = true);
			template<typename T> void Emit(const T& text) const;
			inline void EmitLine() const;
			template<typename T> void EmitLine(const T& line) const;
			inline void Error(const std::string& message);
			inline void Flush() const;
			inline void Warning(const std::string& message);
			inline bool UnrecognizedLine(bool error = false);

			std::vector<Mesh> m_meshes;
			std::vector<std::string> m_materials;
			std::vector<Vector3f> m_normals;
			std::vector<Vector4f> m_positions;
			std::vector<Vector3f> m_texCoords;
			mutable Stream* m_currentStream;
			std::string m_currentLine;
			std::filesystem::path m_mtlLib;
			mutable std::ostringstream m_outputStream;
			bool m_keepLastLine;
			unsigned int m_lineCount;
			unsigned int m_errorCount;
	};
}

#include <Nazara/Utility/Formats/OBJParser.inl>

#endif // NAZARA_FORMATS_OBJPARSER_HPP
