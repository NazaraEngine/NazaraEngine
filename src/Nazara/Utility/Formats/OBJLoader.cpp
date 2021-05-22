// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Formats/OBJLoader.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Utility/IndexMapper.hpp>
#include <Nazara/Utility/MaterialData.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/VertexMapper.hpp>
#include <Nazara/Utility/Formats/MTLParser.hpp>
#include <Nazara/Utility/Formats/OBJParser.hpp>
#include <limits>
#include <memory>
#include <unordered_map>
#include <Nazara/Utility/Debug.hpp>

///TODO: N'avoir qu'un seul VertexBuffer communs à tous les meshes

namespace Nz
{
	namespace
	{
		bool IsSupported(const std::string& extension)
		{
			return (extension == "obj");
		}

		Ternary Check(Stream& stream, const MeshParams& parameters)
		{
			NazaraUnused(stream);

			bool skip;
			if (parameters.custom.GetBooleanParameter("SkipNativeOBJLoader", &skip) && skip)
				return Ternary::False;

			OBJParser parser;
			if (!parser.Check(stream))
				return Ternary::False;

			return Ternary::Unknown;
		}

		bool ParseMTL(Mesh* mesh, const std::filesystem::path& filePath, const std::string* materials, const OBJParser::Mesh* meshes, std::size_t meshCount)
		{
			File file(filePath);
			if (!file.Open(OpenMode_ReadOnly | OpenMode_Text))
			{
				NazaraError("Failed to open MTL file (" + file.GetPath().generic_u8string() + ')');
				return false;
			}

			MTLParser materialParser;
			if (!materialParser.Parse(file))
			{
				NazaraError("MTL parser failed");
				return false;
			}

			std::unordered_map<std::string, ParameterList> materialCache;
			std::filesystem::path baseDir = file.GetDirectory();
			for (std::size_t i = 0; i < meshCount; ++i)
			{
				const std::string& matName = materials[meshes[i].material];
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

					if (!mtlMat->alphaMap.empty())
					{
						std::filesystem::path fullPath = mtlMat->alphaMap;
						if (!fullPath.is_absolute())
							fullPath = baseDir / fullPath;

						data.SetParameter(MaterialData::AlphaTexturePath, fullPath.generic_u8string());
					}

					if (!mtlMat->diffuseMap.empty())
					{
						std::filesystem::path fullPath = mtlMat->diffuseMap;
						if (!fullPath.is_absolute())
							fullPath = baseDir / fullPath;

						data.SetParameter(MaterialData::DiffuseTexturePath, fullPath.generic_u8string());
					}

					if (!mtlMat->emissiveMap.empty())
					{
						std::filesystem::path fullPath = mtlMat->emissiveMap;
						if (!fullPath.is_absolute())
							fullPath = baseDir / fullPath;

						data.SetParameter(MaterialData::EmissiveTexturePath, fullPath.generic_u8string());
					}

					if (!mtlMat->normalMap.empty())
					{
						std::filesystem::path fullPath = mtlMat->normalMap;
						if (!fullPath.is_absolute())
							fullPath = baseDir / fullPath;

						data.SetParameter(MaterialData::NormalTexturePath, fullPath.generic_u8string());
					}

					if (!mtlMat->specularMap.empty())
					{
						std::filesystem::path fullPath = mtlMat->specularMap;
						if (!fullPath.is_absolute())
							fullPath = baseDir / fullPath;

						data.SetParameter(MaterialData::SpecularTexturePath, fullPath.generic_u8string());
					}

					// If we either have an alpha value or an alpha map, let's configure the material for transparency
					if (alphaValue != 255 || !mtlMat->alphaMap.empty())
					{
						// Some default settings
						data.SetParameter(MaterialData::Blending, true);
						data.SetParameter(MaterialData::DepthWrite, true);
						data.SetParameter(MaterialData::BlendDstAlpha, static_cast<long long>(BlendFunc::Zero));
						data.SetParameter(MaterialData::BlendDstColor, static_cast<long long>(BlendFunc::InvSrcAlpha));
						data.SetParameter(MaterialData::BlendModeAlpha, static_cast<long long>(BlendEquation::Add));
						data.SetParameter(MaterialData::BlendModeColor, static_cast<long long>(BlendEquation::Add));
						data.SetParameter(MaterialData::BlendSrcAlpha, static_cast<long long>(BlendFunc::One));
						data.SetParameter(MaterialData::BlendSrcColor, static_cast<long long>(BlendFunc::SrcAlpha));
					}

					it = materialCache.emplace(matName, std::move(data)).first;
				}

				mesh->SetMaterialData(meshes[i].material, it->second);
			}

			return true;
		}

