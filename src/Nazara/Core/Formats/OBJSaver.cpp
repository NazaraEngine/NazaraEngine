// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Formats/OBJSaver.hpp>
#include <Nazara/Core/MaterialData.hpp>
#include <Nazara/Core/Mesh.hpp>
#include <Nazara/Core/StaticMesh.hpp>
#include <Nazara/Core/TriangleIterator.hpp>
#include <Nazara/Core/VertexMapper.hpp>
#include <Nazara/Core/Formats/MTLParser.hpp>
#include <Nazara/Core/Formats/OBJParser.hpp>
#include <map>
#include <unordered_set>

namespace Nz
{
	namespace NAZARA_ANONYMOUS_NAMESPACE
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

		bool IsOBJSupportedSave(std::string_view extension)
		{
			return (extension == ".obj");
		}

		bool SaveOBJToStream(const Mesh& mesh, std::string_view format, Stream& stream, const MeshParams& parameters)
		{
			NAZARA_USE_ANONYMOUS_NAMESPACE

			NazaraUnused(parameters);

			if (!mesh.IsValid())
			{
				NazaraError("invalid mesh");
				return false;
			}

			if (mesh.IsAnimable())
			{
				NazaraError("an animated mesh cannot be saved to {0} format", format);
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
				if (auto result = matData.GetStringParameter(MaterialData::Name))
					name = std::move(result).GetValue();
				else
					name = "material_" + std::to_string(i);

				// Makes sure we only have one material of that name
				while (registredMaterials.find(name) != registredMaterials.end())
					name += '_';

				registredMaterials.insert(name);
				materialNames[i] = name;

				MTLParser::Material* material = mtlFormat.AddMaterial(name);

				auto pathResult = matData.GetStringParameter(MaterialData::FilePath);
				if (!pathResult)
				{
					if (auto result = matData.GetColorParameter(MaterialData::AmbientColor))
						material->ambient = result.GetValue();

					if (auto result = matData.GetColorParameter(MaterialData::BaseColor))
						material->diffuse = result.GetValue();

					if (auto result = matData.GetColorParameter(MaterialData::SpecularColor))
						material->specular = result.GetValue();

					if (auto result = matData.GetDoubleParameter(MaterialData::Shininess))
						material->shininess = SafeCast<float>(result.GetValue());

					if (auto result = matData.GetStringParameter(MaterialData::AlphaTexturePath))
						material->alphaMap = std::move(result).GetValue();

					if (auto result = matData.GetStringParameter(MaterialData::BaseColorTexturePath))
						material->diffuseMap = std::move(result).GetValue();

					if (auto result = matData.GetStringParameter(MaterialData::SpecularTexturePath))
						material->specularMap = std::move(result).GetValue();
				}
				else
					material->diffuseMap = std::move(pathResult).GetValue();
			}

			// Meshes
			std::size_t meshCount = mesh.GetSubMeshCount();
			OBJParser::Mesh* meshes = objFormat.SetMeshCount(meshCount);
			for (std::size_t i = 0; i < meshCount; ++i)
			{
				StaticMesh& staticMesh = static_cast<StaticMesh&>(*mesh.GetSubMesh(i));

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
					TriangleIterator triangleIt(staticMesh);
					do
					{
						OBJParser::Face& face = meshes[i].faces[faceIndex];
						face.firstVertex = faceIndex * 3;
						face.vertexCount = 3;

						for (unsigned int j = 0; j < 3; ++j)
						{
							OBJParser::FaceVertex& vertexIndices = meshes[i].vertices[face.firstVertex + j];

							std::size_t index = triangleIt[j];
							vertexIndices.position = positionCache.Insert(positionPtr[index]);

							if (normalPtr)
								vertexIndices.normal = normalCache.Insert(normalPtr[index]);
							else
								vertexIndices.normal = 0;

							if (texCoordsPtr)
								vertexIndices.texCoord = texCoordsCache.Insert(texCoordsPtr[index]);
							else
								vertexIndices.texCoord = 0;
						}

						faceIndex++;
					}
					while (triangleIt.Advance());
				}
			}

			objFormat.SetNormalCount(normalCache.GetCount());
			objFormat.SetPositionCount(positionCache.GetCount());
			objFormat.SetTexCoordCount(texCoordsCache.GetCount());

			objFormat.Save(stream);

			if (!mtlPath.empty())
			{
				File mtlFile(mtlPath, OpenMode::Write | OpenMode::Truncate);
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
			NAZARA_USE_ANONYMOUS_NAMESPACE

			MeshSaver::Entry entry;
			entry.formatSupport = IsOBJSupportedSave;
			entry.streamSaver = SaveOBJToStream;

			return entry;
		}
	}
}
