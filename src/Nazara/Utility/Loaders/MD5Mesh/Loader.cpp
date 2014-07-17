// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Loaders/MD5Mesh.hpp>
#include <Nazara/Utility/Loaders/MD5Mesh/Parser.hpp>
#include <Nazara/Utility/IndexIterator.hpp>
#include <Nazara/Utility/IndexMapper.hpp>
#include <Nazara/Utility/SkeletalMesh.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace
{
	bool IsSupported(const NzString& extension)
	{
		return (extension == "md5mesh");
	}

	nzTernary Check(NzInputStream& stream, const NzMeshParams& parameters)
	{
		NazaraUnused(parameters);

		NzMD5MeshParser parser(stream);
		return parser.Check();
	}

	bool Load(NzMesh* mesh, NzInputStream& stream, const NzMeshParams& parameters)
	{
		NzMD5MeshParser parser(stream);
		if (!parser.Parse())
		{
			NazaraError("MD5Mesh parser failed");
			return false;
		}

		// Pour que le squelette soit correctement aligné, il faut appliquer un quaternion "de correction" aux joints à la base du squelette
		NzQuaternionf rotationQuat = NzEulerAnglesf(-90.f, 180.f, 0.f);
		NzString baseDir = stream.GetDirectory();

		// Le hellknight de Doom 3 fait ~120 unités, et il est dit qu'il fait trois mètres
		// Nous réduisons donc la taille générale des fichiers MD5 de 1/40
		NzVector3f scale(parameters.scale/40.f);

		const NzMD5MeshParser::Joint* joints = parser.GetJoints();
		const NzMD5MeshParser::Mesh* meshes = parser.GetMeshes();
		unsigned int jointCount = parser.GetJointCount();
		unsigned int meshCount = parser.GetMeshCount();

		if (parameters.animated)
		{
			mesh->CreateSkeletal(jointCount);

			NzSkeleton* skeleton = mesh->GetSkeleton();
			for (unsigned int i = 0; i < jointCount; ++i)
			{
				NzJoint* joint = skeleton->GetJoint(i);

				int parent = joints[i].parent;
				if (parent >= 0)
					joint->SetParent(skeleton->GetJoint(parent));

				joint->SetName(joints[i].name);

				NzMatrix4f bindMatrix;

				if (parent >= 0)
					bindMatrix.MakeTransform(joints[i].bindPos, joints[i].bindOrient);
				else
					bindMatrix.MakeTransform(rotationQuat * joints[i].bindPos, rotationQuat * joints[i].bindOrient);

				joint->SetInverseBindMatrix(bindMatrix.InverseAffine());
			}

			mesh->SetMaterialCount(meshCount);
			for (unsigned int i = 0; i < meshCount; ++i)
			{
				const NzMD5MeshParser::Mesh& md5Mesh = meshes[i];

				unsigned int indexCount = md5Mesh.triangles.size()*3;
				unsigned int vertexCount = md5Mesh.vertices.size();

				bool largeIndices = (vertexCount > std::numeric_limits<nzUInt16>::max());

				std::unique_ptr<NzIndexBuffer> indexBuffer(new NzIndexBuffer(largeIndices, indexCount, parameters.storage));
				indexBuffer->SetPersistent(false);

				std::unique_ptr<NzVertexBuffer> vertexBuffer(new NzVertexBuffer(NzVertexDeclaration::Get(nzVertexLayout_XYZ_Normal_UV_Tangent_Skinning), vertexCount, parameters.storage, nzBufferUsage_Static));
				vertexBuffer->SetPersistent(false);

				// Index buffer
				NzIndexMapper indexMapper(indexBuffer.get(), nzBufferAccess_DiscardAndWrite);

				// Le format définit un set de triangles nous permettant de retrouver facilement les indices
				// Cependant les sommets des triangles ne sont pas spécifiés dans le même ordre que ceux du moteur
				// (On parle ici de winding)
				unsigned int index = 0;
				for (const NzMD5MeshParser::Triangle& triangle : md5Mesh.triangles)
				{
					// On les respécifie dans le bon ordre (inversion du winding)
					indexMapper.Set(index++, triangle.x);
					indexMapper.Set(index++, triangle.z);
					indexMapper.Set(index++, triangle.y);
				}

				indexMapper.Unmap();

				// Vertex buffer
				struct Weight
				{
					float bias;
					unsigned int jointIndex;
				};

				std::vector<Weight> tempWeights;

				NzBufferMapper<NzVertexBuffer> vertexMapper(vertexBuffer.get(), nzBufferAccess_WriteOnly);
				NzSkeletalMeshVertex* vertices = static_cast<NzSkeletalMeshVertex*>(vertexMapper.GetPointer());
				for (const NzMD5MeshParser::Vertex& vertex : md5Mesh.vertices)
				{
					// Skinning MD5 (Formule d'Id Tech)
					NzVector3f finalPos(NzVector3f::Zero());

					// On stocke tous les poids dans le tableau temporaire en même temps qu'on calcule la position finale du sommet.
					tempWeights.resize(vertex.weightCount);
					for (unsigned int j = 0; j < vertex.weightCount; ++j)
					{
						const NzMD5MeshParser::Weight& weight = md5Mesh.weights[vertex.startWeight + j];
						const NzMD5MeshParser::Joint& joint = joints[weight.joint];

						finalPos += (joint.bindPos + joint.bindOrient*weight.pos) * weight.bias;

						// Avant d'ajouter les poids, il faut s'assurer qu'il n'y en ait pas plus que le maximum supporté
						// et dans le cas contraire, garder les poids les plus importants et les renormaliser
						tempWeights[j] = {weight.bias, weight.joint};
					}

					// Avons nous plus de poids que le moteur ne peut en supporter ?
					unsigned int weightCount = vertex.weightCount;
					if (weightCount > NAZARA_UTILITY_SKINNING_MAX_WEIGHTS)
					{
						// Pour augmenter la qualité du skinning tout en ne gardant que X poids, on ne garde que les poids
						// les plus importants, ayant le plus d'impact sur le sommet final
						std::sort(tempWeights.begin(), tempWeights.end(), [] (const Weight& a, const Weight& b) -> bool {
							return a.bias > b.bias;
						});

						// Sans oublier bien sûr de renormaliser les poids (que leur somme soit 1)
						float weightSum = 0.f;
						for (unsigned int j = 0; j < NAZARA_UTILITY_SKINNING_MAX_WEIGHTS; ++j)
							weightSum += tempWeights[j].bias;

						for (unsigned int j = 0; j < NAZARA_UTILITY_SKINNING_MAX_WEIGHTS; ++j)
							tempWeights[j].bias /= weightSum;

						weightCount = NAZARA_UTILITY_SKINNING_MAX_WEIGHTS;
					}

					vertices->weightCount = weightCount;
					for (unsigned int j = 0; j < NAZARA_UTILITY_SKINNING_MAX_WEIGHTS; ++j)
					{
						if (j < weightCount)
						{
							// On donne une valeur aux poids présents
							vertices->weights[j] = tempWeights[j].bias;
							vertices->jointIndexes[j] = tempWeights[j].jointIndex;
						}
						else
						{
							// Et un poids de 0 sur le joint 0 pour les autres (nécessaire pour le GPU Skinning)
							// La raison est que le GPU ne tiendra pas compte du nombre de poids pour des raisons de performances.
							vertices->weights[j] = 0.f;
							vertices->jointIndexes[j] = 0;
						}
					}

					vertices->position = finalPos;

					// Le format MD5 spécifie ses UV avec l'origine en bas à gauche, contrairement au moteur
					// dont l'origine est en haut à gauche, nous inversons donc la valeur en Y.
					vertices->uv.Set(vertex.uv.x, 1.f - vertex.uv.y);
					vertices++;
				}

				vertexMapper.Unmap();

				// Material
				mesh->SetMaterial(i, baseDir + md5Mesh.shader);

				// Submesh
				std::unique_ptr<NzSkeletalMesh> subMesh(new NzSkeletalMesh(mesh));
				subMesh->Create(vertexBuffer.get());
				vertexBuffer.release();

				if (parameters.optimizeIndexBuffers)
					indexBuffer->Optimize();

				subMesh->SetIndexBuffer(indexBuffer.get());
				indexBuffer.release();

				subMesh->GenerateNormalsAndTangents();
				subMesh->SetMaterialIndex(i);
				subMesh->SetPrimitiveMode(nzPrimitiveMode_TriangleList);

				mesh->AddSubMesh(subMesh.get());
				subMesh.release();

				// Animation
				// Il est peut-être éventuellement possible que la probabilité que l'animation ait le même nom soit non-nulle.
				NzString path = stream.GetPath();
				if (!path.IsEmpty())
				{
					path.Replace(".md5mesh", ".md5anim", -8, NzString::CaseInsensitive);
					if (NzFile::Exists(path))
						mesh->SetAnimation(path);
				}
			}
		}
		else
		{
			if (!mesh->CreateStatic()) // Ne devrait jamais échouer
			{
				NazaraInternalError("Failed to create mesh");
				return false;
			}

			mesh->SetMaterialCount(meshCount);
			for (unsigned int i = 0; i < meshCount; ++i)
			{
				const NzMD5MeshParser::Mesh& md5Mesh = meshes[i];
				unsigned int indexCount = md5Mesh.triangles.size()*3;
				unsigned int vertexCount = md5Mesh.vertices.size();

				// Index buffer
				bool largeIndices = (vertexCount > std::numeric_limits<nzUInt16>::max());

				std::unique_ptr<NzIndexBuffer> indexBuffer(new NzIndexBuffer(largeIndices, indexCount, parameters.storage));
				indexBuffer->SetPersistent(false);

				NzIndexMapper indexMapper(indexBuffer.get(), nzBufferAccess_DiscardAndWrite);
				NzIndexIterator index = indexMapper.begin();

				for (const NzMD5MeshParser::Triangle& triangle : md5Mesh.triangles)
				{
					// On les respécifie dans le bon ordre
					*index++ = triangle.x;
					*index++ = triangle.z;
					*index++ = triangle.y;
				}
				indexMapper.Unmap();

				// Vertex buffer
				std::unique_ptr<NzVertexBuffer> vertexBuffer(new NzVertexBuffer(NzVertexDeclaration::Get(nzVertexLayout_XYZ_Normal_UV_Tangent), vertexCount, parameters.storage));
				NzBufferMapper<NzVertexBuffer> vertexMapper(vertexBuffer.get(), nzBufferAccess_WriteOnly);

				NzMeshVertex* vertex = reinterpret_cast<NzMeshVertex*>(vertexMapper.GetPointer());
				for (const NzMD5MeshParser::Vertex& md5Vertex : md5Mesh.vertices)
				{
					// Skinning MD5 (Formule d'Id Tech)
					NzVector3f finalPos(NzVector3f::Zero());
					for (unsigned int j = 0; j < md5Vertex.weightCount; ++j)
					{
						const NzMD5MeshParser::Weight& weight = md5Mesh.weights[md5Vertex.startWeight + j];
						const NzMD5MeshParser::Joint& joint = joints[weight.joint];

						finalPos += (joint.bindPos + joint.bindOrient*weight.pos) * weight.bias;
					}

					// On retourne le modèle dans le bon sens
					vertex->position = scale * (rotationQuat * finalPos);
					vertex->uv.Set(md5Vertex.uv.x, 1.f - md5Vertex.uv.y);
					vertex++;
				}

				vertexMapper.Unmap();

				// Submesh
				std::unique_ptr<NzStaticMesh> subMesh(new NzStaticMesh(mesh));
				subMesh->Create(vertexBuffer.get());

				vertexBuffer->SetPersistent(false);
				vertexBuffer.release();

				if (parameters.optimizeIndexBuffers)
					indexBuffer->Optimize();

				subMesh->SetIndexBuffer(indexBuffer.get());
				indexBuffer.release();

				// Material
				mesh->SetMaterial(i, baseDir + md5Mesh.shader);

				subMesh->GenerateAABB();
				subMesh->GenerateNormalsAndTangents();
				subMesh->SetMaterialIndex(i);

				if (parameters.center)
					subMesh->Center();

				mesh->AddSubMesh(subMesh.get());
				subMesh.release();
			}
		}

		return true;
	}
}

void NzLoaders_MD5Mesh_Register()
{
	NzMeshLoader::RegisterLoader(IsSupported, Check, Load);
}

void NzLoaders_MD5Mesh_Unregister()
{
	NzMeshLoader::UnregisterLoader(IsSupported, Check, Load);
}
