// Copyright (C) 2016 Jérôme Leclercq
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
#include <Nazara/Utility/Formats/MTLParser.hpp>
#include <Nazara/Utility/Formats/OBJParser.hpp>
#include <cstdio>
#include <limits>
#include <memory>
#include <unordered_map>
#include <Nazara/Utility/Debug.hpp>

///TODO: N'avoir qu'un seul VertexBuffer communs à tous les meshes

namespace Nz
{
	namespace
	{
		bool IsSupported(const String& extension)
		{
			return (extension == "obj");
		}

		Ternary Check(Stream& stream, const MeshParams& parameters)
		{
			NazaraUnused(stream);

			bool skip;
			if (parameters.custom.GetBooleanParameter("SkipNativeOBJLoader", &skip) && skip)
				return Ternary_False;

			return Ternary_Unknown;
		}

		bool ParseMTL(Mesh* mesh, const String& filePath, const String* materials, const OBJParser::Mesh* meshes, unsigned int meshCount)
		{
			File file(filePath);
			if (!file.Open(OpenMode_ReadOnly | OpenMode_Text))
			{
				NazaraError("Failed to open MTL file (" + file.GetPath() + ')');
				return false;
			}

			MTLParser materialParser;
			if (!materialParser.Parse(file))
			{
				NazaraError("MTL parser failed");
				return false;
			}

			std::unordered_map<String, ParameterList> materialCache;
			String baseDir = file.GetDirectory();
			for (unsigned int i = 0; i < meshCount; ++i)
			{
				const String& matName = materials[meshes[i].material];
				const MTLParser::Material* mtlMat = materialParser.GetMaterial(matName);
				if (!mtlMat)
				{
					NazaraWarning("MTL has no material \"" + matName + '"');
					continue;
				}

				auto it = materialCache.find(matName);
				if (it == materialCache.end())
				{
					ParameterList data;

					data.SetParameter(MaterialData::CustomDefined);

					UInt8 alphaValue = static_cast<UInt8>(mtlMat->alpha*255.f);

					Color ambientColor(mtlMat->ambient);
					Color diffuseColor(mtlMat->diffuse);
					Color specularColor(mtlMat->specular);
					ambientColor.a = alphaValue;
					diffuseColor.a = alphaValue;
					specularColor.a = alphaValue;

					data.SetParameter(MaterialData::AmbientColor, ambientColor);
					data.SetParameter(MaterialData::DiffuseColor, diffuseColor);
					data.SetParameter(MaterialData::Shininess, mtlMat->shininess);
					data.SetParameter(MaterialData::SpecularColor, specularColor);

					if (!mtlMat->alphaMap.IsEmpty())
					{
						String fullPath = mtlMat->alphaMap;
						if (!Nz::File::IsAbsolute(fullPath))
							fullPath.Prepend(baseDir);

						data.SetParameter(MaterialData::AlphaTexturePath, fullPath);
					}

					if (!mtlMat->diffuseMap.IsEmpty())
					{
						String fullPath = mtlMat->diffuseMap;
						if (!Nz::File::IsAbsolute(fullPath))
							fullPath.Prepend(baseDir);

						data.SetParameter(MaterialData::DiffuseTexturePath, fullPath);
					}

					if (!mtlMat->specularMap.IsEmpty())
					{
						String fullPath = mtlMat->specularMap;
						if (!Nz::File::IsAbsolute(fullPath))
							fullPath.Prepend(baseDir);

						data.SetParameter(MaterialData::SpecularTexturePath, fullPath);
					}

					// If we either have an alpha value or an alpha map, let's configure the material for transparency
					if (alphaValue != 255 || !mtlMat->alphaMap.IsEmpty())
					{
						// Some default settings
						data.SetParameter(MaterialData::Blending, true);
						data.SetParameter(MaterialData::DepthWrite, true);
						data.SetParameter(MaterialData::DstBlend, static_cast<int>(BlendFunc_InvSrcAlpha));
						data.SetParameter(MaterialData::SrcBlend, static_cast<int>(BlendFunc_SrcAlpha));
					}

					it = materialCache.emplace(matName, std::move(data)).first;
				}

				mesh->SetMaterialData(meshes[i].material, it->second);
			}

			return true;
		}

