// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
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

// TODO: Use only one index buffer / vertex buffer for all submeshes

namespace Nz
{
	namespace
	{
		bool IsOBJSupported(std::string_view extension)
		{
			return (extension == ".obj");
		}

		bool ParseMTL(Mesh& mesh, const std::filesystem::path& filePath, const std::string* materials, const OBJParser::Mesh* meshes, std::size_t meshCount)
		{
			File file(filePath);
			if (!file.Open(OpenMode::Read | OpenMode::Text))
			{
				NazaraErrorFmt("failed to open MTL file ({0})", file.GetPath());
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
					NazaraWarningFmt("MTL has no material \"{0}\"", matName);
					continue;
				}

				auto it = materialCache.find(matName);
				if (it == materialCache.end())
				{
					ParameterList data;

					float alphaValue = mtlMat->alpha;

					Color ambientColor(mtlMat->ambient);
					Color baseColor(mtlMat->diffuse);
					Color specularColor(mtlMat->specular);
					ambientColor.a = alphaValue;
					baseColor.a = alphaValue;
					specularColor.a = alphaValue;

					data.SetParameter(MaterialData::Type, "Phong");
					data.SetParameter(MaterialData::AmbientColor, ambientColor);
					data.SetParameter(MaterialData::BaseColor, baseColor);
					data.SetParameter(MaterialData::Shininess, mtlMat->shininess);
					data.SetParameter(MaterialData::SpecularColor, specularColor);

					if (!mtlMat->alphaMap.empty())
					{
						std::filesystem::path fullPath = mtlMat->alphaMap;
						if (!fullPath.is_absolute())
							fullPath = baseDir / fullPath;

						data.SetParameter(MaterialData::AlphaTexturePath, PathToString(fullPath));
					}

					if (!mtlMat->diffuseMap.empty())
					{
						std::filesystem::path fullPath = mtlMat->diffuseMap;
						if (!fullPath.is_absolute())
							fullPath = baseDir / fullPath;

						data.SetParameter(MaterialData::BaseColorTexturePath, PathToString(fullPath));
					}

					if (!mtlMat->emissiveMap.empty())
					{
						std::filesystem::path fullPath = mtlMat->emissiveMap;
						if (!fullPath.is_absolute())
							fullPath = baseDir / fullPath;

						data.SetParameter(MaterialData::EmissiveTexturePath, PathToString(fullPath));
					}

					if (!mtlMat->normalMap.empty())
					{
						std::filesystem::path fullPath = mtlMat->normalMap;
						if (!fullPath.is_absolute())
							fullPath = baseDir / fullPath;

						data.SetParameter(MaterialData::NormalTexturePath, PathToString(fullPath));
					}

					if (!mtlMat->specularMap.empty())
					{
						std::filesystem::path fullPath = mtlMat->specularMap;
						if (!fullPath.is_absolute())
							fullPath = baseDir / fullPath;

						data.SetParameter(MaterialData::SpecularTexturePath, PathToString(fullPath));
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

				mesh.SetMaterialData(meshes[i].material, it->second);
			}

			return true;
		}

		Result<std::shared_ptr<Mesh>, ResourceLoadingError> LoadOBJ(Stream& stream, const MeshParams& parameters)
		{
			long long reservedVertexCount = parameters.custom.GetIntegerParameter("ReserveVertexCount").GetValueOr(1'000);

			OBJParser parser;

			UInt64 streamPos = stream.GetCursorPos();

			if (!parser.Check(stream))
				return Err(ResourceLoadingError::Unrecognized);

			stream.SetCursorPos(streamPos);

			if (!parser.Parse(stream, reservedVertexCount))
			{
				NazaraError("OBJ parser failed");
				return Err(ResourceLoadingError::DecodingError);
			}

			std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
			mesh->CreateStatic();

			const std::string* materials = parser.GetMaterials();
			const Vector4f* positions = parser.GetPositions();
			const Vector3f* normals = parser.GetNormals();
			const Vector3f* texCoords = parser.GetTexCoords();

			const OBJParser::Mesh* meshes = parser.GetMeshes();
			std::size_t meshCount = parser.GetMeshCount();

			NazaraAssert(materials != nullptr && positions != nullptr && normals != nullptr && texCoords != nullptr && meshes != nullptr && meshCount > 0,
			             "Invalid OBJParser output");

			// Triangulation temporary vector
			std::vector<UInt32> faceIndices;
			for (std::size_t i = 0; i < meshCount; ++i)
			{
				std::size_t faceCount = meshes[i].faces.size();
				if (faceCount == 0)
					continue;

				std::vector<UInt32> indices;
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

				UInt32 vertexCount = 0;
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
				bool largeIndices = (vertexCount > std::numeric_limits<UInt16>::max());

				std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBuffer>((largeIndices) ? IndexType::U32 : IndexType::U16, SafeCast<UInt32>(indices.size()), parameters.indexBufferFlags, parameters.bufferFactory);
				std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>(parameters.vertexDeclaration, vertexCount, parameters.vertexBufferFlags, parameters.bufferFactory);

				// Remplissage des indices
				IndexMapper indexMapper(*indexBuffer);
				for (std::size_t j = 0; j < indices.size(); ++j)
					indexMapper.Set(j, indices[j]);

				indexMapper.Unmap(); // Pour laisser les autres tâches affecter l'index buffer

				if (parameters.optimizeIndexBuffers)
					indexBuffer->Optimize();

				// Remplissage des vertices

				bool hasNormals = true;
				bool hasTexCoords = true;

				VertexMapper vertexMapper(*vertexBuffer);

				auto normalPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Normal);
				auto posPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Position);
				auto uvPtr = vertexMapper.GetComponentPtr<Vector2f>(VertexComponent::TexCoord);

				if (!normalPtr)
					hasNormals = false;

				if (!uvPtr)
					hasTexCoords = false;

				for (auto& vertexPair : vertices)
				{
					const OBJParser::FaceVertex& vertexIndices = vertexPair.first;
					unsigned int index = vertexPair.second;

					if (posPtr)
					{
						const Vector4f& vec = positions[vertexIndices.position - 1];
						posPtr[index] = TransformPositionTRS(parameters.vertexOffset, parameters.vertexRotation, parameters.vertexScale, Vector3f(vec));
					}

					if (hasNormals)
					{
						if (vertexIndices.normal > 0)
							normalPtr[index] = TransformNormalTRS(parameters.vertexRotation, parameters.vertexScale, normals[vertexIndices.normal - 1]);
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

				// Official .obj files have no vertex color, fill it with white
				if (auto colorPtr = vertexMapper.GetComponentPtr<Color>(VertexComponent::Color))
				{
					for (UInt32 j = 0; j < vertexCount; ++j)
						colorPtr[j] = Color::White();
				}

				vertexMapper.Unmap();

				std::shared_ptr<StaticMesh> subMesh = std::make_shared<StaticMesh>(std::move(vertexBuffer), indexBuffer);
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
				ErrorFlags errFlags({}, ~ErrorMode::ThrowException);
				ParseMTL(*mesh, stream.GetDirectory() / mtlLib, materials, meshes, meshCount);
			}

			return mesh;
		}
	}

	namespace Loaders
	{
		MeshLoader::Entry GetMeshLoader_OBJ()
		{
			MeshLoader::Entry loader;
			loader.extensionSupport = IsOBJSupported;
			loader.streamLoader = LoadOBJ;
			loader.parameterFilter = [](const MeshParams& parameters)
			{
				if (auto result = parameters.custom.GetBooleanParameter("SkipBuiltinOBJLoader"); result.GetValueOr(false))
					return false;

				return true;
			};

			return loader;
		}
	}
}
