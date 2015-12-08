// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Formats/OBJLoader.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/Model.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/IndexMapper.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/Formats/MTLParser.hpp>
#include <Nazara/Utility/Formats/OBJParser.hpp>
#include <limits>
#include <memory>
#include <unordered_map>
#include <Nazara/Graphics/Debug.hpp>

///TODO: N'avoir qu'un seul VertexBuffer communs à tous les meshes

namespace Nz
{
	namespace
	{
		bool IsSupported(const String& extension)
		{
			return (extension == "obj");
		}

		Ternary Check(Stream& stream, const ModelParameters& parameters)
		{
			NazaraUnused(stream);
			NazaraUnused(parameters);

			return Ternary_Unknown;
		}

		bool LoadMaterials(Model* model, const String& filePath, const MaterialParams& parameters, const String* materials, const OBJParser::Mesh* meshes, unsigned int meshCount)
		{
			File file(filePath);
			if (!file.Open(OpenMode_ReadOnly | OpenMode_Text))
			{
				NazaraError("Failed to open MTL file (" + file.GetPath() + ')');
				return false;
			}

			MTLParser materialParser(file);
			if (!materialParser.Parse())
			{
				NazaraError("MTL parser failed");
				return false;
			}

			std::unordered_map<String, MaterialRef> materialCache;
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
					MaterialRef material = Material::New();
					material->SetShader(parameters.shaderName);

					UInt8 alphaValue = static_cast<UInt8>(mtlMat->alpha*255.f);

					Color ambientColor(mtlMat->ambient);
					Color diffuseColor(mtlMat->diffuse);
					Color specularColor(mtlMat->specular);
					ambientColor.a = alphaValue;
					diffuseColor.a = alphaValue;
					specularColor.a = alphaValue;

					material->SetAmbientColor(ambientColor);
					material->SetDiffuseColor(diffuseColor);
					material->SetSpecularColor(specularColor);
					material->SetShininess(mtlMat->shininess);

					bool isTranslucent = (alphaValue != 255);

					if (parameters.loadAlphaMap && !mtlMat->alphaMap.IsEmpty())
					{
						if (material->SetAlphaMap(baseDir + mtlMat->alphaMap))
							isTranslucent = true; // Une alpha map indique de la transparence
						else
							NazaraWarning("Failed to load alpha map (" + mtlMat->alphaMap + ')');
					}

					if (parameters.loadDiffuseMap && !mtlMat->diffuseMap.IsEmpty())
					{
						if (!material->SetDiffuseMap(baseDir + mtlMat->diffuseMap))
							NazaraWarning("Failed to load diffuse map (" + mtlMat->diffuseMap + ')');
					}

					if (parameters.loadSpecularMap && !mtlMat->specularMap.IsEmpty())
					{
						if (!material->SetSpecularMap(baseDir + mtlMat->specularMap))
							NazaraWarning("Failed to load specular map (" + mtlMat->specularMap + ')');
					}

					// Si nous avons une alpha map ou des couleurs transparentes,
					// nous devons configurer le matériau pour accepter la transparence au mieux
					if (isTranslucent)
					{
						// On paramètre le matériau pour accepter la transparence au mieux
						material->Enable(RendererParameter_Blend, true);
						material->Enable(RendererParameter_DepthWrite, false);
						material->SetDstBlend(BlendFunc_InvSrcAlpha);
						material->SetSrcBlend(BlendFunc_SrcAlpha);
					}

					it = materialCache.emplace(matName, std::move(material)).first;
				}

				model->SetMaterial(meshes[i].material, it->second);
			}

			return true;
		}

		bool Load(Model* model, Stream& stream, const ModelParameters& parameters)
		{
			OBJParser parser(stream);
			if (!parser.Parse())
			{
				NazaraError("OBJ parser failed");
				return false;
			}

			MeshRef mesh = Mesh::New();
			if (!mesh->CreateStatic()) // Ne devrait jamais échouer
			{
				NazaraInternalError("Failed to create mesh");
				return false;
			}

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
						return lhs.normal == rhs.normal &&
							   lhs.position == rhs.position &&
							   lhs.texCoord == rhs.texCoord;
					}
				};

				std::unordered_map<OBJParser::FaceVertex, unsigned int, FaceVertexHasher, FaceVertexComparator> vertices;

				unsigned int vertexCount = 0;
				for (unsigned int j = 0; j < faceCount; ++j)
				{
					unsigned int faceVertexCount = meshes[i].faces[j].vertices.size();
					faceIndices.resize(faceVertexCount);

					for (unsigned int k = 0; k < faceVertexCount; ++k)
					{
						const OBJParser::FaceVertex& vertex = meshes[i].faces[j].vertices[k];

						auto it = vertices.find(vertex);
						if (it == vertices.end())
							it = vertices.emplace(vertex, vertexCount++).first;

						faceIndices[k] = it->second;
					}

					for (unsigned int k = 1; k < faceVertexCount-1; ++k)
					{
						indices.push_back(faceIndices[0]);
						indices.push_back(faceIndices[k]);
						indices.push_back(faceIndices[k+1]);
					}
				}

				// Création des buffers
				IndexBufferRef indexBuffer = IndexBuffer::New(vertexCount > std::numeric_limits<UInt16>::max(), indices.size(), parameters.mesh.storage, BufferUsage_Static);
				VertexBufferRef vertexBuffer = VertexBuffer::New(VertexDeclaration::Get(VertexLayout_XYZ_Normal_UV_Tangent), vertexCount, parameters.mesh.storage, BufferUsage_Static);

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

					const Vector4f& vec = positions[vertexIndices.position];
					vertex.position.Set(vec.x, vec.y, vec.z);
					vertex.position *= parameters.mesh.scale/vec.w;

					if (vertexIndices.normal >= 0)
						vertex.normal = normals[vertexIndices.normal];
					else
						hasNormals = false;

					if (vertexIndices.texCoord >= 0)
					{
						const Vector3f& uvw = texCoords[vertexIndices.texCoord];
						vertex.uv.Set(uvw.x, (parameters.mesh.flipUVs) ? 1.f - uvw.y : uvw.y); // Inversion des UVs si demandé
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

				if (parameters.mesh.optimizeIndexBuffers)
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

			if (parameters.mesh.center)
				mesh->Recenter();

			model->SetMesh(mesh);

			// On charge les matériaux si demandé
			String mtlLib = parser.GetMtlLib();
			if (parameters.loadMaterials && !mtlLib.IsEmpty())
			{
				ErrorFlags flags(ErrorFlag_ThrowExceptionDisabled);
				LoadMaterials(model, stream.GetDirectory() + mtlLib, parameters.material, materials, meshes, meshCount);
			}

			return true;
		}
	}

	namespace Loaders
	{
		void RegisterOBJ()
		{
			ModelLoader::RegisterLoader(IsSupported, Check, Load);
		}

		void UnregisterOBJ()
		{
			ModelLoader::UnregisterLoader(IsSupported, Check, Load);
		}
	}
}
