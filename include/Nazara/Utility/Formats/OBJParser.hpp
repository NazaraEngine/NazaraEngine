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
			struct Face;
			struct FaceVertex;
			struct Mesh;

			OBJParser() = default;
			~OBJParser() = default;

			inline void Clear();

			inline String* GetMaterials();
			inline const String* GetMaterials() const;
			inline UInt32 GetMaterialCount() const;
			inline Mesh* GetMeshes();
			inline const Mesh* GetMeshes() const;
			inline UInt32 GetMeshCount() const;
			inline const String& GetMtlLib() const;
			inline Vector3f* GetNormals();
			inline const Vector3f* GetNormals() const;
			inline UInt32 GetNormalCount() const;
			inline Vector4f* GetPositions();
			inline const Vector4f* GetPositions() const;
			inline UInt32 GetPositionCount() const;
			inline Vector3f* GetTexCoords();
			inline const Vector3f* GetTexCoords() const;
			inline UInt32 GetTexCoordCount() const;

			bool Parse(Stream& stream, UInt32 reservedVertexCount = 100);

			bool Save(Stream& stream) const;

			inline String* SetMaterialCount(UInt32 materialCount);
			inline Mesh* SetMeshCount(UInt32 meshCount);
			inline void SetMtlLib(const String& mtlLib);
			inline Vector3f* SetNormalCount(UInt32 normalCount);
			inline Vector4f* SetPositionCount(UInt32 positionCount);
			inline Vector3f* SetTexCoordCount(UInt32 texCoordCount);

			struct Face
			{
				UInt32 firstVertex;
				UInt32 vertexCount;
			};

			struct FaceVertex
			{
				UInt32 normal;
				UInt32 position;
				UInt32 texCoord;
			};

			struct Mesh
			{
				std::vector<Face> faces;
				std::vector<FaceVertex> vertices;
				String name;
				UInt32 material;
			};

		private:
			bool Advance(bool required = true);
			template<typename T> void Emit(const T& text) const;
			inline void EmitLine() const;
			template<typename T> void EmitLine(const T& line) const;
			inline void Error(const String& message);
			inline void Flush() const;
			inline void Warning(const String& message);
			inline bool UnrecognizedLine(bool error = false);

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
			unsigned int m_errorCount;
	};
}

#include <Nazara/Utility/Formats/OBJParser.inl>

#endif // NAZARA_FORMATS_OBJPARSER_HPP
