/*
Nazara Engine - Assimp Plugin

Copyright (C) 2015 Jérôme "Lynix" Leclercq (lynix680@gmail.com)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <CustomStream.hpp>
#include <Nazara/Utils/CallOnExit.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Animation.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/IndexIterator.hpp>
#include <Nazara/Utility/IndexMapper.hpp>
#include <Nazara/Utility/Joint.hpp>
#include <Nazara/Utility/MaterialData.hpp>
#include <Nazara/Utility/Sequence.hpp>
#include <Nazara/Utility/SkeletalMesh.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/VertexMapper.hpp>
#include <Nazara/Utility/Utility.hpp>
#include <assimp/cfileio.h>
#include <assimp/cimport.h>
#include <assimp/config.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <unordered_set>

using namespace Nz;

void ProcessJoints(aiNode* node, Skeleton* skeleton, const std::unordered_set<std::string_view>& joints)
{
	std::string_view jointName(node->mName.data, node->mName.length);
	if (joints.count(jointName))
	{
		Joint* joint = skeleton->GetJoint(std::string(jointName));

		if (node->mParent)
			joint->SetParent(skeleton->GetJoint(node->mParent->mName.C_Str()));

		Matrix4f transformMatrix(node->mTransformation.a1, node->mTransformation.a2, node->mTransformation.a3, node->mTransformation.a4,
		                         node->mTransformation.b1, node->mTransformation.b2, node->mTransformation.b3, node->mTransformation.b4,
		                         node->mTransformation.c1, node->mTransformation.c2, node->mTransformation.c3, node->mTransformation.c4,
		                         node->mTransformation.d1, node->mTransformation.d2, node->mTransformation.d3, node->mTransformation.d4);

		transformMatrix.Transpose();
		transformMatrix.InverseTransform();

		joint->SetInverseBindMatrix(transformMatrix);
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
		ProcessJoints(node->mChildren[i], skeleton, joints);
}

bool IsSupported(const std::string_view& extension)
{
	std::string dotExt;
	dotExt.reserve(extension.size() + 1);
	dotExt += '.';
	dotExt += extension;

	return (aiIsExtensionSupported(dotExt.data()) == AI_TRUE);
}

Ternary CheckAnimation(Stream& /*stream*/, const AnimationParams& parameters)
{
	bool skip;
	if (parameters.custom.GetBooleanParameter("SkipAssimpLoader", &skip) && skip)
		return Ternary::False;

	return Ternary::Unknown;
}

std::shared_ptr<Animation> LoadAnimation(Stream& stream, const AnimationParams& parameters)
{
	std::string streamPath = Nz::PathToString(stream.GetPath());

	FileIOUserdata userdata;
	userdata.originalFilePath = (!streamPath.empty()) ? streamPath.data() : StreamPath;
	userdata.originalStream = &stream;

	aiFileIO fileIO;
	fileIO.CloseProc = StreamCloser;
	fileIO.OpenProc = StreamOpener;
	fileIO.UserData = reinterpret_cast<char*>(&userdata);

	unsigned int postProcess = aiProcess_CalcTangentSpace  | aiProcess_Debone
	                         | aiProcess_FindInvalidData   | aiProcess_FixInfacingNormals
	                         | aiProcess_FlipWindingOrder  | aiProcess_GenSmoothNormals
	                         | aiProcess_GenUVCoords       | aiProcess_JoinIdenticalVertices
	                         | aiProcess_LimitBoneWeights  | aiProcess_MakeLeftHanded
	                         | aiProcess_OptimizeGraph     | aiProcess_OptimizeMeshes
	                         | aiProcess_RemoveComponent   | aiProcess_RemoveRedundantMaterials
	                         | aiProcess_SortByPType       | aiProcess_SplitLargeMeshes
	                         | aiProcess_TransformUVCoords | aiProcess_Triangulate;

	aiPropertyStore* properties = aiCreatePropertyStore();
	aiSetImportPropertyInteger(properties, AI_CONFIG_PP_LBW_MAX_WEIGHTS, 4);
	//aiSetImportPropertyInteger(properties, AI_CONFIG_PP_RVC_FLAGS,       ~aiComponent_ANIMATIONS);

	const aiScene* scene = aiImportFileExWithProperties(userdata.originalFilePath, postProcess, &fileIO, properties);
	aiReleasePropertyStore(properties);

	if (!scene)
	{
		NazaraError("Assimp failed to import file: " + std::string(aiGetErrorString()));
		return nullptr;
	}

	if (!scene->HasAnimations())
	{
		NazaraError("File has no animation");
		return nullptr;
	}

	aiAnimation* animation = scene->mAnimations[0];

	unsigned int maxFrameCount = 0;
	for (unsigned int i = 0; i < animation->mNumChannels; ++i)
	{
		aiNodeAnim* nodeAnim = animation->mChannels[i];

		maxFrameCount = std::max({ maxFrameCount, nodeAnim->mNumPositionKeys, nodeAnim->mNumRotationKeys, nodeAnim->mNumScalingKeys });
	}

	std::shared_ptr<Animation> anim = std::make_shared<Animation>();

	anim->CreateSkeletal(maxFrameCount, animation->mNumChannels);

	Sequence sequence;
	sequence.firstFrame = 0;
	sequence.frameCount = maxFrameCount;
	sequence.frameRate = animation->mTicksPerSecond;

	anim->AddSequence(sequence);

	SequenceJoint* sequenceJoints = anim->GetSequenceJoints();

	Quaternionf rotationQuat = Quaternionf::Identity();

	for (unsigned int i = 0; i < animation->mNumChannels; ++i)
	{
		aiNodeAnim* nodeAnim = animation->mChannels[i];
		for (unsigned int j = 0; j < nodeAnim->mNumPositionKeys; ++j)
		{
			SequenceJoint& sequenceJoint = sequenceJoints[i*animation->mNumChannels + j];

			aiQuaternion rotation = nodeAnim->mRotationKeys[j].mValue;
			aiVector3D position = nodeAnim->mPositionKeys[j].mValue;

			sequenceJoint.position = Vector3f(position.x, position.y, position.z);
			sequenceJoint.rotation = Quaternionf(rotation.w, rotation.x, rotation.y, rotation.z);
			sequenceJoint.scale.Set(1.f);
		}
	}

	return anim;
}

