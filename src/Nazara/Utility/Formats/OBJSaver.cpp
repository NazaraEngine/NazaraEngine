// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Formats/OBJLoader.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/IndexMapper.hpp>
#include <Nazara/Utility/MaterialData.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/TriangleIterator.hpp>
#include <Nazara/Utility/VertexMapper.hpp>
#include <Nazara/Utility/Formats/MTLParser.hpp>
#include <Nazara/Utility/Formats/OBJParser.hpp>
#include <cstdio>
#include <limits>
#include <map>
#include <memory>
#include <unordered_set>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	namespace
	{
		template<typename T>
		class VertexCache
		{
			public:
				VertexCache(T* ptr) :
				m_count(0),
				m_buffer(ptr)
				{
				}

				UInt32 GetCount() const
				{
					return m_count;
				}

				UInt32 Insert(const T& data)
				{
					auto it = m_cache.find(data);
					if (it == m_cache.end())
					{
						it = m_cache.insert(std::make_pair(data, m_count)).first;
						m_buffer[m_count] = data;
						m_count++;
					}

					return it->second + 1;
				}

			private:
				UInt32 m_count;
				std::map<T, UInt32> m_cache;
				T* m_buffer;
		};

		bool IsSupported(const String& extension)
		{
			return (extension == "obj");
		}

		bool SaveToStream(const Mesh& mesh, const String& format, Stream& stream, const MeshParams& parameters)
		{
			NazaraUnused(parameters);

			if (!mesh.IsValid())
			{
				NazaraError("Invalid mesh");
				return false;
			}

			if (mesh.IsAnimable())
			{
				NazaraError("An animated mesh cannot be saved to " + format + " format");
				return false;
			}

			UInt32 worstCacheVertexCount = mesh.GetVertexCount();
			OBJParser objFormat;
			objFormat.SetNormalCount(worstCacheVertexCount);
			objFormat.SetPositionCount(worstCacheVertexCount);
			objFormat.SetTexCoordCount(worstCacheVertexCount);

			String mtlPath = stream.GetPath();
			if (!mtlPath.IsEmpty())
			{
				mtlPath.Replace(".obj", ".mtl");
				String fileName = mtlPath.SubStringFrom(NAZARA_DIRECTORY_SEPARATOR, -1, true);
				if (!fileName.IsEmpty())
					objFormat.SetMtlLib(fileName);
			}

			VertexCache<Vector3f> normalCache(objFormat.GetNormals());
			VertexCache<Vector4f> positionCache(objFormat.GetPositions());
			VertexCache<Vector3f> texCoordsCache(objFormat.GetTexCoords());

			// Materials
			MTLParser mtlFormat;
			std::unordered_set<String> registredMaterials;

			UInt32 matCount = mesh.GetMaterialCount();
			String* materialNames = objFormat.SetMaterialCount(matCount);
			for (UInt32 i = 0; i < matCount; ++i)
			{
				const ParameterList& matData = mesh.GetMaterialData(i);

				String name;
				if (!matData.GetStringParameter(MaterialData::Name, &name))
					name = "material_" + String::Number(i);

				// Makes sure we only have one material of that name
				while (registredMaterials.find(name) != registredMaterials.end())
					name += '_';

				registredMaterials.insert(name);
				materialNames[i] = name;

				MTLParser::Material* material = mtlFormat.AddMaterial(name);

				String strVal;
				if (matData.GetStringParameter(MaterialData::FilePath, &strVal))
					material->diffuseMap = strVal;
				else
				{
					Color colorVal;
					float fValue;

					if (matData.GetColorParameter(MaterialData::AmbientColor, &colorVal))
						material->ambient = colorVal;

					if (matData.GetColorParameter(MaterialData::DiffuseColor, &colorVal))
						material->diffuse = colorVal;

					if (matData.GetColorParameter(MaterialData::SpecularColor, &colorVal))
						material->specular = colorVal;

					if (matData.GetFloatParameter(MaterialData::Shininess, &fValue))
						material->shininess = fValue;

					if (matData.GetStringParameter(MaterialData::AlphaTexturePath, &strVal))
						material->alphaMap = strVal;

					if (matData.GetStringParameter(MaterialData::DiffuseTexturePath, &strVal))
						material->diffuseMap = strVal;

					if (matData.GetStringParameter(MaterialData::SpecularTexturePath, &strVal))
						material->specularMap = strVal;
				}
			}

			// Meshes
			UInt32 meshCount = mesh.GetSubMeshCount();
			OBJParser::Mesh* meshes = objFormat.SetMeshCount(meshCount);
			for (UInt32 i = 0; i < meshCount; ++i)
			{
				const StaticMesh* staticMesh = static_cast<const StaticMesh*>(mesh.GetSubMesh(i));

				UInt32 triangleCount = staticMesh->GetTriangleCount();

				meshes[i].faces.resize(triangleCount);
				meshes[i].material = staticMesh->GetMaterialIndex();
				meshes[i].name = "mesh_" + String::Number(i);
				meshes[i].vertices.resize(triangleCount * 3);

				{
					VertexMapper vertexMapper(staticMesh);

					SparsePtr<Vector3f> normalPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent_Normal);
					SparsePtr<Vector3f> positionPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent_Position);
					SparsePtr<Vector2f> texCoordsPtr = vertexMapper.GetComponentPtr<Vector2f>(VertexComponent_TexCoord);

					UInt32 faceIndex = 0;
					TriangleIterator triangle(staticMesh);
					do 
					{
						OBJParser::Face& face = meshes[i].faces[faceIndex];
						face.firstVertex = faceIndex * 3;
						face.vertexCount = 3;

						for (unsigned int j = 0; j < 3; ++j)
						{
							OBJParser::FaceVertex& vertexIndices = meshes[i].vertices[face.firstVertex + j];

							UInt32 index = triangle[j];
							vertexIndices.normal = normalCache.Insert(normalPtr[index]);
							vertexIndices.position = positionCache.Insert(positionPtr[index]);
							vertexIndices.texCoord = texCoordsCache.Insert(texCoordsPtr[index]);
						}

						faceIndex++;
					}
					while (triangle.Advance());
				}
			}

			objFormat.SetNormalCount(normalCache.GetCount());
			objFormat.SetPositionCount(positionCache.GetCount());
			objFormat.SetTexCoordCount(texCoordsCache.GetCount());

			objFormat.Save(stream);

			if (!mtlPath.IsEmpty())
			{
				File mtlFile(mtlPath, OpenMode_WriteOnly | OpenMode_Truncate);
				if (mtlFile.IsOpen())
					mtlFormat.Save(mtlFile);
			}

			return true;
		}
	}

	namespace Loaders
	{
		void RegisterOBJSaver()
		{
			MeshSaver::RegisterSaver(IsSupported, SaveToStream);
		}

		void UnregisterOBJSaver()
		{
			MeshSaver::UnregisterSaver(IsSupported, SaveToStream);
		}
	}
}
