// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/MaterialData.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/TriangleIterator.hpp>
#include <Nazara/Utility/VertexMapper.hpp>
#include <Nazara/Utility/Formats/MTLParser.hpp>
#include <Nazara/Utility/Formats/OBJParser.hpp>
#include <map>
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

				std::size_t GetCount() const
				{
					return m_count;
				}

				std::size_t Insert(const T& data)
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
				std::size_t m_count;
				std::map<T, std::size_t> m_cache;
				T* m_buffer;
		};

		bool IsSupported(const std::string_view& extension)
		{
			return (extension == "obj");
		}

		bool SaveToStream(const Mesh& mesh, const std::string& format, Stream& stream, const MeshParams& parameters)
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

			std::size_t worstCacheVertexCount = mesh.GetVertexCount();
			OBJParser objFormat;
			objFormat.SetNormalCount(worstCacheVertexCount);
			objFormat.SetPositionCount(worstCacheVertexCount);
			objFormat.SetTexCoordCount(worstCacheVertexCount);

			std::filesystem::path mtlPath = stream.GetPath();
			if (!mtlPath.empty())
			{
				mtlPath.replace_extension(".mtl");
				std::filesystem::path fileName = mtlPath.filename();
				if (!fileName.empty())
					objFormat.SetMtlLib(fileName);
			}

			VertexCache<Vector3f> normalCache(objFormat.GetNormals());
			VertexCache<Vector4f> positionCache(objFormat.GetPositions());
			VertexCache<Vector3f> texCoordsCache(objFormat.GetTexCoords());

			// Materials
			MTLParser mtlFormat;
			std::unordered_set<std::string> registredMaterials;

			std::size_t matCount = mesh.GetMaterialCount();
			std::string* materialNames = objFormat.SetMaterialCount(matCount);
			for (std::size_t i = 0; i < matCount; ++i)
			{
				const ParameterList& matData = mesh.GetMaterialData(i);

				std::string name;
				if (!matData.GetStringParameter(MaterialData::Name, &name))
					name = "material_" + std::to_string(i);

				// Makes sure we only have one material of that name
				while (registredMaterials.find(name) != registredMaterials.end())
					name += '_';

				registredMaterials.insert(name);
				materialNames[i] = name;

				MTLParser::Material* material = mtlFormat.AddMaterial(name);

				if (!matData.GetStringParameter(MaterialData::FilePath, &material->diffuseMap))
				{
					Color colorVal;
					double dValue;

					if (matData.GetColorParameter(MaterialData::AmbientColor, &colorVal))
						material->ambient = colorVal;

					if (matData.GetColorParameter(MaterialData::DiffuseColor, &colorVal))
						material->diffuse = colorVal;

					if (matData.GetColorParameter(MaterialData::SpecularColor, &colorVal))
						material->specular = colorVal;

					if (matData.GetDoubleParameter(MaterialData::Shininess, &dValue))
						material->shininess = float(dValue);

					matData.GetStringParameter(MaterialData::AlphaTexturePath, &material->alphaMap);
					matData.GetStringParameter(MaterialData::DiffuseTexturePath, &material->diffuseMap);
					matData.GetStringParameter(MaterialData::SpecularTexturePath, &material->specularMap);
				}
			}

			// Meshes
			std::size_t meshCount = mesh.GetSubMeshCount();
			OBJParser::Mesh* meshes = objFormat.SetMeshCount(meshCount);
			for (std::size_t i = 0; i < meshCount; ++i)
			{
				const StaticMesh& staticMesh = static_cast<const StaticMesh&>(*mesh.GetSubMesh(i));

				std::size_t triangleCount = staticMesh.GetTriangleCount();

				meshes[i].faces.resize(triangleCount);
				meshes[i].material = staticMesh.GetMaterialIndex();
				meshes[i].name = "mesh_" + std::to_string(i);
				meshes[i].vertices.resize(triangleCount * 3);

				{
					VertexMapper vertexMapper(staticMesh);

					SparsePtr<Vector3f> normalPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Normal);
					SparsePtr<Vector3f> positionPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Position);
					SparsePtr<Vector2f> texCoordsPtr = vertexMapper.GetComponentPtr<Vector2f>(VertexComponent::TexCoord);

					std::size_t faceIndex = 0;
					TriangleIterator triangle(staticMesh);
					do
					{
						OBJParser::Face& face = meshes[i].faces[faceIndex];
						face.firstVertex = faceIndex * 3;
						face.vertexCount = 3;

						for (unsigned int j = 0; j < 3; ++j)
						{
							OBJParser::FaceVertex& vertexIndices = meshes[i].vertices[face.firstVertex + j];

							std::size_t index = triangle[j];
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

			if (!mtlPath.empty())
			{
				File mtlFile(mtlPath, OpenMode::WriteOnly | OpenMode::Truncate);
				if (mtlFile.IsOpen())
					mtlFormat.Save(mtlFile);
			}

			return true;
		}
	}

	namespace Loaders
	{
		MeshSaver::Entry GetMeshSaver_OBJ()
		{
			MeshSaver::Entry entry;
			entry.formatSupport = IsSupported;
			entry.streamSaver = SaveToStream;

			return entry;
		}
	}
}