Ternary CheckMesh(Stream& /*stream*/, const MeshParams& parameters)
{
	bool skip;
	if (parameters.custom.GetBooleanParameter("SkipAssimpLoader", &skip) && skip)
		return Ternary::False;

	return Ternary::Unknown;
}

std::shared_ptr<Mesh> LoadMesh(Stream& stream, const MeshParams& parameters)
{
	std::string streamPath = Nz::PathToString(stream.GetPath());

	FileIOUserdata userdata;
	userdata.originalFilePath = (!streamPath.empty()) ? streamPath.data() : StreamPath;
	userdata.originalStream = &stream;

	aiFileIO fileIO;
	fileIO.CloseProc = StreamCloser;
	fileIO.OpenProc = StreamOpener;
	fileIO.UserData = reinterpret_cast<char*>(&userdata);

	unsigned int postProcess = aiProcess_CalcTangentSpace  | aiProcess_Debone
	                         | aiProcess_FindInvalidData   | aiProcess_FixInfacingNormals
	                         | aiProcess_FlipWindingOrder  | aiProcess_GenSmoothNormals
	                         | aiProcess_GenUVCoords       | aiProcess_JoinIdenticalVertices
	                         | aiProcess_LimitBoneWeights  | aiProcess_MakeLeftHanded
	                         | aiProcess_OptimizeGraph     | aiProcess_OptimizeMeshes
	                         | aiProcess_RemoveComponent   | aiProcess_RemoveRedundantMaterials
	                         | aiProcess_SortByPType       | aiProcess_SplitLargeMeshes
	                         | aiProcess_TransformUVCoords | aiProcess_Triangulate;

	if (parameters.optimizeIndexBuffers)
		postProcess |= aiProcess_ImproveCacheLocality;

	double smoothingAngle = 80.f;
	parameters.custom.GetDoubleParameter("AssimpLoader_SmoothingAngle", &smoothingAngle);

	long long triangleLimit = 1'000'000;
	parameters.custom.GetIntegerParameter("AssimpLoader_TriangleLimit", &triangleLimit);

	long long vertexLimit   = 1'000'000;
	parameters.custom.GetIntegerParameter("AssimpLoader_VertexLimit", &vertexLimit);

	int excludedComponents = 0;

	if (!parameters.vertexDeclaration->HasComponent(VertexComponent::Color))
		excludedComponents |= aiComponent_COLORS;

	if (!parameters.vertexDeclaration->HasComponent(VertexComponent::Normal))
		excludedComponents |= aiComponent_NORMALS;

	if (!parameters.vertexDeclaration->HasComponent(VertexComponent::Tangent))
		excludedComponents |= aiComponent_TANGENTS_AND_BITANGENTS;

	if (!parameters.vertexDeclaration->HasComponent(VertexComponent::TexCoord))
		excludedComponents |= aiComponent_TEXCOORDS;

	aiPropertyStore* properties = aiCreatePropertyStore();
	CallOnExit releaseProperties([&] { aiReleasePropertyStore(properties); });

	aiSetImportPropertyFloat(properties,   AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, float(smoothingAngle));
	aiSetImportPropertyInteger(properties, AI_CONFIG_PP_LBW_MAX_WEIGHTS,         4);
	aiSetImportPropertyInteger(properties, AI_CONFIG_PP_SBP_REMOVE,              ~aiPrimitiveType_TRIANGLE); //< We only want triangles
	aiSetImportPropertyInteger(properties, AI_CONFIG_PP_SLM_TRIANGLE_LIMIT,      int(triangleLimit));
	aiSetImportPropertyInteger(properties, AI_CONFIG_PP_SLM_VERTEX_LIMIT,        int(vertexLimit));
	aiSetImportPropertyInteger(properties, AI_CONFIG_PP_RVC_FLAGS,               excludedComponents);

	const aiScene* scene = aiImportFileExWithProperties(userdata.originalFilePath, postProcess, &fileIO, properties);
	CallOnExit releaseScene([&] { aiReleaseImport(scene); });

	releaseProperties.CallAndReset();

	if (!scene)
	{
		NazaraError("Assimp failed to import file: " + std::string(aiGetErrorString()));
		return nullptr;
	}

	std::unordered_set<std::string_view> joints;

	bool animatedMesh = false;
	if (parameters.animated)
	{
		for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
		{
			aiMesh* currentMesh = scene->mMeshes[meshIndex];
			if (currentMesh->HasBones()) // Inline functions can be safely called
			{
				animatedMesh = true;
				for (unsigned int boneIndex = 0; boneIndex < currentMesh->mNumBones; ++boneIndex)
				{
					aiBone* bone = currentMesh->mBones[boneIndex];
					joints.insert(bone->mName.C_Str());
				}
			}
		}
	}

	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	if (animatedMesh)
	{
		mesh->CreateSkeletal(UInt32(joints.size()));

		Skeleton* skeleton = mesh->GetSkeleton();

		// First, assign names
		unsigned int jointIndex = 0;
		for (std::string_view jointName : joints)
			skeleton->GetJoint(jointIndex++)->SetName(std::string(jointName));

		ProcessJoints(scene->mRootNode, skeleton, joints);

		// aiMaterial index in scene => Material index and data in Mesh
		std::unordered_map<unsigned int, std::pair<UInt32, ParameterList>> materials;

		for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
		{
			aiMesh* iMesh = scene->mMeshes[meshIndex];
			if (iMesh->HasBones())
				continue;

			unsigned int indexCount = iMesh->mNumFaces * 3;
			unsigned int vertexCount = iMesh->mNumVertices;

			// Index buffer
			bool largeIndices = (vertexCount > std::numeric_limits<UInt16>::max());

			std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBuffer>((largeIndices) ? IndexType::U32 : IndexType::U16, indexCount, parameters.indexBufferFlags, parameters.bufferFactory);

			IndexMapper indexMapper(*indexBuffer);
			IndexIterator index = indexMapper.begin();

			for (unsigned int faceIndex = 0; faceIndex < iMesh->mNumFaces; ++faceIndex)
			{
				const aiFace& face = iMesh->mFaces[faceIndex];
				if (face.mNumIndices != 3)
					NazaraWarning("Assimp plugin: This face is not a triangle!");

				*index++ = face.mIndices[0];
				*index++ = face.mIndices[1];
				*index++ = face.mIndices[2];
			}
			indexMapper.Unmap();

			// Make sure the normal/tangent matrix won't rescale our vectors
			Nz::Matrix4f normalTangentMatrix = parameters.matrix;
			if (normalTangentMatrix.HasScale())
				normalTangentMatrix.ApplyScale(1.f / normalTangentMatrix.GetScale());

			std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>(VertexDeclaration::Get(VertexLayout::XYZ_Normal_UV_Tangent_Skinning), vertexCount, parameters.vertexBufferFlags, parameters.bufferFactory);

			VertexMapper vertexMapper(*vertexBuffer);

			// Vertex positions
			if (auto posPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Position))
			{
				for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
				{
					aiVector3D position = iMesh->mVertices[vertexIdx];
					*posPtr++ = parameters.matrix * Vector3f(position.x, position.y, position.z);
				}
			}

			// Vertex normals
			if (auto normalPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Normal))
			{
				for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
				{
					aiVector3D normal = iMesh->mNormals[vertexIdx];
					*normalPtr++ = normalTangentMatrix.Transform({ normal.x, normal.y, normal.z }, 0.f);
				}
			}

			// Vertex tangents
			bool generateTangents = false;
			if (auto tangentPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Tangent))
			{
				if (iMesh->HasTangentsAndBitangents())
				{
					for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
					{
						aiVector3D tangent = iMesh->mTangents[vertexIdx];
						*tangentPtr++ = normalTangentMatrix.Transform({ tangent.x, tangent.y, tangent.z }, 0.f);
					}
				}
				else
					generateTangents = true;
			}

			// Vertex UVs
			if (auto uvPtr = vertexMapper.GetComponentPtr<Vector2f>(VertexComponent::TexCoord))
			{
				if (iMesh->HasTextureCoords(0))
				{
					for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
					{
						aiVector3D uv = iMesh->mTextureCoords[0][vertexIdx];
						*uvPtr++ = parameters.texCoordOffset + Vector2f(uv.x, uv.y) * parameters.texCoordScale;
					}
				}
				else
				{
					for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
						*uvPtr++ = Vector2f::Zero();
				}
			}

			// Vertex colors
			if (auto colorPtr = vertexMapper.GetComponentPtr<Color>(VertexComponent::Color))
			{
				if (iMesh->HasVertexColors(0))
				{
					for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
					{
						aiColor4D color = iMesh->mColors[0][vertexIdx];
						*colorPtr++ = Color(UInt8(color.r * 255.f), UInt8(color.g * 255.f), UInt8(color.b * 255.f), UInt8(color.a * 255.f));
					}
				}
				else
				{
					for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
						*colorPtr++ = Color::White;
				}
			}

			auto jointIndicesPtr = vertexMapper.GetComponentPtr<Vector4i32>(VertexComponent::JointIndices);
			auto jointWeightPtr = vertexMapper.GetComponentPtr<Vector4f>(VertexComponent::JointWeights);

			if (jointIndicesPtr || jointWeightPtr)
			{
				std::vector<std::size_t> weightIndices(iMesh->mNumVertices, 0);

				for (unsigned int boneIndex = 0; boneIndex < iMesh->mNumBones; ++boneIndex)
				{
					aiBone* bone = iMesh->mBones[boneIndex];
					for (unsigned int weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex)
					{
						aiVertexWeight& vertexWeight = bone->mWeights[weightIndex];

						std::size_t vertexWeightIndex = weightIndices[vertexWeight.mVertexId]++;

						if (jointIndicesPtr)
							jointIndicesPtr[vertexWeight.mVertexId][vertexWeightIndex] = boneIndex;

						if (jointWeightPtr)
							jointWeightPtr[vertexWeight.mVertexId][vertexWeightIndex] = vertexWeight.mWeight;
					}
				}
			}

			// Submesh
			std::shared_ptr<SkeletalMesh> subMesh = std::make_shared<SkeletalMesh>(vertexBuffer, indexBuffer);
			subMesh->SetMaterialIndex(iMesh->mMaterialIndex);

			auto matIt = materials.find(iMesh->mMaterialIndex);
			if (matIt == materials.end())
			{
				ParameterList matData;
				aiMaterial* aiMat = scene->mMaterials[iMesh->mMaterialIndex];

				auto ConvertColor = [&](const char* aiKey, unsigned int aiType, unsigned int aiIndex, const char* colorKey)
				{
					aiColor4D color;
					if (aiGetMaterialColor(aiMat, aiKey, aiType, aiIndex, &color) == aiReturn_SUCCESS)
					{
						matData.SetParameter(colorKey, Color(color.r, color.g, color.b, color.a));
						return true;
					}

					return false;
				};

				auto ConvertTexture = [&](aiTextureType aiType, const char* textureKey, const char* wrapKey = nullptr)
				{
					aiString path;
					aiTextureMapMode mapMode[3];
					if (aiGetMaterialTexture(aiMat, aiType, 0, &path, nullptr, nullptr, nullptr, nullptr, &mapMode[0], nullptr) == aiReturn_SUCCESS)
					{
						matData.SetParameter(textureKey, PathToString(stream.GetDirectory() / std::string_view(path.data, path.length)));

						if (wrapKey)
						{
							SamplerWrap wrap = SamplerWrap::Clamp;
							switch (mapMode[0])
							{
								case aiTextureMapMode_Clamp:
								case aiTextureMapMode_Decal:
									wrap = SamplerWrap::Clamp;
									break;

								case aiTextureMapMode_Mirror:
									wrap = SamplerWrap::MirroredRepeat;
									break;

								case aiTextureMapMode_Wrap:
									wrap = SamplerWrap::Repeat;
									break;

								default:
									NazaraWarning("Assimp texture map mode 0x" + NumberToString(mapMode[0], 16) + " not handled");
									break;
							}

							matData.SetParameter(wrapKey, static_cast<long long>(wrap));
						}

						return true;
					}

					return false;
				};

				ConvertColor(AI_MATKEY_COLOR_AMBIENT, MaterialData::AmbientColor);

				if (!ConvertColor(AI_MATKEY_BASE_COLOR, MaterialData::BaseColor))
					ConvertColor(AI_MATKEY_COLOR_DIFFUSE, MaterialData::BaseColor);

				ConvertColor(AI_MATKEY_COLOR_SPECULAR, MaterialData::SpecularColor);

				if (!ConvertTexture(aiTextureType_BASE_COLOR, MaterialData::BaseColorTexturePath, MaterialData::BaseColorWrap))
					ConvertTexture(aiTextureType_DIFFUSE, MaterialData::BaseColorTexturePath, MaterialData::BaseColorWrap);

				ConvertTexture(aiTextureType_DIFFUSE_ROUGHNESS, MaterialData::RoughnessTexturePath, MaterialData::RoughnessWrap);
				ConvertTexture(aiTextureType_EMISSIVE,          MaterialData::EmissiveTexturePath,  MaterialData::EmissiveWrap);
				ConvertTexture(aiTextureType_HEIGHT,            MaterialData::HeightTexturePath,    MaterialData::HeightWrap);
				ConvertTexture(aiTextureType_METALNESS,         MaterialData::MetallicTexturePath,  MaterialData::MetallicWrap);
				ConvertTexture(aiTextureType_NORMALS,           MaterialData::NormalTexturePath,    MaterialData::NormalWrap);
				ConvertTexture(aiTextureType_OPACITY,           MaterialData::AlphaTexturePath,     MaterialData::AlphaWrap);
				ConvertTexture(aiTextureType_SPECULAR,          MaterialData::SpecularTexturePath,  MaterialData::SpecularWrap);

				aiString name;
				if (aiGetMaterialString(aiMat, AI_MATKEY_NAME, &name) == aiReturn_SUCCESS)
					matData.SetParameter(MaterialData::Name, std::string(name.data, name.length));

				int iValue;
				if (aiGetMaterialInteger(aiMat, AI_MATKEY_TWOSIDED, &iValue) == aiReturn_SUCCESS)
					matData.SetParameter(MaterialData::FaceCulling, !iValue);

				matIt = materials.insert(std::make_pair(iMesh->mMaterialIndex, std::make_pair(UInt32(materials.size()), std::move(matData)))).first;
			}

			subMesh->SetMaterialIndex(matIt->first);

			mesh->AddSubMesh(subMesh);
		}

		mesh->SetMaterialCount(std::max<UInt32>(UInt32(materials.size()), 1));
		for (const auto& pair : materials)
			mesh->SetMaterialData(pair.second.first, pair.second.second);
	}
	else
	{
		mesh->CreateStatic();

		// aiMaterial index in scene => Material index and data in Mesh
		std::unordered_map<unsigned int, std::pair<UInt32, ParameterList>> materials;

		for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
		{
			aiMesh* iMesh = scene->mMeshes[meshIndex];
			if (iMesh->HasBones())
				continue; // Don't process skeletal meshes

			unsigned int indexCount = iMesh->mNumFaces * 3;
			unsigned int vertexCount = iMesh->mNumVertices;

			// Index buffer
			bool largeIndices = (vertexCount > std::numeric_limits<UInt16>::max());

			std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBuffer>((largeIndices) ? IndexType::U32 : IndexType::U16, indexCount, parameters.indexBufferFlags, parameters.bufferFactory);

			IndexMapper indexMapper(*indexBuffer);
			IndexIterator index = indexMapper.begin();

			for (unsigned int faceIndex = 0; faceIndex < iMesh->mNumFaces; ++faceIndex)
			{
				const aiFace& face = iMesh->mFaces[faceIndex];
				if (face.mNumIndices != 3)
					NazaraWarning("Assimp plugin: This face is not a triangle!");

				// Index buffer
				bool largeIndices = (vertexCount > std::numeric_limits<UInt16>::max());

				std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBuffer>((largeIndices) ? IndexType::U32 : IndexType::U16, indexCount, parameters.indexBufferFlags, parameters.bufferFactory);

				IndexMapper indexMapper(*indexBuffer);
				IndexIterator index = indexMapper.begin();

				for (unsigned int faceIdx = 0; faceIdx < iMesh->mNumFaces; ++faceIdx)
				{
					aiFace& face = iMesh->mFaces[faceIdx];
					if (face.mNumIndices != 3)
						NazaraWarning("Assimp plugin: This face is not a triangle!");

					*index++ = face.mIndices[0];
					*index++ = face.mIndices[1];
					*index++ = face.mIndices[2];
				}
				indexMapper.Unmap();

				// Vertex buffer

				// Make sure the normal/tangent matrix won't rescale our vectors
				Nz::Matrix4f normalTangentMatrix = parameters.matrix;
				if (normalTangentMatrix.HasScale())
					normalTangentMatrix.ApplyScale(1.f / normalTangentMatrix.GetScale());

				std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>(parameters.vertexDeclaration, vertexCount, parameters.vertexBufferFlags, parameters.bufferFactory);

				VertexMapper vertexMapper(*vertexBuffer);

				// Vertex positions
				if (auto posPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Position))
				{
					for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
					{
						aiVector3D position = iMesh->mVertices[vertexIdx];
						*posPtr++ = parameters.matrix * Vector3f(position.x, position.y, position.z);
					}
				}

				// Vertex normals
				if (auto normalPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Normal))
				{
					for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
					{
						aiVector3D normal = iMesh->mNormals[vertexIdx];
						*normalPtr++ = normalTangentMatrix.Transform({normal.x, normal.y, normal.z}, 0.f);
					}
				}

				// Vertex tangents
				bool generateTangents = false;
				if (auto tangentPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Tangent))
				{
					if (iMesh->HasTangentsAndBitangents())
					{
						for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
						{
							aiVector3D tangent = iMesh->mTangents[vertexIdx];
							*tangentPtr++ = normalTangentMatrix.Transform({tangent.x, tangent.y, tangent.z}, 0.f);
						}
					}
					else
						generateTangents = true;
				}

				// Vertex UVs
				if (auto uvPtr = vertexMapper.GetComponentPtr<Vector2f>(VertexComponent::TexCoord))
				{
					if (iMesh->HasTextureCoords(0))
					{
						for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
						{
							aiVector3D uv = iMesh->mTextureCoords[0][vertexIdx];
							*uvPtr++ = parameters.texCoordOffset + Vector2f(uv.x, uv.y) * parameters.texCoordScale;
						}
					}
					else
					{
						for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
							*uvPtr++ = Vector2f::Zero();
					}
				}

				// Vertex colors
				if (auto colorPtr = vertexMapper.GetComponentPtr<Color>(VertexComponent::Color))
				{
					if (iMesh->HasVertexColors(0))
					{
						for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
						{
							aiColor4D color = iMesh->mColors[0][vertexIdx];
							*colorPtr++ = Color(color.r, color.g, color.b, color.a);
						}
					}
					else
					{
						for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
							*colorPtr++ = Color::White;
					}
				}

				vertexMapper.Unmap();

				// Submesh
				std::shared_ptr<StaticMesh> subMesh = std::make_shared<StaticMesh>(vertexBuffer, indexBuffer);
				subMesh->GenerateAABB();
				subMesh->SetMaterialIndex(iMesh->mMaterialIndex);

				if (generateTangents)
					subMesh->GenerateTangents();

				auto matIt = materials.find(iMesh->mMaterialIndex);
				if (matIt == materials.end())
				{
					ParameterList matData;
					aiMaterial* aiMat = scene->mMaterials[iMesh->mMaterialIndex];

					auto ConvertColor = [&] (const char* aiKey, unsigned int aiType, unsigned int aiIndex, const char* colorKey)
					{
						aiColor4D color;
						if (aiGetMaterialColor(aiMat, aiKey, aiType, aiIndex, &color) == aiReturn_SUCCESS)
						{
							matData.SetParameter(colorKey, Color(color.r, color.g, color.b, color.a));
						}
					};

					auto ConvertTexture = [&] (aiTextureType aiType, const char* textureKey, const char* wrapKey = nullptr)
					{
						aiString path;
						aiTextureMapMode mapMode[3];
						if (aiGetMaterialTexture(aiMat, aiType, 0, &path, nullptr, nullptr, nullptr, nullptr, &mapMode[0], nullptr) == aiReturn_SUCCESS)
						{
							matData.SetParameter(textureKey, Nz::PathToString(stream.GetDirectory() / std::string_view(path.data, path.length)));

							if (wrapKey)
							{
								SamplerWrap wrap = SamplerWrap::Clamp;
								switch (mapMode[0])
								{
									case aiTextureMapMode_Clamp:
									case aiTextureMapMode_Decal:
										wrap = SamplerWrap::Clamp;
										break;

									case aiTextureMapMode_Mirror:
										wrap = SamplerWrap::MirroredRepeat;
										break;

									case aiTextureMapMode_Wrap:
										wrap = SamplerWrap::Repeat;
										break;

									default:
										NazaraWarning("Assimp texture map mode 0x" + NumberToString(mapMode[0], 16) + " not handled");
										break;
								}

								matData.SetParameter(wrapKey, static_cast<long long>(wrap));
							}
						}
					};

					ConvertColor(AI_MATKEY_COLOR_AMBIENT, MaterialData::AmbientColor);
					ConvertColor(AI_MATKEY_COLOR_DIFFUSE, MaterialData::BaseColor);
					ConvertColor(AI_MATKEY_COLOR_SPECULAR, MaterialData::SpecularColor);

					ConvertTexture(aiTextureType_DIFFUSE, MaterialData::BaseColorTexturePath, MaterialData::BaseColorWrap);
					ConvertTexture(aiTextureType_EMISSIVE, MaterialData::EmissiveTexturePath);
					ConvertTexture(aiTextureType_HEIGHT, MaterialData::HeightTexturePath);
					ConvertTexture(aiTextureType_NORMALS, MaterialData::NormalTexturePath);
					ConvertTexture(aiTextureType_OPACITY, MaterialData::AlphaTexturePath);
					ConvertTexture(aiTextureType_SPECULAR, MaterialData::SpecularTexturePath, MaterialData::SpecularWrap);

					aiString name;
					if (aiGetMaterialString(aiMat, AI_MATKEY_NAME, &name) == aiReturn_SUCCESS)
						matData.SetParameter(MaterialData::Name, std::string(name.data, name.length));

					int iValue;
					if (aiGetMaterialInteger(aiMat, AI_MATKEY_TWOSIDED, &iValue) == aiReturn_SUCCESS)
						matData.SetParameter(MaterialData::FaceCulling, !iValue);

					matIt = materials.insert(std::make_pair(iMesh->mMaterialIndex, std::make_pair(UInt32(materials.size()), std::move(matData)))).first;
				}

				subMesh->SetMaterialIndex(matIt->first);

				mesh->AddSubMesh(subMesh);
			}
			indexMapper.Unmap();

			// Vertex buffer

			// Make sure the normal/tangent matrix won't rescale our vectors
			Nz::Matrix4f normalTangentMatrix = parameters.matrix;
			if (normalTangentMatrix.HasScale())
				normalTangentMatrix.ApplyScale(1.f / normalTangentMatrix.GetScale());

			std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>(parameters.vertexDeclaration, vertexCount, parameters.vertexBufferFlags, parameters.bufferFactory);

			VertexMapper vertexMapper(*vertexBuffer);

			// Vertex positions
			if (auto posPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Position))
			{
				for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
				{
					aiVector3D position = iMesh->mVertices[vertexIdx];
					*posPtr++ = parameters.matrix * Vector3f(position.x, position.y, position.z);
				}
			}

			// Vertex normals
			if (auto normalPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Normal))
			{
				for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
				{
					aiVector3D normal = iMesh->mNormals[vertexIdx];
					*normalPtr++ = normalTangentMatrix.Transform({normal.x, normal.y, normal.z}, 0.f);
				}
			}

			// Vertex tangents
			bool generateTangents = false;
			if (auto tangentPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Tangent))
			{
				if (iMesh->HasTangentsAndBitangents())
				{
					for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
					{
						aiVector3D tangent = iMesh->mTangents[vertexIdx];
						*tangentPtr++ = normalTangentMatrix.Transform({tangent.x, tangent.y, tangent.z}, 0.f);
					}
				}
				else
					generateTangents = true;
			}

			// Vertex UVs
			if (auto uvPtr = vertexMapper.GetComponentPtr<Vector2f>(VertexComponent::TexCoord))
			{
				if (iMesh->HasTextureCoords(0))
				{
					for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
					{
						aiVector3D uv = iMesh->mTextureCoords[0][vertexIdx];
						*uvPtr++ = parameters.texCoordOffset + Vector2f(uv.x, uv.y) * parameters.texCoordScale;
					}
				}
				else
				{
					for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
						*uvPtr++ = Vector2f::Zero();
				}
			}

			// Vertex colors
			if (auto colorPtr = vertexMapper.GetComponentPtr<Color>(VertexComponent::Color))
			{
				if (iMesh->HasVertexColors(0))
				{
					for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
					{
						aiColor4D color = iMesh->mColors[0][vertexIdx];
						*colorPtr++ = Color(UInt8(color.r * 255.f), UInt8(color.g * 255.f), UInt8(color.b * 255.f), UInt8(color.a * 255.f));
					}
				}
				else
				{
					for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
						*colorPtr++ = Color::White;
				}
			}

			vertexMapper.Unmap();

			// Submesh
			std::shared_ptr<StaticMesh> subMesh = std::make_shared<StaticMesh>(vertexBuffer, indexBuffer);
			subMesh->GenerateAABB();
			subMesh->SetMaterialIndex(iMesh->mMaterialIndex);

			if (generateTangents)
				subMesh->GenerateTangents();

			auto matIt = materials.find(iMesh->mMaterialIndex);
			if (matIt == materials.end())
			{
				ParameterList matData;
				aiMaterial* aiMat = scene->mMaterials[iMesh->mMaterialIndex];

				auto ConvertColor = [&] (const char* aiKey, unsigned int aiType, unsigned int aiIndex, const char* colorKey)
				{
					aiColor4D color;
					if (aiGetMaterialColor(aiMat, aiKey, aiType, aiIndex, &color) == aiReturn_SUCCESS)
					{
						matData.SetParameter(colorKey, Color(static_cast<UInt8>(color.r * 255), static_cast<UInt8>(color.g * 255), static_cast<UInt8>(color.b * 255), static_cast<UInt8>(color.a * 255)));
					}
				};

				auto ConvertTexture = [&] (aiTextureType aiType, const char* textureKey, const char* wrapKey = nullptr)
				{
					aiString path;
					aiTextureMapMode mapMode[3];
					if (aiGetMaterialTexture(aiMat, aiType, 0, &path, nullptr, nullptr, nullptr, nullptr, &mapMode[0], nullptr) == aiReturn_SUCCESS)
					{
						matData.SetParameter(textureKey, (stream.GetDirectory() / std::string_view(path.data, path.length)).generic_u8string());

						if (wrapKey)
						{
							SamplerWrap wrap = SamplerWrap::Clamp;
							switch (mapMode[0])
							{
								case aiTextureMapMode_Clamp:
								case aiTextureMapMode_Decal:
									wrap = SamplerWrap::Clamp;
									break;

								case aiTextureMapMode_Mirror:
									wrap = SamplerWrap::MirroredRepeat;
									break;

								case aiTextureMapMode_Wrap:
									wrap = SamplerWrap::Repeat;
									break;

								default:
									NazaraWarning("Assimp texture map mode 0x" + NumberToString(mapMode[0], 16) + " not handled");
									break;
							}

							matData.SetParameter(wrapKey, static_cast<long long>(wrap));
						}
					}
				};

				ConvertColor(AI_MATKEY_COLOR_AMBIENT, MaterialData::AmbientColor);
				ConvertColor(AI_MATKEY_COLOR_DIFFUSE, MaterialData::DiffuseColor);
				ConvertColor(AI_MATKEY_COLOR_SPECULAR, MaterialData::SpecularColor);

				ConvertTexture(aiTextureType_DIFFUSE, MaterialData::DiffuseTexturePath, MaterialData::DiffuseWrap);
				ConvertTexture(aiTextureType_EMISSIVE, MaterialData::EmissiveTexturePath);
				ConvertTexture(aiTextureType_HEIGHT, MaterialData::HeightTexturePath);
				ConvertTexture(aiTextureType_NORMALS, MaterialData::NormalTexturePath);
				ConvertTexture(aiTextureType_OPACITY, MaterialData::AlphaTexturePath);
				ConvertTexture(aiTextureType_SPECULAR, MaterialData::SpecularTexturePath, MaterialData::SpecularWrap);

				aiString name;
				if (aiGetMaterialString(aiMat, AI_MATKEY_NAME, &name) == aiReturn_SUCCESS)
					matData.SetParameter(MaterialData::Name, std::string(name.data, name.length));

				int iValue;
				if (aiGetMaterialInteger(aiMat, AI_MATKEY_TWOSIDED, &iValue) == aiReturn_SUCCESS)
					matData.SetParameter(MaterialData::FaceCulling, !iValue);

				matIt = materials.insert(std::make_pair(iMesh->mMaterialIndex, std::make_pair(UInt32(materials.size()), std::move(matData)))).first;
			}

			subMesh->SetMaterialIndex(matIt->first);

			mesh->AddSubMesh(subMesh);
			

			mesh->SetMaterialCount(std::max<UInt32>(UInt32(materials.size()), 1));
			for (const auto& pair : materials)
				mesh->SetMaterialData(pair.second.first, pair.second.second);
		}

		if (parameters.center)
			mesh->Recenter();
	}

	return mesh;
}

