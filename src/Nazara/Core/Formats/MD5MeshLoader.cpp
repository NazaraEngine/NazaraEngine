// Copyright (C) 2026 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Formats/MD5MeshLoader.hpp>
#include <Nazara/Core/IndexIterator.hpp>
#include <Nazara/Core/IndexMapper.hpp>
#include <Nazara/Core/Joint.hpp>
#include <Nazara/Core/MaterialData.hpp>
#include <Nazara/Core/Mesh.hpp>
#include <Nazara/Core/SkeletalMesh.hpp>
#include <Nazara/Core/Skeleton.hpp>
#include <Nazara/Core/StaticMesh.hpp>
#include <Nazara/Core/VertexMapper.hpp>
#include <Nazara/Core/Formats/MD5MeshParser.hpp>
#include <memory>

namespace Nz
{
	namespace
	{
		bool IsMD5MeshSupported(std::string_view extension)
		{
			return (extension == ".md5mesh");
		}

		Result<std::shared_ptr<Mesh>, ResourceLoadingError> LoadMD5Mesh(Stream& stream, const MeshParams& parameters)
		{
			MD5MeshParser parser(stream);

			UInt64 streamPos = stream.GetCursorPos();

			if (!parser.Check())
				return Err(ResourceLoadingError::Unrecognized);

			stream.SetCursorPos(streamPos);

			if (!parser.Parse())
			{
				NazaraError("MD5Mesh parser failed");
				return Err(ResourceLoadingError::DecodingError);
			}

			UInt32 maxWeightCount = 4;
			if (auto result = parameters.custom.GetIntegerParameter("MaxWeightCount"))
			{
				maxWeightCount = SafeCast<UInt32>(result.GetValue());
				if (maxWeightCount > 4)
				{
					NazaraWarning("MaxWeightCount cannot be over 4");
					maxWeightCount = 4;
				}
			}


			// Pour que le squelette soit correctement aligné, il faut appliquer un quaternion "de correction" aux joints à la base du squelette
			Quaternionf rotationQuat = Quaternionf::RotationBetween(Vector3f::UnitX(), Vector3f::Forward()) *
			                           Quaternionf::RotationBetween(Vector3f::UnitZ(), Vector3f::Up());

			std::filesystem::path baseDir = stream.GetDirectory();

			// Le hellknight de Doom 3 fait ~120 unités, et il est dit qu'il fait trois mètres
			// Nous réduisons donc la taille générale des fichiers MD5 de 1/40
			Matrix4f matrix = Matrix4f::Transform(Nz::Vector3f::Zero(), rotationQuat, Vector3f(1.f / 40.f));

			rotationQuat = Quaternionf::Identity();

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

					joint->SetInverseBindMatrix(Matrix4f::TransformInverse(joints[i].bindPos, joints[i].bindOrient));
					joint->SetName(joints[i].name);
				}

