// Copyright (C) 2020 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Formats/MD5MeshLoader.hpp>
#include <Nazara/Utility/IndexIterator.hpp>
#include <Nazara/Utility/IndexMapper.hpp>
#include <Nazara/Utility/Joint.hpp>
#include <Nazara/Utility/MaterialData.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/SkeletalMesh.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/VertexMapper.hpp>
#include <Nazara/Utility/Formats/MD5MeshParser.hpp>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	namespace
	{
		bool IsSupported(const std::string_view& extension)
		{
			return (extension == "md5mesh");
		}

		Ternary Check(Stream& stream, const MeshParams& parameters)
		{
			bool skip;
			if (parameters.custom.GetBooleanParameter("SkipNativeMD5MeshLoader", &skip) && skip)
				return Ternary::False;

			MD5MeshParser parser(stream);
			return parser.Check();
		}

		std::shared_ptr<Mesh> Load(Stream& stream, const MeshParams& parameters)
		{
			MD5MeshParser parser(stream);
			if (!parser.Parse())
			{
				NazaraError("MD5Mesh parser failed");
				return nullptr;
			}

			// Pour que le squelette soit correctement aligné, il faut appliquer un quaternion "de correction" aux joints à la base du squelette
			Quaternionf rotationQuat = Quaternionf::RotationBetween(Vector3f::UnitX(), Vector3f::Forward()) *
			                           Quaternionf::RotationBetween(Vector3f::UnitZ(), Vector3f::Up());

			std::filesystem::path baseDir = stream.GetDirectory();

			// Le hellknight de Doom 3 fait ~120 unités, et il est dit qu'il fait trois mètres
			// Nous réduisons donc la taille générale des fichiers MD5 de 1/40
			Matrix4f matrix = Matrix4f::Transform(Nz::Vector3f::Zero(), rotationQuat, Vector3f(1.f / 40.f));
			matrix *= parameters.matrix;

			const MD5MeshParser::Joint* joints = parser.GetJoints();
			const MD5MeshParser::Mesh* meshes = parser.GetMeshes();
			UInt32 jointCount = parser.GetJointCount();
			UInt32 meshCount = parser.GetMeshCount();

			if (parameters.animated)
			{
				std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
				mesh->CreateSkeletal(jointCount);

				Skeleton* skeleton = mesh->GetSkeleton();
				for (UInt32 i = 0; i < jointCount; ++i)
				{
					Joint* joint = skeleton->GetJoint(i);

					int parent = joints[i].parent;
					if (parent >= 0)
						joint->SetParent(skeleton->GetJoint(parent));

					joint->SetName(joints[i].name);

					Matrix4f bindMatrix;

					if (parent >= 0)
						bindMatrix.MakeTransform(joints[i].bindPos, joints[i].bindOrient);
					else
						bindMatrix.MakeTransform(rotationQuat * joints[i].bindPos, rotationQuat * joints[i].bindOrient);

					joint->SetInverseBindMatrix(bindMatrix.InverseAffine());
				}

				mesh->SetMaterialCount(meshCount);
				for (UInt32 i = 0; i < meshCount; ++i)
				{
					const MD5MeshParser::Mesh& md5Mesh = meshes[i];

					std::size_t indexCount = md5Mesh.triangles.size()*3;
					std::size_t vertexCount = md5Mesh.vertices.size();

					bool largeIndices = (vertexCount > std::numeric_limits<UInt16>::max());

					std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBuffer>(largeIndices, UInt32(indexCount), parameters.storage, parameters.indexBufferFlags);
					std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>(VertexDeclaration::Get(VertexLayout::XYZ_Normal_UV_Tangent_Skinning), UInt32(vertexCount), parameters.storage, parameters.vertexBufferFlags);

					// Index buffer
					IndexMapper indexMapper(*indexBuffer, BufferAccess::DiscardAndWrite);

					// Le format définit un set de triangles nous permettant de retrouver facilement les indices
					// Cependant les sommets des triangles ne sont pas spécifiés dans le même ordre que ceux du moteur
					// (On parle ici de winding)
					UInt32 index = 0;
					for (const MD5MeshParser::Triangle& triangle : md5Mesh.triangles)
					{
						// On les respécifie dans le bon ordre (inversion du winding)
						indexMapper.Set(index++, triangle.x);
						indexMapper.Set(index++, triangle.z);
						indexMapper.Set(index++, triangle.y);
					}

					indexMapper.Unmap();

					if (parameters.optimizeIndexBuffers)
						indexBuffer->Optimize();

					// Vertex buffer
					struct Weight
					{
						float bias;
						unsigned int jointIndex;
					};

					std::vector<Weight> tempWeights;

					BufferMapper<VertexBuffer> vertexMapper(*vertexBuffer, BufferAccess::WriteOnly);
					SkeletalMeshVertex* vertices = static_cast<SkeletalMeshVertex*>(vertexMapper.GetPointer());

					for (const MD5MeshParser::Vertex& vertex : md5Mesh.vertices)
					{
						// Skinning MD5 (Formule d'Id Tech)
						Vector3f finalPos(Vector3f::Zero());

						// On stocke tous les poids dans le tableau temporaire en même temps qu'on calcule la position finale du sommet.
						tempWeights.resize(vertex.weightCount);
						for (unsigned int j = 0; j < vertex.weightCount; ++j)
						{
							const MD5MeshParser::Weight& weight = md5Mesh.weights[vertex.startWeight + j];
							const MD5MeshParser::Joint& joint = joints[weight.joint];

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
						vertices->uv.Set(parameters.texCoordOffset + vertex.uv * parameters.texCoordScale);
						vertices++;
					}

					vertexMapper.Unmap();

					// Material
					ParameterList matData;
					matData.SetParameter(MaterialData::FilePath, (baseDir / md5Mesh.shader).generic_u8string());

					mesh->SetMaterialData(i, std::move(matData));

					// Submesh
					std::shared_ptr<SkeletalMesh> subMesh = std::make_shared<SkeletalMesh>(vertexBuffer, indexBuffer);
					subMesh->GenerateNormalsAndTangents();
					subMesh->SetMaterialIndex(i);

					mesh->AddSubMesh(subMesh);

					// Animation
					// Il est peut-être éventuellement possible que la probabilité que l'animation ait le même nom soit non-nulle.
					std::filesystem::path path = stream.GetPath();
					if (!path.empty())
					{
						path.replace_extension(".md5anim");
						if (std::filesystem::exists(path))
							mesh->SetAnimation(path);
					}
				}

				return mesh;
			}
			else
			{
				std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
				if (!mesh->CreateStatic()) // Ne devrait jamais échouer
				{
					NazaraInternalError("Failed to create mesh");
					return nullptr;
				}

				mesh->SetMaterialCount(meshCount);
				for (UInt32 i = 0; i < meshCount; ++i)
				{
					const MD5MeshParser::Mesh& md5Mesh = meshes[i];
					std::size_t indexCount = md5Mesh.triangles.size()*3;
					std::size_t vertexCount = md5Mesh.vertices.size();

					// Index buffer
					bool largeIndices = (vertexCount > std::numeric_limits<UInt16>::max());

					std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBuffer>(largeIndices, UInt32(indexCount), parameters.storage, parameters.indexBufferFlags);

					IndexMapper indexMapper(*indexBuffer, BufferAccess::DiscardAndWrite);
					IndexIterator index = indexMapper.begin();

					for (const MD5MeshParser::Triangle& triangle : md5Mesh.triangles)
					{
						// On les respécifie dans le bon ordre
						*index++ = triangle.x;
						*index++ = triangle.z;
						*index++ = triangle.y;
					}
					indexMapper.Unmap();

					if (parameters.optimizeIndexBuffers)
						indexBuffer->Optimize();

					// Vertex buffer
					std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>(parameters.vertexDeclaration, UInt32(vertexCount), parameters.storage, parameters.vertexBufferFlags);

					VertexMapper vertexMapper(*vertexBuffer, BufferAccess::DiscardAndWrite);

					// Vertex positions
					if (auto posPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Position))
					{
						for (const MD5MeshParser::Vertex& md5Vertex : md5Mesh.vertices)
						{
							// Id Tech MD5 skinning
							Vector3f finalPos(Vector3f::Zero());
							for (unsigned int j = 0; j < md5Vertex.weightCount; ++j)
							{
								const MD5MeshParser::Weight& weight = md5Mesh.weights[md5Vertex.startWeight + j];
								const MD5MeshParser::Joint& joint = joints[weight.joint];

								finalPos += (joint.bindPos + joint.bindOrient * weight.pos) * weight.bias;
							}

							// On retourne le modèle dans le bon sens
							*posPtr++ = matrix * finalPos;
						}
					}

					// Vertex UVs
					if (auto uvPtr = vertexMapper.GetComponentPtr<Vector2f>(VertexComponent::TexCoord))
					{
						for (const MD5MeshParser::Vertex& md5Vertex : md5Mesh.vertices)
							*uvPtr++ = parameters.texCoordOffset + md5Vertex.uv * parameters.texCoordScale;
					}

					// Vertex colors (.md5mesh files have no vertex color)
					if (auto colorPtr = vertexMapper.GetComponentPtr<Color>(VertexComponent::Color))
					{
						for (std::size_t i = 0; i < md5Mesh.vertices.size(); ++i)
							*colorPtr++ = Color::White;
					}

					vertexMapper.Unmap();

					// Submesh
					std::shared_ptr<StaticMesh> subMesh = std::make_shared<StaticMesh>(vertexBuffer, indexBuffer);
					subMesh->GenerateAABB();
					subMesh->SetMaterialIndex(i);

					if (parameters.vertexDeclaration->HasComponentOfType<Vector3f>(VertexComponent::Normal))
					{
						if (parameters.vertexDeclaration->HasComponentOfType<Vector3f>(VertexComponent::Tangent))
							subMesh->GenerateNormalsAndTangents();
						else
							subMesh->GenerateNormals();
					}

					mesh->AddSubMesh(subMesh);

					// Material
					ParameterList matData;
					matData.SetParameter(MaterialData::FilePath, (baseDir / md5Mesh.shader).generic_u8string());

					mesh->SetMaterialData(i, std::move(matData));
				}

				if (parameters.center)
					mesh->Recenter();

				return mesh;
			}
		}
	}

	namespace Loaders
	{
		MeshLoader::Entry GetMeshLoader_MD5Mesh()
		{
			MeshLoader::Entry loader;
			loader.extensionSupport = IsSupported;
			loader.streamChecker = Check;
			loader.streamLoader = Load;

			return loader;
		}
	}
}