		bool Load(Mesh* mesh, Stream& stream, const MeshParams& parameters)
		{
			int reservedVertexCount;
			if (!parameters.custom.GetIntegerParameter("NativeOBJLoader_VertexCount", &reservedVertexCount))
				reservedVertexCount = 100;

			OBJParser parser;
			if (!parser.Parse(stream, reservedVertexCount))
			{
				NazaraError("OBJ parser failed");
				return false;
			}

			mesh->CreateStatic();

			const String* materials = parser.GetMaterials();
			const Vector4f* positions = parser.GetPositions();
			const Vector3f* normals = parser.GetNormals();
			const Vector3f* texCoords = parser.GetTexCoords();

			const OBJParser::Mesh* meshes = parser.GetMeshes();
			unsigned int meshCount = parser.GetMeshCount();

			NazaraAssert(materials != nullptr && positions != nullptr && normals != nullptr &&
			             texCoords != nullptr && meshes != nullptr && meshCount > 0,
			             "Invalid OBJParser output");

			// Un conteneur temporaire pour contenir les indices de face avant triangulation
			std::vector<unsigned int> faceIndices(3); // Comme il y aura au moins trois sommets
			for (unsigned int i = 0; i < meshCount; ++i)
			{
				unsigned int faceCount = meshes[i].faces.size();
				if (faceCount == 0)
					continue;

				std::vector<unsigned int> indices;
				indices.reserve(faceCount*3); // Pire cas si les faces sont des triangles

				// Afin d'utiliser OBJParser::FaceVertex comme clé dans un unordered_map,
				// nous devons fournir un foncteur de hash ainsi qu'un foncteur de comparaison

				// Hash
				struct FaceVertexHasher
				{
					std::size_t operator()(const OBJParser::FaceVertex& o) const
					{
						std::size_t seed = 0;
						HashCombine(seed, o.normal);
						HashCombine(seed, o.position);
						HashCombine(seed, o.texCoord);

						return seed;
					}
				};

				// Comparaison
				struct FaceVertexComparator
				{
					bool operator()(const OBJParser::FaceVertex& lhs, const OBJParser::FaceVertex& rhs) const
					{
						return lhs.normal   == rhs.normal   &&
						       lhs.position == rhs.position &&
						       lhs.texCoord == rhs.texCoord;
					}
				};

				std::unordered_map<OBJParser::FaceVertex, unsigned int, FaceVertexHasher, FaceVertexComparator> vertices;
				vertices.reserve(meshes[i].vertices.size());

				unsigned int vertexCount = 0;
				for (unsigned int j = 0; j < faceCount; ++j)
				{
					unsigned int faceVertexCount = meshes[i].faces[j].vertexCount;
					faceIndices.resize(faceVertexCount);

					for (unsigned int k = 0; k < faceVertexCount; ++k)
					{
						const OBJParser::FaceVertex& vertex = meshes[i].vertices[meshes[i].faces[j].firstVertex + k];

						auto it = vertices.find(vertex);
						if (it == vertices.end())
							it = vertices.emplace(vertex, vertexCount++).first;

						faceIndices[k] = it->second;
					}

					// Triangulation
					for (unsigned int k = 1; k < faceVertexCount-1; ++k)
					{
						indices.push_back(faceIndices[0]);
						indices.push_back(faceIndices[k]);
						indices.push_back(faceIndices[k+1]);
					}
				}

				// Création des buffers
				IndexBufferRef indexBuffer = IndexBuffer::New(vertexCount > std::numeric_limits<UInt16>::max(), indices.size(), parameters.storage, BufferUsage_Static);
				VertexBufferRef vertexBuffer = VertexBuffer::New(VertexDeclaration::Get(VertexLayout_XYZ_Normal_UV_Tangent), vertexCount, parameters.storage, BufferUsage_Static);

				// Remplissage des indices
				IndexMapper indexMapper(indexBuffer, BufferAccess_WriteOnly);
				for (unsigned int j = 0; j < indices.size(); ++j)
					indexMapper.Set(j, indices[j]);

				indexMapper.Unmap(); // Pour laisser les autres tâches affecter l'index buffer

				// Remplissage des vertices
				bool hasNormals = true;
				bool hasTexCoords = true;
				BufferMapper<VertexBuffer> vertexMapper(vertexBuffer, BufferAccess_WriteOnly);
				MeshVertex* meshVertices = static_cast<MeshVertex*>(vertexMapper.GetPointer());
				for (auto& vertexPair : vertices)
				{
					const OBJParser::FaceVertex& vertexIndices = vertexPair.first;
					unsigned int index = vertexPair.second;

					MeshVertex& vertex = meshVertices[index];

					const Vector4f& vec = positions[vertexIndices.position-1];
					vertex.position = Vector3f(parameters.matrix * vec);

					if (vertexIndices.normal > 0)
						vertex.normal = normals[vertexIndices.normal-1];
					else
						hasNormals = false;

					if (vertexIndices.texCoord > 0)
					{
						const Vector3f& uvw = texCoords[vertexIndices.texCoord-1];
						vertex.uv.Set(uvw.x, (parameters.flipUVs) ? 1.f - uvw.y : uvw.y); // Inversion des UVs si demandé
					}
					else
						hasTexCoords = false;
				}

				vertexMapper.Unmap();

				StaticMeshRef subMesh = StaticMesh::New(mesh);
				if (!subMesh->Create(vertexBuffer))
				{
					NazaraError("Failed to create StaticMesh");
					continue;
				}

				if (parameters.optimizeIndexBuffers)
					indexBuffer->Optimize();

				subMesh->GenerateAABB();
				subMesh->SetIndexBuffer(indexBuffer);
				subMesh->SetMaterialIndex(meshes[i].material);
				subMesh->SetPrimitiveMode(PrimitiveMode_TriangleList);

				// Ce que nous pouvons générer dépend des données à disposition (par exemple les tangentes nécessitent des coordonnées de texture)
				if (hasNormals && hasTexCoords)
					subMesh->GenerateTangents();
				else if (hasTexCoords)
					subMesh->GenerateNormalsAndTangents();
				else
					subMesh->GenerateNormals();

				mesh->AddSubMesh(meshes[i].name + '_' + materials[meshes[i].material], subMesh);
			}
			mesh->SetMaterialCount(parser.GetMaterialCount());

			if (parameters.center)
				mesh->Recenter();

			// On charge les matériaux si demandé
			String mtlLib = parser.GetMtlLib();
			if (!mtlLib.IsEmpty())
			{
				ErrorFlags flags(ErrorFlag_ThrowExceptionDisabled);
				ParseMTL(mesh, stream.GetDirectory() + mtlLib, materials, meshes, meshCount);
			}

			return true;
		}
	}

	namespace Loaders
	{
		void RegisterOBJLoader()
		{
			MeshLoader::RegisterLoader(IsSupported, Check, Load);
		}

		void UnregisterOBJLoader()
		{
			MeshLoader::UnregisterLoader(IsSupported, Check, Load);
		}
	}
}