		MeshRef Load(Stream& stream, const MeshParams& parameters)
		{
			long long reservedVertexCount;
			if (!parameters.custom.GetIntegerParameter("NativeOBJLoader_VertexCount", &reservedVertexCount))
				reservedVertexCount = 100;

			OBJParser parser;
			if (!parser.Parse(stream, reservedVertexCount))
			{
				NazaraError("OBJ parser failed");
				return nullptr;
			}

			MeshRef mesh = Mesh::New();
			mesh->CreateStatic();

			const std::string* materials = parser.GetMaterials();
			const Vector4f* positions = parser.GetPositions();
			const Vector3f* normals = parser.GetNormals();
			const Vector3f* texCoords = parser.GetTexCoords();

			const OBJParser::Mesh* meshes = parser.GetMeshes();
			std::size_t meshCount = parser.GetMeshCount();

			NazaraAssert(materials != nullptr && positions != nullptr && normals != nullptr &&
			             texCoords != nullptr && meshes != nullptr && meshCount > 0,
			             "Invalid OBJParser output");

			// Un conteneur temporaire pour contenir les indices de face avant triangulation
			std::vector<std::size_t> faceIndices(3); // Comme il y aura au moins trois sommets
			for (std::size_t i = 0; i < meshCount; ++i)
			{
				std::size_t faceCount = meshes[i].faces.size();
				if (faceCount == 0)
					continue;

				std::vector<std::size_t> indices;
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
					std::size_t faceVertexCount = meshes[i].faces[j].vertexCount;
					faceIndices.resize(faceVertexCount);

					for (std::size_t k = 0; k < faceVertexCount; ++k)
					{
						const OBJParser::FaceVertex& vertex = meshes[i].vertices[meshes[i].faces[j].firstVertex + k];

						auto it = vertices.find(vertex);
						if (it == vertices.end())
							it = vertices.emplace(vertex, vertexCount++).first;

						faceIndices[k] = it->second;
					}

					// Triangulation
					for (std::size_t k = 1; k < faceVertexCount-1; ++k)
					{
						indices.push_back(faceIndices[0]);
						indices.push_back(faceIndices[k]);
						indices.push_back(faceIndices[k+1]);
					}
				}

				// Création des buffers
				IndexBufferRef indexBuffer = IndexBuffer::New(vertexCount > std::numeric_limits<UInt16>::max(), std::size_t(indices.size()), parameters.storage, parameters.indexBufferFlags);
				VertexBufferRef vertexBuffer = VertexBuffer::New(parameters.vertexDeclaration, std::size_t(vertexCount), parameters.storage, parameters.vertexBufferFlags);

				// Remplissage des indices
				IndexMapper indexMapper(indexBuffer, BufferAccess_WriteOnly);
				for (std::size_t j = 0; j < indices.size(); ++j)
					indexMapper.Set(j, UInt32(indices[j]));

				indexMapper.Unmap(); // Pour laisser les autres tâches affecter l'index buffer

				if (parameters.optimizeIndexBuffers)
					indexBuffer->Optimize();

				// Remplissage des vertices

				// Make sure the normal matrix won't rescale our normals
				Nz::Matrix4f normalMatrix = parameters.matrix;
				if (normalMatrix.HasScale())
					normalMatrix.ApplyScale(1.f / normalMatrix.GetScale());

				bool hasNormals = true;
				bool hasTexCoords = true;

				VertexMapper vertexMapper(vertexBuffer, BufferAccess_DiscardAndWrite);

				auto normalPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent_Normal);
				auto posPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent_Position);
				auto uvPtr = vertexMapper.GetComponentPtr<Vector2f>(VertexComponent_TexCoord);

				if (!normalPtr)
					hasNormals = false;

				if (!uvPtr)
					hasTexCoords = false;

				for (auto& vertexPair : vertices)
				{
					const OBJParser::FaceVertex& vertexIndices = vertexPair.first;
					unsigned int index = vertexPair.second;

					const Vector4f& vec = positions[vertexIndices.position-1];
					posPtr[index] = Vector3f(parameters.matrix * vec);

					if (hasNormals)
					{
						if (vertexIndices.normal > 0)
							normalPtr[index] = normalMatrix.Transform(normals[vertexIndices.normal - 1], 0.f);
						else
							hasNormals = false;
					}

					if (hasTexCoords)
					{
						if (vertexIndices.texCoord > 0)
						{
							Vector2f uv = Vector2f(texCoords[vertexIndices.texCoord - 1]);
							uv.y = 1.f - uv.y; //< OBJ model texcoords seems to majority start from bottom left

							uvPtr[index] = Vector2f(parameters.texCoordOffset + uv * parameters.texCoordScale);
						}
						else
							hasTexCoords = false;
					}
				}

				vertexMapper.Unmap();

				StaticMeshRef subMesh = StaticMesh::New(vertexBuffer, indexBuffer);
				subMesh->GenerateAABB();
				subMesh->SetMaterialIndex(meshes[i].material);

				// Ce que nous pouvons générer dépend des données à disposition (par exemple les tangentes nécessitent des coordonnées de texture)
				if (hasNormals && hasTexCoords)
					subMesh->GenerateTangents();
				else if (hasTexCoords)
					subMesh->GenerateNormalsAndTangents();
				else if (normalPtr)
					subMesh->GenerateNormals();

				mesh->AddSubMesh(meshes[i].name + '_' + materials[meshes[i].material], subMesh);
			}
			mesh->SetMaterialCount(parser.GetMaterialCount());

			if (parameters.center)
				mesh->Recenter();

			// On charge les matériaux si demandé
			std::filesystem::path mtlLib = parser.GetMtlLib();
			if (!mtlLib.empty())
			{
				ErrorFlags flags(ErrorFlag_ThrowExceptionDisabled);
				ParseMTL(mesh, stream.GetDirectory() / mtlLib, materials, meshes, meshCount);
			}

			return mesh;
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