namespace
{
	const Nz::AnimationLoader::Entry* animationLoaderEntry = nullptr;
	const Nz::MeshLoader::Entry* meshLoaderEntry = nullptr;
}

extern "C"
{
	NAZARA_EXPORT int PluginLoad()
	{
		Nz::Utility* utility = Nz::Utility::Instance();
		NazaraAssert(utility, "utility module is not instancied");

		Nz::AnimationLoader& animationLoader = utility->GetAnimationLoader();
		animationLoaderEntry = animationLoader.RegisterLoader({
			IsSupported,
			nullptr,
			nullptr,
			CheckAnimation,
			LoadAnimation
		});
		
		Nz::MeshLoader& meshLoader = utility->GetMeshLoader();
		meshLoaderEntry = meshLoader.RegisterLoader({
			IsSupported,
			nullptr,
			nullptr,
			CheckMesh,
			LoadMesh
		});

		return 1;
	}

	NAZARA_EXPORT void PluginUnload()
	{
		Nz::Utility* utility = Nz::Utility::Instance();
		NazaraAssert(utility, "utility module is not instancied");

		Nz::AnimationLoader& animationLoader = utility->GetAnimationLoader();
		animationLoader.UnregisterLoader(animationLoaderEntry);
		animationLoaderEntry = nullptr;

		Nz::MeshLoader& meshLoader = utility->GetMeshLoader();
		meshLoader.UnregisterLoader(meshLoaderEntry);
		meshLoaderEntry = nullptr;
	}
}
