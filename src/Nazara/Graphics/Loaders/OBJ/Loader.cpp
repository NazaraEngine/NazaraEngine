// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Loaders/OBJ.hpp>
#include <Nazara/Graphics/Loaders/OBJ/MTLParser.hpp>
#include <Nazara/Graphics/Loaders/OBJ/OBJParser.hpp>
#include <Nazara/Graphics/Model.hpp>
#include <Nazara/Renderer/Material.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/IndexMapper.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <limits>
#include <memory>
#include <unordered_map>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	bool IsSupported(const NzString& extension)
	{
		return (extension == "obj");
	}

	nzTernary Check(NzInputStream& stream, const NzModelParameters& parameters)
	{
		NazaraUnused(stream);
		NazaraUnused(parameters);

		return nzTernary_Unknown;
	}

	bool Load(NzModel* model, NzInputStream& stream, const NzModelParameters& parameters)
	{
		NzOBJParser parser(stream);
		if (!parser.Parse())
		{
			NazaraError("OBJ parser failed");
			return false;
		}

		std::unique_ptr<NzMesh> mesh(new NzMesh);
		mesh->SetPersistent(false);
		if (!mesh->CreateStatic()) // Ne devrait jamais échouer
		{
			NazaraInternalError("Failed to create mesh");
			return false;
		}

		const NzString* materials = parser.GetMaterials();
		const NzVector4f* positions = parser.GetPositions();
		const NzVector3f* normals = parser.GetNormals();
		const NzVector3f* texCoords = parser.GetTexCoords();

		std::vector<unsigned int> faceIndices;

		const NzOBJParser::Mesh* meshes = parser.GetMeshes();
		unsigned int meshCount = parser.GetMeshCount();
		for (unsigned int i = 0; i < meshCount; ++i)
		{
			unsigned int faceCount = meshes[i].faces.size();

			std::vector<unsigned int> indices;
			indices.reserve(faceCount*3); // Pire cas (si les faces sont des triangles)

			// Bien plus rapide qu'un vector (pour la recherche)
			std::unordered_map<int, std::unordered_map<int, std::unordered_map<int, unsigned int>>> vertices;

			unsigned int vertexCount = 0;
			for (unsigned int j = 0; j < faceCount; ++j)
			{
				unsigned int faceVertexCount = meshes[i].faces[j].vertices.size();
				faceIndices.resize(faceVertexCount);

				for (unsigned int k = 0; k < faceVertexCount; ++k)
				{
					const NzOBJParser::FaceVertex& vertex = meshes[i].faces[j].vertices[k];

					auto& map = vertices[vertex.texCoord][vertex.normal];
					auto it = map.find(vertex.position);
					if (it == map.end())
					{
						faceIndices[k] = vertexCount;
						map[vertex.position] = vertexCount++;
					}
					else
						faceIndices[k] = it->second;
				}

				for (unsigned int k = 1; k < faceVertexCount-1; ++k)
				{
					indices.push_back(faceIndices[0]);
					indices.push_back(faceIndices[k]);
					indices.push_back(faceIndices[k+1]);
				}
			}

			std::unique_ptr<NzIndexBuffer> indexBuffer(new NzIndexBuffer(indices.size(), vertexCount > std::numeric_limits<nzUInt16>::max(), parameters.mesh.storage, nzBufferUsage_Static));
			indexBuffer->SetPersistent(false);

			std::unique_ptr<NzVertexBuffer> vertexBuffer(new NzVertexBuffer(NzMesh::GetDeclaration(), vertexCount, parameters.mesh.storage, nzBufferUsage_Static));
			vertexBuffer->SetPersistent(false);

			// Remplissage des indices
			NzIndexMapper indexMapper(indexBuffer.get(), nzBufferAccess_WriteOnly);
			for (unsigned int j = 0; j < indices.size(); ++j)
				indexMapper.Set(j, indices[j]);

			indexMapper.Unmap();

			// Remplissage des vertices
			bool hasNormals = true;
			bool hasTexCoords = true;
			NzBufferMapper<NzVertexBuffer> vertexMapper(vertexBuffer.get(), nzBufferAccess_WriteOnly);
			NzMeshVertex* meshVertices = static_cast<NzMeshVertex*>(vertexMapper.GetPointer());
			for (auto uvIt : vertices)
			{
				for (auto normalIt : uvIt.second)
				{
					for (auto positionIt : normalIt.second)
					{
						NzMeshVertex& vertex = meshVertices[positionIt.second];

						const NzVector4f& vec = positions[positionIt.first];
						vertex.position.Set(vec.x/vec.w, vec.y/vec.w, vec.z/vec.w);

						int index;

						index = normalIt.first; // Normale
						if (index >= 0)
							vertex.normal = normals[index];
						else
							hasNormals = false;

						index = uvIt.first; // Coordonnées de texture
						if (index >= 0)
						{
							const NzVector3f& uvw = texCoords[index];
							vertex.uv.Set(uvw.x, uvw.y);
						}
						else
							hasTexCoords = false;
					}
				}
			}

			vertexMapper.Unmap();

			std::unique_ptr<NzStaticMesh> subMesh(new NzStaticMesh(mesh.get()));
			if (!subMesh->Create(vertexBuffer.get()))
			{
				NazaraError("Failed to create StaticMesh");
				continue;
			}
			vertexBuffer.release();

			if (parameters.mesh.optimizeIndexBuffers)
				indexBuffer->Optimize();

			subMesh->SetIndexBuffer(indexBuffer.get());
			indexBuffer.release();

			subMesh->GenerateAABB();
			subMesh->SetMaterialIndex(meshes[i].material);
			subMesh->SetPrimitiveMode(nzPrimitiveMode_TriangleList);

			if (hasNormals && hasTexCoords)
				subMesh->GenerateTangents();
			else if (hasTexCoords)
				subMesh->GenerateNormalsAndTangents();
			else
				subMesh->GenerateNormals();

			if (mesh->AddSubMesh(meshes[i].name + '_' + materials[meshes[i].material], subMesh.get()))
				subMesh.release();
			else
				NazaraError("Failed to add SubMesh to Mesh");
		}

		mesh->SetMaterialCount(parser.GetMaterialCount());

		model->SetMesh(mesh.get());
		mesh.release();

		// On charge les matériaux si demandé
		NzString mtlLib = parser.GetMtlLib();
		if (parameters.loadMaterials && !mtlLib.IsEmpty())
		{
			NzFile file(stream.GetDirectory() + mtlLib);
			if (file.Open(NzFile::ReadOnly | NzFile::Text))
			{
				NzMTLParser materialParser(file);
				if (materialParser.Parse())
				{
					NzString baseDir = file.GetDirectory();
					for (unsigned int i = 0; i < meshCount; ++i)
					{
						const NzString& matName = materials[meshes[i].material];
						const NzMTLParser::Material* mtlMat = materialParser.GetMaterial(matName);
						if (mtlMat)
						{
							std::unique_ptr<NzMaterial> material(new NzMaterial);
							material->SetPersistent(false);

							NzColor ambientColor(mtlMat->ambient);
							ambientColor.a = mtlMat->alpha;

							NzColor diffuseColor(mtlMat->diffuse);
							diffuseColor.a = mtlMat->alpha;

							NzColor specularColor(mtlMat->specular);
							specularColor.a = mtlMat->alpha;

							material->SetAmbientColor(ambientColor);
							material->SetDiffuseColor(diffuseColor);
							material->SetSpecularColor(specularColor);
							material->SetShininess(mtlMat->shininess);

							if (parameters.material.loadDiffuseMap && !mtlMat->diffuseMap.IsEmpty())
							{
								std::unique_ptr<NzTexture> diffuseMap(new NzTexture);
								diffuseMap->SetPersistent(false);

								if (diffuseMap->LoadFromFile(baseDir + mtlMat->diffuseMap))
								{
									material->SetDiffuseMap(diffuseMap.get());
									diffuseMap.release();
								}
								else
									NazaraWarning("Failed to load diffuse map (" + mtlMat->diffuseMap + ')');
							}

							if (parameters.material.loadSpecularMap && !mtlMat->specularMap.IsEmpty())
							{
								std::unique_ptr<NzTexture> specularMap(new NzTexture);
								specularMap->SetPersistent(false);

								if (specularMap->LoadFromFile(baseDir + mtlMat->specularMap))
								{
									material->SetSpecularMap(specularMap.get());
									specularMap.release();
								}
								else
									NazaraWarning("Failed to load specular map (" + mtlMat->diffuseMap + ')');
							}

							model->SetMaterial(meshes[i].material, material.get());
							material.release();
						}
						else
							NazaraWarning("MTL has no material \"" + matName + '"');
					}
				}
				else
					NazaraWarning("MTL parser failed");
			}
			else
				NazaraWarning("Failed to open MTL file (" + file.GetPath() + ')');
		}

		return true;
	}
}

void NzLoaders_OBJ_Register()
{
	NzModelLoader::RegisterLoader(IsSupported, Check, Load);
}

void NzLoaders_OBJ_Unregister()
{
	NzModelLoader::UnregisterLoader(IsSupported, Check, Load);
}
