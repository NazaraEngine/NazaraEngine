// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/Loaders/OBJ.hpp>
#include <Nazara/Core/Algorithm.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/Model.hpp>
#include <Nazara/Graphics/Loaders/OBJ/MTLParser.hpp>
#include <Nazara/Graphics/Loaders/OBJ/OBJParser.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/IndexMapper.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <limits>
#include <memory>
#include <unordered_map>
#include <Nazara/Graphics/Debug.hpp>

///TODO: N'avoir qu'un seul VertexBuffer communs à tous les meshes

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

	bool LoadMaterials(NzModel* model, const NzString& filePath, const NzMaterialParams& parameters, const NzString* materials, const NzOBJParser::Mesh* meshes, unsigned int meshCount)
	{
		NzFile file(filePath);
		if (!file.Open(NzFile::ReadOnly | NzFile::Text))
		{
			NazaraError("Failed to open MTL file (" + file.GetPath() + ')');
			return false;
		}

		NzMTLParser materialParser(file);
		if (!materialParser.Parse())
		{
			NazaraError("MTL parser failed");
			return false;
		}

		std::unordered_map<NzString, NzMaterialRef> materialCache;
		NzString baseDir = file.GetDirectory();
		for (unsigned int i = 0; i < meshCount; ++i)
		{
			const NzString& matName = materials[meshes[i].material];
			const NzMTLParser::Material* mtlMat = materialParser.GetMaterial(matName);
			if (!mtlMat)
			{
				NazaraWarning("MTL has no material \"" + matName + '"');
				continue;
			}

			auto it = materialCache.find(matName);
			if (it == materialCache.end())
			{
				NzMaterialRef material = NzMaterial::New();
				material->SetShader(parameters.shaderName);

				nzUInt8 alphaValue = static_cast<nzUInt8>(mtlMat->alpha*255.f);

				NzColor ambientColor(mtlMat->ambient);
				NzColor diffuseColor(mtlMat->diffuse);
				NzColor specularColor(mtlMat->specular);
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
					material->Enable(nzRendererParameter_Blend, true);
					material->Enable(nzRendererParameter_DepthWrite, false);
					material->SetDstBlend(nzBlendFunc_InvSrcAlpha);
					material->SetSrcBlend(nzBlendFunc_SrcAlpha);
				}

				it = materialCache.emplace(matName, std::move(material)).first;
			}

			model->SetMaterial(meshes[i].material, it->second);
		}
	}

	bool Load(NzModel* model, NzInputStream& stream, const NzModelParameters& parameters)
	{
		NzOBJParser parser(stream);
		if (!parser.Parse())
		{
			NazaraError("OBJ parser failed");
			return false;
		}

		NzMeshRef mesh = NzMesh::New();
		if (!mesh->CreateStatic()) // Ne devrait jamais échouer
		{
			NazaraInternalError("Failed to create mesh");
			return false;
		}

		const NzString* materials = parser.GetMaterials();
		const NzVector4f* positions = parser.GetPositions();
		const NzVector3f* normals = parser.GetNormals();
		const NzVector3f* texCoords = parser.GetTexCoords();

		const NzOBJParser::Mesh* meshes = parser.GetMeshes();
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
				std::size_t operator()(const NzOBJParser::FaceVertex& o) const
				{
					std::size_t seed = 0;
					NzHashCombine(seed, o.normal);
					NzHashCombine(seed, o.position);
					NzHashCombine(seed, o.texCoord);

					return seed;
				}
			};

			// Comparaison
			struct FaceVertexComparator
			{
				bool operator()(const NzOBJParser::FaceVertex& lhs, const NzOBJParser::FaceVertex& rhs) const
				{
					return lhs.normal == rhs.normal &&
					       lhs.position == rhs.position &&
					       lhs.texCoord == rhs.texCoord;
				}
			};

			std::unordered_map<NzOBJParser::FaceVertex, unsigned int, FaceVertexHasher, FaceVertexComparator> vertices;

			unsigned int vertexCount = 0;
			for (unsigned int j = 0; j < faceCount; ++j)
			{
				unsigned int faceVertexCount = meshes[i].faces[j].vertices.size();
				faceIndices.resize(faceVertexCount);

				for (unsigned int k = 0; k < faceVertexCount; ++k)
				{
					const NzOBJParser::FaceVertex& vertex = meshes[i].faces[j].vertices[k];

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
			NzIndexBufferRef indexBuffer = NzIndexBuffer::New(vertexCount > std::numeric_limits<nzUInt16>::max(), indices.size(), parameters.mesh.storage, nzBufferUsage_Static);
			NzVertexBufferRef vertexBuffer = NzVertexBuffer::New(NzVertexDeclaration::Get(nzVertexLayout_XYZ_Normal_UV_Tangent), vertexCount, parameters.mesh.storage, nzBufferUsage_Static);

			// Remplissage des indices
			NzIndexMapper indexMapper(indexBuffer, nzBufferAccess_WriteOnly);
			for (unsigned int j = 0; j < indices.size(); ++j)
				indexMapper.Set(j, indices[j]);

			indexMapper.Unmap(); // Pour laisser les autres tâches affecter l'index buffer

			// Remplissage des vertices
			bool hasNormals = true;
			bool hasTexCoords = true;
			NzBufferMapper<NzVertexBuffer> vertexMapper(vertexBuffer, nzBufferAccess_WriteOnly);
			NzMeshVertex* meshVertices = static_cast<NzMeshVertex*>(vertexMapper.GetPointer());
			for (auto& vertexPair : vertices)
			{
				const NzOBJParser::FaceVertex& vertexIndices = vertexPair.first;
				unsigned int index = vertexPair.second;

				NzMeshVertex& vertex = meshVertices[index];

				const NzVector4f& vec = positions[vertexIndices.position];
				vertex.position.Set(vec.x, vec.y, vec.z);
				vertex.position *= parameters.mesh.scale/vec.w;

				if (vertexIndices.normal >= 0)
					vertex.normal = normals[vertexIndices.normal];
				else
					hasNormals = false;

				if (vertexIndices.texCoord >= 0)
				{
					const NzVector3f& uvw = texCoords[vertexIndices.texCoord];
					vertex.uv.Set(uvw.x, (parameters.mesh.flipUVs) ? 1.f - uvw.y : uvw.y); // Inversion des UVs si demandé
				}
				else
					hasTexCoords = false;
			}

			vertexMapper.Unmap();

			NzStaticMeshRef subMesh = NzStaticMesh::New(mesh);
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
			subMesh->SetPrimitiveMode(nzPrimitiveMode_TriangleList);

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
		NzString mtlLib = parser.GetMtlLib();
		if (parameters.loadMaterials && !mtlLib.IsEmpty())
		{
			NzErrorFlags flags(nzErrorFlag_ThrowExceptionDisabled);
			LoadMaterials(model, stream.GetDirectory() + mtlLib, parameters.material, materials, meshes, meshCount);
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