				mesh->SetMaterialCount(meshCount);
				for (UInt32 i = 0; i < meshCount; ++i)
				{
					const MD5MeshParser::Mesh& md5Mesh = meshes[i];

					UInt32 indexCount = SafeCast<UInt32>(md5Mesh.triangles.size() * 3);
					UInt32 vertexCount = SafeCast<UInt32>(md5Mesh.vertices.size());

					bool largeIndices = (vertexCount > std::numeric_limits<UInt16>::max());

					std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBuffer>((largeIndices) ? IndexType::U32 : IndexType::U16, indexCount, parameters.indexBufferFlags, parameters.bufferFactory);
					std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>(VertexDeclaration::Get(VertexLayout::XYZ_Normal_UV_Tangent_Skinning), UInt32(vertexCount), parameters.vertexBufferFlags, parameters.bufferFactory);

					// Index buffer
					IndexMapper indexMapper(*indexBuffer);

					if (parameters.reverseWinding)
					{
						// Winding order is reversed relative to what the engine expects
						UInt32 index = 0;
						for (const MD5MeshParser::Triangle& triangle : md5Mesh.triangles)
						{
							// On les respécifie dans le bon ordre (inversion du winding)
							indexMapper.Set(index++, triangle.x);
							indexMapper.Set(index++, triangle.y);
							indexMapper.Set(index++, triangle.z);
						}
					}
					else
					{
						UInt32 index = 0;
						for (const MD5MeshParser::Triangle& triangle : md5Mesh.triangles)
						{
							indexMapper.Set(index++, triangle.x);
							indexMapper.Set(index++, triangle.z);
							indexMapper.Set(index++, triangle.y);
						}
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

					VertexMapper vertexMapper(*vertexBuffer);

					auto posPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Position);
					auto jointIndicesPtr = vertexMapper.GetComponentPtr<Vector4i32>(VertexComponent::JointIndices);
					auto jointWeightPtr = vertexMapper.GetComponentPtr<Vector4f>(VertexComponent::JointWeights);
					auto uvPtr = vertexMapper.GetComponentPtr<Vector2f>(VertexComponent::TexCoord);

					for (const MD5MeshParser::Vertex& vertex : md5Mesh.vertices)
					{
						// Skinning MD5 (Formule d'Id Tech)
						Vector3f finalPos(Vector3f::Zero());

						// On stocke tous les poids dans le tableau temporaire en même temps qu'on calcule la position finale du sommet.
						tempWeights.resize(vertex.weightCount);
						for (unsigned int weightIndex = 0; weightIndex < vertex.weightCount; ++weightIndex)
						{
							const MD5MeshParser::Weight& weight = md5Mesh.weights[vertex.startWeight + weightIndex];
							const MD5MeshParser::Joint& joint = joints[weight.joint];

							finalPos += (joint.bindPos + joint.bindOrient * weight.pos) * weight.bias;

							// Avant d'ajouter les poids, il faut s'assurer qu'il n'y en ait pas plus que le maximum supporté
							// et dans le cas contraire, garder les poids les plus importants et les renormaliser
							tempWeights[weightIndex] = {weight.bias, weight.joint};
						}

						// Avons nous plus de poids que le moteur ne peut en supporter ?
						UInt32 weightCount = vertex.weightCount;
						if (weightCount > maxWeightCount)
						{
							// Pour augmenter la qualité du skinning tout en ne gardant que X poids, on ne garde que les poids
							// les plus importants, ayant le plus d'impact sur le sommet final
							std::sort(tempWeights.begin(), tempWeights.end(), [] (const Weight& a, const Weight& b) -> bool {
								return a.bias > b.bias;
							});

							// Sans oublier bien sûr de renormaliser les poids (que leur somme soit 1)
							float weightSum = 0.f;
							for (UInt32 j = 0; j < maxWeightCount; ++j)
								weightSum += tempWeights[j].bias;

							for (UInt32 j = 0; j < maxWeightCount; ++j)
								tempWeights[j].bias /= weightSum;

							weightCount = maxWeightCount;
						}

						if (posPtr)
							*posPtr++ = finalPos;

						if (uvPtr)
							*uvPtr++ = Vector2f(parameters.texCoordOffset + vertex.uv * parameters.texCoordScale);

						if (jointIndicesPtr)
						{
							Vector4i32& jointIndices = *jointIndicesPtr++;

							for (UInt32 j = 0; j < maxWeightCount; ++j)
								jointIndices[j] = (j < weightCount) ? tempWeights[j].jointIndex : 0;
						}

						if (jointWeightPtr)
						{
							Vector4f& jointWeights = *jointWeightPtr++;

							for (UInt32 j = 0; j < maxWeightCount; ++j)
								jointWeights[j] = (j < weightCount) ? tempWeights[j].bias : 0;
						}
					}

					// Vertex colors (.md5mesh files have no vertex color)
					if (auto colorPtr = vertexMapper.GetComponentPtr<Color>(VertexComponent::Color))
					{
						for (std::size_t j = 0; j < md5Mesh.vertices.size(); ++j)
							*colorPtr++ = Color::White();
					}

					vertexMapper.Unmap();

					// Material
					ParameterList matData;
					matData.SetParameter(MaterialData::BaseColorTexturePath, PathToString(baseDir / md5Mesh.shader));
					matData.SetParameter(MaterialData::Type, "Phong");

					mesh->SetMaterialData(i, std::move(matData));

					// Submesh
					std::shared_ptr<SkeletalMesh> subMesh = std::make_shared<SkeletalMesh>(vertexBuffer, indexBuffer);

					if (parameters.vertexDeclaration->HasComponentOfType<Vector3f>(VertexComponent::Normal))
					{
						if (parameters.vertexDeclaration->HasComponentOfType<Vector3f>(VertexComponent::Tangent))
							subMesh->GenerateNormalsAndTangents();
						else
							subMesh->GenerateNormals();
					}

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
					return Err(ResourceLoadingError::Internal);
				}

				mesh->SetMaterialCount(meshCount);
				for (UInt32 i = 0; i < meshCount; ++i)
				{
					const MD5MeshParser::Mesh& md5Mesh = meshes[i];
					UInt32 indexCount = SafeCast<UInt32>(md5Mesh.triangles.size() * 3);
					UInt32 vertexCount = SafeCast<UInt32>(md5Mesh.vertices.size());

					// Index buffer
					bool largeIndices = (vertexCount > std::numeric_limits<UInt16>::max());

					std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBuffer>((largeIndices) ? IndexType::U32 : IndexType::U16, indexCount, parameters.indexBufferFlags, parameters.bufferFactory);

					IndexMapper indexMapper(*indexBuffer);
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
					std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>(parameters.vertexDeclaration, vertexCount, parameters.vertexBufferFlags, parameters.bufferFactory);

					VertexMapper vertexMapper(*vertexBuffer);

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
						for (std::size_t j = 0; j < md5Mesh.vertices.size(); ++j)
							*colorPtr++ = Color::White();
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
					matData.SetParameter(MaterialData::BaseColorTexturePath, PathToString(baseDir / md5Mesh.shader));

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
			loader.extensionSupport = IsMD5MeshSupported;
			loader.streamLoader = LoadMD5Mesh;
			loader.parameterFilter = [](const MeshParams& parameters)
			{
				if (auto result = parameters.custom.GetBooleanParameter("SkipBuiltinMD5MeshLoader"); result.GetValueOr(false))
					return false;

				return true;
			};

			return loader;
		}
	}
}
