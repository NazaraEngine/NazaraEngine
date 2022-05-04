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
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct SceneInfo
{
	struct Node
	{
		const aiNode* node;
		std::size_t totalChildrenCount;
	};

	struct SkeletalMesh
	{
		const aiMesh* mesh;
		std::size_t nodeIndex;
		std::size_t skeletonRootIndex;
		std::unordered_map<std::string, unsigned int> bones;
	};

	struct StaticMesh
	{
		const aiMesh* mesh;
		std::size_t nodeIndex;
	};

	std::unordered_multimap<std::string, std::size_t> nodeByName;
	std::vector<Node> nodes;
	std::vector<SkeletalMesh> skeletalMeshes;
	std::vector<StaticMesh> staticMeshes;
};

void VisitNodes(SceneInfo& sceneInfo, const aiScene* scene, const aiNode* node)
{
	std::size_t nodeIndex = sceneInfo.nodes.size();
	sceneInfo.nodeByName.emplace(node->mName.C_Str(), nodeIndex);
	auto& sceneNode = sceneInfo.nodes.emplace_back();
	sceneNode.node = node;

	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		if (mesh->HasBones())
		{
			auto& skeletalMesh = sceneInfo.skeletalMeshes.emplace_back();
			skeletalMesh.mesh = mesh;
			skeletalMesh.nodeIndex = nodeIndex;

			for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
				skeletalMesh.bones.emplace(mesh->mBones[boneIndex]->mName.C_Str(), boneIndex);
		}
		else
		{
			auto& staticMesh = sceneInfo.staticMeshes.emplace_back();
			staticMesh.mesh = mesh;
			staticMesh.nodeIndex = nodeIndex;
		}
	}

	std::size_t prevNodeCount = sceneInfo.nodes.size();

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
		VisitNodes(sceneInfo, scene, node->mChildren[i]);

	// Can't use sceneNode from there

	sceneInfo.nodes[nodeIndex].totalChildrenCount = sceneInfo.nodes.size() - prevNodeCount;
}

bool FindSkeletonRoot(SceneInfo& sceneInfo, SceneInfo::SkeletalMesh& skeletalMesh, const aiNode* node)
{
	if (skeletalMesh.bones.find(node->mName.C_Str()) != skeletalMesh.bones.end())
	{
		// Get to parents until there's only one child
		while (node->mParent && node->mParent->mNumChildren != 1)
			node = node->mParent;

		/*if (!node->mParent && node->mNumChildren > 1)
		{
			NazaraError("failed to identify skeleton root node");
			return false;
		}*/

		auto range = sceneInfo.nodeByName.equal_range(node->mName.C_Str());
		if (std::distance(range.first, range.second) != 1)
		{
			NazaraError("failed to identify skeleton root node: " + std::to_string(std::distance(range.first, range.second)) + " node(s) matched");
			return false;
		}

		skeletalMesh.skeletonRootIndex = range.first->second;
		return true;
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		if (FindSkeletonRoot(sceneInfo, skeletalMesh, node->mChildren[i]))
			return true;
	}

	return false;
}

void ProcessJoints(const SceneInfo::SkeletalMesh& skeletalMesh, Nz::Skeleton* skeleton, const aiNode* node, std::size_t& jointIndex)
{
	Nz::Joint* joint = skeleton->GetJoint(jointIndex);
	joint->SetName(node->mName.C_Str());

	if (jointIndex != 0)
		joint->SetParent(skeleton->GetJoint(node->mParent->mName.C_Str()));

	jointIndex++;

	if (auto it = skeletalMesh.bones.find(node->mName.C_Str()); it != skeletalMesh.bones.end())
	{
		const aiBone* bone = skeletalMesh.mesh->mBones[it->second];

		Nz::Matrix4f offsetMatrix(bone->mOffsetMatrix.a1, bone->mOffsetMatrix.b1, bone->mOffsetMatrix.c1, bone->mOffsetMatrix.d1,
		                          bone->mOffsetMatrix.a2, bone->mOffsetMatrix.b2, bone->mOffsetMatrix.c2, bone->mOffsetMatrix.d2,
		                          bone->mOffsetMatrix.a3, bone->mOffsetMatrix.b3, bone->mOffsetMatrix.c3, bone->mOffsetMatrix.d3,
		                          bone->mOffsetMatrix.a4, bone->mOffsetMatrix.b4, bone->mOffsetMatrix.c4, bone->mOffsetMatrix.d4);

		joint->SetInverseBindMatrix(offsetMatrix);
	}
	else
		joint->SetInverseBindMatrix(Nz::Matrix4f::Identity());

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
		ProcessJoints(skeletalMesh, skeleton, node->mChildren[i], jointIndex);
}

bool IsSupported(const std::string_view& extension)
{
	std::string dotExt;
	dotExt.reserve(extension.size() + 1);
	dotExt += '.';
	dotExt += extension;

	return (aiIsExtensionSupported(dotExt.data()) == AI_TRUE);
}

Nz::Ternary CheckAnimation(Nz::Stream& /*stream*/, const Nz::AnimationParams& parameters)
{
	bool skip;
	if (parameters.custom.GetBooleanParameter("SkipAssimpLoader", &skip) && skip)
		return Nz::Ternary::False;

	return Nz::Ternary::Unknown;
}

std::shared_ptr<Nz::Animation> LoadAnimation(Nz::Stream& stream, const Nz::AnimationParams& parameters)
{
	NazaraAssert(parameters.IsValid(), "invalid animation parameters");

	std::string streamPath = Nz::PathToString(stream.GetPath());

	FileIOUserdata userdata;
	userdata.originalFilePath = (!streamPath.empty()) ? streamPath.data() : StreamPath;
	userdata.originalStream = &stream;

	aiFileIO fileIO;
	fileIO.CloseProc = StreamCloser;
	fileIO.OpenProc = StreamOpener;
	fileIO.UserData = reinterpret_cast<char*>(&userdata);

	unsigned int postProcess = aiProcess_CalcTangentSpace  /*| aiProcess_Debone*/
	                         | aiProcess_FindInvalidData   | aiProcess_FixInfacingNormals
	                         | aiProcess_FlipWindingOrder  | aiProcess_GenSmoothNormals
	                         | aiProcess_GenUVCoords       | aiProcess_JoinIdenticalVertices
	                         | aiProcess_LimitBoneWeights  | aiProcess_MakeLeftHanded
	                         /*| aiProcess_OptimizeGraph     | aiProcess_OptimizeMeshes*/
	                         | aiProcess_RemoveComponent   | aiProcess_RemoveRedundantMaterials
	                         | aiProcess_SortByPType       | aiProcess_SplitLargeMeshes
	                         | aiProcess_TransformUVCoords | aiProcess_Triangulate;

	aiPropertyStore* properties = aiCreatePropertyStore();
	aiSetImportPropertyInteger(properties, AI_CONFIG_PP_LBW_MAX_WEIGHTS, 4);
	aiSetImportPropertyInteger(properties, AI_CONFIG_PP_SBP_REMOVE, ~aiPrimitiveType_TRIANGLE); //< We only want triangles
	Nz::CallOnExit releaseProperties([&] { aiReleasePropertyStore(properties); });

	const aiScene* scene = aiImportFileExWithProperties(userdata.originalFilePath, postProcess, &fileIO, properties);
	Nz::CallOnExit releaseScene([&] { aiReleaseImport(scene); });

	releaseProperties.CallAndReset();

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

	SceneInfo sceneInfo;
	VisitNodes(sceneInfo, scene, scene->mRootNode);

	const aiAnimation* animation = scene->mAnimations[0];

	unsigned int maxFrameCount = 0;
	for (unsigned int i = 0; i < animation->mNumChannels; ++i)
	{
		const aiNodeAnim* nodeAnim = animation->mChannels[i];

		maxFrameCount = std::max({ maxFrameCount, nodeAnim->mNumPositionKeys, nodeAnim->mNumRotationKeys, nodeAnim->mNumScalingKeys });
	}

	std::shared_ptr<Nz::Animation> anim = std::make_shared<Nz::Animation>();

	anim->CreateSkeletal(maxFrameCount, parameters.skeleton->GetJointCount());

	Nz::Sequence sequence;
	sequence.firstFrame = 0;
	sequence.frameCount = maxFrameCount;
	sequence.frameRate = (animation->mTicksPerSecond != 0.0) ? animation->mTicksPerSecond : 24.0;

	anim->AddSequence(sequence);

	for (unsigned int i = 0; i < animation->mNumChannels; ++i)
	{
		const aiNodeAnim* nodeAnim = animation->mChannels[i];

		std::size_t jointIndex = parameters.skeleton->GetJointIndex(nodeAnim->mNodeName.C_Str());
		if (jointIndex == Nz::Skeleton::InvalidJointIndex)
			continue;

		Nz::Vector3f currentPosition = Nz::Vector3f::Zero();
		Nz::Vector3f currentScale = Nz::Vector3f::Unit();
		Nz::Quaternionf currentRotation = Nz::Quaternionf::Identity();

		unsigned int positionKeyIndex = std::numeric_limits<unsigned int>::max();
		unsigned int rotationKeyIndex = std::numeric_limits<unsigned int>::max();
		unsigned int scaleKeyIndex = std::numeric_limits<unsigned int>::max();

		for (unsigned int frameIndex = 0; frameIndex < maxFrameCount; ++frameIndex)
		{
			double frameTime = frameIndex;

			for (unsigned int nextPos = positionKeyIndex + 1; nextPos < nodeAnim->mNumPositionKeys; ++nextPos)
			{
				if (nodeAnim->mPositionKeys[nextPos].mTime > frameTime)
				{
					if (--nextPos != positionKeyIndex)
					{
						const aiVector3D& vec = nodeAnim->mPositionKeys[nextPos].mValue;
						currentPosition = Nz::Vector3f(vec.x, vec.y, vec.z);
						positionKeyIndex = nextPos;
					}
					break;
				}
			}

			for (unsigned int nextRot = rotationKeyIndex + 1; nextRot < nodeAnim->mNumRotationKeys; ++nextRot)
			{
				if (nodeAnim->mRotationKeys[nextRot].mTime > frameTime)
				{
					if (--nextRot != rotationKeyIndex)
					{
						const aiQuaternion& rot = nodeAnim->mRotationKeys[nextRot].mValue;
						currentRotation = Nz::Quaternionf(rot.w, rot.x, rot.y, rot.z);
						rotationKeyIndex = nextRot;
					}
					break;
				}
			}

			// TODO: Scale

			Nz::SequenceJoint* sequenceJoints = anim->GetSequenceJoints(frameIndex);
			sequenceJoints[jointIndex].position = currentPosition;
			sequenceJoints[jointIndex].rotation = currentRotation;
			sequenceJoints[jointIndex].scale = currentScale;
		}
	}

	Nz::Quaternionf rotationQuat = Nz::Quaternionf::Identity();

	/*for (unsigned int i = 0; i < animation->mNumChannels; ++i)
	{
		const aiNodeAnim* nodeAnim = animation->mChannels[i];

		unsigned int keyCount = std::max({ nodeAnim->mNumPositionKeys, nodeAnim->mNumRotationKeys, nodeAnim->mNumScalingKeys });
		if (nodeAnim->mNumPositionKeys != keyCount && nodeAnim->mNumPositionKeys != 0)
			NazaraWarning("expected at least one position key, got 0");

		if (nodeAnim->mNumRotationKeys != keyCount && nodeAnim->mNumRotationKeys != 0)
			NazaraWarning("expected at least one rotation key, got 0");

		if (nodeAnim->mNumScalingKeys != keyCount && nodeAnim->mNumScalingKeys != 0)
			NazaraWarning("expected at least one scaling key, got 0");

		for (unsigned int j = 0; j < keyCount; ++j)
		{
			unsigned int posKey = std::min(j, nodeAnim->mNumPositionKeys - 1);
			unsigned int rotKey = std::min(j, nodeAnim->mNumRotationKeys - 1);
			unsigned int scaleKey = std::min(j, nodeAnim->mNumScalingKeys - 1);

			aiQuaternion rotation = nodeAnim->mRotationKeys[posKey].mValue;
			aiVector3D position = nodeAnim->mPositionKeys[rotKey].mValue;
			aiVector3D scaling = nodeAnim->mScalingKeys[scaleKey].mValue;

			Nz::SequenceJoint& sequenceJoint = sequenceJoints[i*animation->mNumChannels + j];

			sequenceJoint.position = Nz::Vector3f(position.x, position.y, position.z);
			sequenceJoint.rotation = Nz::Quaternionf(rotation.w, rotation.x, rotation.y, rotation.z);
			sequenceJoint.scale = Nz::Vector3f(scaling.x, scaling.y, scaling.z);
		}
	}*/

	return anim;
}

Nz::Ternary CheckMesh(Nz::Stream& /*stream*/, const Nz::MeshParams& parameters)
{
	bool skip;
	if (parameters.custom.GetBooleanParameter("SkipAssimpLoader", &skip) && skip)
		return Nz::Ternary::False;

	return Nz::Ternary::Unknown;
}

std::shared_ptr<Nz::Mesh> LoadMesh(Nz::Stream& stream, const Nz::MeshParams& parameters)
{
	std::string streamPath = Nz::PathToString(stream.GetPath());

	FileIOUserdata userdata;
	userdata.originalFilePath = (!streamPath.empty()) ? streamPath.data() : StreamPath;
	userdata.originalStream = &stream;

	aiFileIO fileIO;
	fileIO.CloseProc = StreamCloser;
	fileIO.OpenProc = StreamOpener;
	fileIO.UserData = reinterpret_cast<char*>(&userdata);

	unsigned int postProcess = aiProcess_CalcTangentSpace  /*| aiProcess_Debone*/
	                         | aiProcess_FindInvalidData   | aiProcess_FixInfacingNormals
	                         | aiProcess_FlipWindingOrder  | aiProcess_GenSmoothNormals
	                         | aiProcess_GenUVCoords       | aiProcess_JoinIdenticalVertices
	                         | aiProcess_LimitBoneWeights  | aiProcess_MakeLeftHanded
	                         /*| aiProcess_OptimizeGraph     | aiProcess_OptimizeMeshes*/
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

	if (!parameters.vertexDeclaration->HasComponent(Nz::VertexComponent::Color))
		excludedComponents |= aiComponent_COLORS;

	if (!parameters.vertexDeclaration->HasComponent(Nz::VertexComponent::Normal))
		excludedComponents |= aiComponent_NORMALS;

	if (!parameters.vertexDeclaration->HasComponent(Nz::VertexComponent::Tangent))
		excludedComponents |= aiComponent_TANGENTS_AND_BITANGENTS;

	if (!parameters.vertexDeclaration->HasComponent(Nz::VertexComponent::TexCoord))
		excludedComponents |= aiComponent_TEXCOORDS;

	aiPropertyStore* properties = aiCreatePropertyStore();
	Nz::CallOnExit releaseProperties([&] { aiReleasePropertyStore(properties); });

	aiSetImportPropertyFloat(properties,   AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, float(smoothingAngle));
	aiSetImportPropertyInteger(properties, AI_CONFIG_PP_LBW_MAX_WEIGHTS,         4);
	aiSetImportPropertyInteger(properties, AI_CONFIG_PP_SBP_REMOVE,              ~aiPrimitiveType_TRIANGLE); //< We only want triangles
	aiSetImportPropertyInteger(properties, AI_CONFIG_PP_SLM_TRIANGLE_LIMIT,      int(triangleLimit));
	aiSetImportPropertyInteger(properties, AI_CONFIG_PP_SLM_VERTEX_LIMIT,        int(vertexLimit));
	aiSetImportPropertyInteger(properties, AI_CONFIG_PP_RVC_FLAGS,               excludedComponents);

	const aiScene* scene = aiImportFileExWithProperties(userdata.originalFilePath, postProcess, &fileIO, properties);
	Nz::CallOnExit releaseScene([&] { aiReleaseImport(scene); });

	releaseProperties.CallAndReset();

	if (!scene)
	{
		NazaraError("Assimp failed to import file: " + std::string(aiGetErrorString()));
		return nullptr;
	}

	SceneInfo sceneInfo;
	VisitNodes(sceneInfo, scene, scene->mRootNode);

	for (auto& skeletalMesh : sceneInfo.skeletalMeshes)
	{
		if (!FindSkeletonRoot(sceneInfo, skeletalMesh, scene->mRootNode))
			return nullptr;
	}

	std::shared_ptr<Nz::Mesh> mesh = std::make_shared<Nz::Mesh>();
	if (parameters.animated && !sceneInfo.skeletalMeshes.empty())
	{
		auto& skeletalMesh = sceneInfo.skeletalMeshes.front();
		auto& skeletalRoot = sceneInfo.nodes[skeletalMesh.skeletonRootIndex];

		mesh->CreateSkeletal(Nz::SafeCast<Nz::UInt32>(skeletalRoot.totalChildrenCount + 1));

		Nz::Skeleton* skeleton = mesh->GetSkeleton();

		std::size_t jointIndex = 0;
		ProcessJoints(skeletalMesh, skeleton, skeletalRoot.node, jointIndex);

		// aiMaterial index in scene => Material index and data in Mesh
		std::unordered_map<unsigned int, std::pair<Nz::UInt32, Nz::ParameterList>> materials;

		const aiMesh* iMesh = skeletalMesh.mesh;

		unsigned int indexCount = iMesh->mNumFaces * 3;
		unsigned int vertexCount = iMesh->mNumVertices;

		// Index buffer
		bool largeIndices = (vertexCount > std::numeric_limits<Nz::UInt16>::max());

		std::shared_ptr<Nz::IndexBuffer> indexBuffer = std::make_shared<Nz::IndexBuffer>((largeIndices) ? Nz::IndexType::U32 : Nz::IndexType::U16, indexCount, parameters.indexBufferFlags, parameters.bufferFactory);

		Nz::IndexMapper indexMapper(*indexBuffer);
		Nz::IndexIterator index = indexMapper.begin();

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

		std::shared_ptr<Nz::VertexBuffer> vertexBuffer = std::make_shared<Nz::VertexBuffer>(Nz::VertexDeclaration::Get(Nz::VertexLayout::XYZ_Normal_UV_Tangent_Skinning), vertexCount, parameters.vertexBufferFlags, parameters.bufferFactory);

		Nz::VertexMapper vertexMapper(*vertexBuffer);

		// Vertex positions
		if (auto posPtr = vertexMapper.GetComponentPtr<Nz::Vector3f>(Nz::VertexComponent::Position))
		{
			for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
			{
				aiVector3D position = iMesh->mVertices[vertexIdx];
				*posPtr++ = parameters.matrix * Nz::Vector3f(position.x, position.y, position.z);
			}
		}

		// Vertex normals
		if (auto normalPtr = vertexMapper.GetComponentPtr<Nz::Vector3f>(Nz::VertexComponent::Normal))
		{
			for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
			{
				aiVector3D normal = iMesh->mNormals[vertexIdx];
				*normalPtr++ = normalTangentMatrix.Transform({ normal.x, normal.y, normal.z }, 0.f);
			}
		}

		// Vertex tangents
		bool generateTangents = false;
		if (auto tangentPtr = vertexMapper.GetComponentPtr<Nz::Vector3f>(Nz::VertexComponent::Tangent))
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
		if (auto uvPtr = vertexMapper.GetComponentPtr<Nz::Vector2f>(Nz::VertexComponent::TexCoord))
		{
			if (iMesh->HasTextureCoords(0))
			{
				for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
				{
					aiVector3D uv = iMesh->mTextureCoords[0][vertexIdx];
					*uvPtr++ = parameters.texCoordOffset + Nz::Vector2f(uv.x, uv.y) * parameters.texCoordScale;
				}
			}
			else
			{
				for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
					*uvPtr++ = Nz::Vector2f::Zero();
			}
		}

		// Vertex colors
		if (auto colorPtr = vertexMapper.GetComponentPtr<Nz::Color>(Nz::VertexComponent::Color))
		{
			if (iMesh->HasVertexColors(0))
			{
				for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
				{
					aiColor4D color = iMesh->mColors[0][vertexIdx];
					*colorPtr++ = Nz::Color(color.r, color.g, color.b, color.a);
				}
			}
			else
			{
				for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
					*colorPtr++ = Nz::Color::White;
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
		std::shared_ptr<Nz::SkeletalMesh> subMesh = std::make_shared<Nz::SkeletalMesh>(vertexBuffer, indexBuffer);
		subMesh->SetMaterialIndex(iMesh->mMaterialIndex);

		auto matIt = materials.find(iMesh->mMaterialIndex);
		if (matIt == materials.end())
		{
			Nz::ParameterList matData;
			aiMaterial* aiMat = scene->mMaterials[iMesh->mMaterialIndex];

			auto ConvertColor = [&](const char* aiKey, unsigned int aiType, unsigned int aiIndex, const char* colorKey)
			{
				aiColor4D color;
				if (aiGetMaterialColor(aiMat, aiKey, aiType, aiIndex, &color) == aiReturn_SUCCESS)
				{
					matData.SetParameter(colorKey, Nz::Color(color.r, color.g, color.b, color.a));
				}
			};

			auto ConvertTexture = [&](aiTextureType aiType, const char* textureKey, const char* wrapKey = nullptr)
			{
				aiString path;
				aiTextureMapMode mapMode[3];
				if (aiGetMaterialTexture(aiMat, aiType, 0, &path, nullptr, nullptr, nullptr, nullptr, &mapMode[0], nullptr) == aiReturn_SUCCESS)
				{
					matData.SetParameter(textureKey, (stream.GetDirectory() / std::string_view(path.data, path.length)).generic_u8string());

					if (wrapKey)
					{
						Nz::SamplerWrap wrap = Nz::SamplerWrap::Clamp;
						switch (mapMode[0])
						{
							case aiTextureMapMode_Clamp:
							case aiTextureMapMode_Decal:
								wrap = Nz::SamplerWrap::Clamp;
								break;

							case aiTextureMapMode_Mirror:
								wrap = Nz::SamplerWrap::MirroredRepeat;
								break;

							case aiTextureMapMode_Wrap:
								wrap = Nz::SamplerWrap::Repeat;
								break;

							default:
								NazaraWarning("Assimp texture map mode 0x" + Nz::NumberToString(mapMode[0], 16) + " not handled");
								break;
						}

						matData.SetParameter(wrapKey, static_cast<long long>(wrap));
					}
				}
			};

			ConvertColor(AI_MATKEY_COLOR_AMBIENT, Nz::MaterialData::AmbientColor);
			ConvertColor(AI_MATKEY_COLOR_DIFFUSE, Nz::MaterialData::DiffuseColor);
			ConvertColor(AI_MATKEY_COLOR_SPECULAR, Nz::MaterialData::SpecularColor);

			ConvertTexture(aiTextureType_DIFFUSE, Nz::MaterialData::DiffuseTexturePath, Nz::MaterialData::DiffuseWrap);
			ConvertTexture(aiTextureType_EMISSIVE, Nz::MaterialData::EmissiveTexturePath);
			ConvertTexture(aiTextureType_HEIGHT, Nz::MaterialData::HeightTexturePath);
			ConvertTexture(aiTextureType_NORMALS, Nz::MaterialData::NormalTexturePath);
			ConvertTexture(aiTextureType_OPACITY, Nz::MaterialData::AlphaTexturePath);
			ConvertTexture(aiTextureType_SPECULAR, Nz::MaterialData::SpecularTexturePath, Nz::MaterialData::SpecularWrap);

			aiString name;
			if (aiGetMaterialString(aiMat, AI_MATKEY_NAME, &name) == aiReturn_SUCCESS)
				matData.SetParameter(Nz::MaterialData::Name, std::string(name.data, name.length));

			int iValue;
			if (aiGetMaterialInteger(aiMat, AI_MATKEY_TWOSIDED, &iValue) == aiReturn_SUCCESS)
				matData.SetParameter(Nz::MaterialData::FaceCulling, !iValue);

			matIt = materials.insert(std::make_pair(iMesh->mMaterialIndex, std::make_pair(Nz::UInt32(materials.size()), std::move(matData)))).first;
		}

		subMesh->SetMaterialIndex(matIt->first);

		mesh->AddSubMesh(subMesh);

		mesh->SetMaterialCount(std::max<Nz::UInt32>(Nz::SafeCast<Nz::UInt32>(materials.size()), 1));
		for (const auto& pair : materials)
			mesh->SetMaterialData(pair.second.first, pair.second.second);
	}
	else
	{
		mesh->CreateStatic();

		// aiMaterial index in scene => Material index and data in Mesh
		std::unordered_map<unsigned int, std::pair<Nz::UInt32, Nz::ParameterList>> materials;

		for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
		{
			aiMesh* iMesh = scene->mMeshes[meshIndex];

			unsigned int indexCount = iMesh->mNumFaces * 3;
			unsigned int vertexCount = iMesh->mNumVertices;

			// Index buffer
			bool largeIndices = (vertexCount > std::numeric_limits<Nz::UInt16>::max());

			std::shared_ptr<Nz::IndexBuffer> indexBuffer = std::make_shared<Nz::IndexBuffer>((largeIndices) ? Nz::IndexType::U32 : Nz::IndexType::U16, indexCount, parameters.indexBufferFlags, parameters.bufferFactory);

			Nz::IndexMapper indexMapper(*indexBuffer);
			Nz::IndexIterator index = indexMapper.begin();

			for (unsigned int faceIndex = 0; faceIndex < iMesh->mNumFaces; ++faceIndex)
			{
				const aiFace& face = iMesh->mFaces[faceIndex];
				if (face.mNumIndices != 3)
					NazaraWarning("Assimp plugin: This face is not a triangle!");

				// Index buffer
				*index++ = face.mIndices[0];
				*index++ = face.mIndices[1];
				*index++ = face.mIndices[2];

				indexMapper.Unmap();
			}

			// Vertex buffer

			// Make sure the normal/tangent matrix won't rescale our vectors
			Nz::Matrix4f normalTangentMatrix = parameters.matrix;
			if (normalTangentMatrix.HasScale())
				normalTangentMatrix.ApplyScale(1.f / normalTangentMatrix.GetScale());

			std::shared_ptr<Nz::VertexBuffer> vertexBuffer = std::make_shared<Nz::VertexBuffer>(parameters.vertexDeclaration, vertexCount, parameters.vertexBufferFlags, parameters.bufferFactory);

			Nz::VertexMapper vertexMapper(*vertexBuffer);

			// Vertex positions
			if (auto posPtr = vertexMapper.GetComponentPtr<Nz::Vector3f>(Nz::VertexComponent::Position))
			{
				for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
				{
					aiVector3D position = iMesh->mVertices[vertexIdx];
					*posPtr++ = parameters.matrix * Nz::Vector3f(position.x, position.y, position.z);
				}
			}

			// Vertex normals
			if (auto normalPtr = vertexMapper.GetComponentPtr<Nz::Vector3f>(Nz::VertexComponent::Normal))
			{
				for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
				{
					aiVector3D normal = iMesh->mNormals[vertexIdx];
					*normalPtr++ = normalTangentMatrix.Transform({normal.x, normal.y, normal.z}, 0.f);
				}
			}

			// Vertex tangents
			bool generateTangents = false;
			if (auto tangentPtr = vertexMapper.GetComponentPtr<Nz::Vector3f>(Nz::VertexComponent::Tangent))
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
			if (auto uvPtr = vertexMapper.GetComponentPtr<Nz::Vector2f>(Nz::VertexComponent::TexCoord))
			{
				if (iMesh->HasTextureCoords(0))
				{
					for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
					{
						aiVector3D uv = iMesh->mTextureCoords[0][vertexIdx];
						*uvPtr++ = parameters.texCoordOffset + Nz::Vector2f(uv.x, uv.y) * parameters.texCoordScale;
					}
				}
				else
				{
					for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
						*uvPtr++ = Nz::Vector2f::Zero();
				}
			}

			// Vertex colors
			if (auto colorPtr = vertexMapper.GetComponentPtr<Nz::Color>(Nz::VertexComponent::Color))
			{
				if (iMesh->HasVertexColors(0))
				{
					for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
					{
						aiColor4D color = iMesh->mColors[0][vertexIdx];
						*colorPtr++ = Nz::Color(color.r, color.g, color.b, color.a);
					}
				}
				else
				{
					for (unsigned int vertexIdx = 0; vertexIdx < vertexCount; ++vertexIdx)
						*colorPtr++ = Nz::Color::White;
				}
			}

			vertexMapper.Unmap();

			// Submesh
			std::shared_ptr<Nz::StaticMesh> subMesh = std::make_shared<Nz::StaticMesh>(vertexBuffer, indexBuffer);
			subMesh->GenerateAABB();
			subMesh->SetMaterialIndex(iMesh->mMaterialIndex);

			if (generateTangents)
				subMesh->GenerateTangents();

			auto matIt = materials.find(iMesh->mMaterialIndex);
			if (matIt == materials.end())
			{
				Nz::ParameterList matData;
				aiMaterial* aiMat = scene->mMaterials[iMesh->mMaterialIndex];

				auto ConvertColor = [&] (const char* aiKey, unsigned int aiType, unsigned int aiIndex, const char* colorKey)
				{
					aiColor4D color;
					if (aiGetMaterialColor(aiMat, aiKey, aiType, aiIndex, &color) == aiReturn_SUCCESS)
					{
						matData.SetParameter(colorKey, Nz::Color(color.r, color.g, color.b, color.a));
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
							Nz::SamplerWrap wrap = Nz::SamplerWrap::Clamp;
							switch (mapMode[0])
							{
								case aiTextureMapMode_Clamp:
								case aiTextureMapMode_Decal:
									wrap = Nz::SamplerWrap::Clamp;
									break;

								case aiTextureMapMode_Mirror:
									wrap = Nz::SamplerWrap::MirroredRepeat;
									break;

								case aiTextureMapMode_Wrap:
									wrap = Nz::SamplerWrap::Repeat;
									break;

								default:
									NazaraWarning("Assimp texture map mode 0x" + Nz::NumberToString(mapMode[0], 16) + " not handled");
									break;
							}

							matData.SetParameter(wrapKey, static_cast<long long>(wrap));
						}
					}
				};

				ConvertColor(AI_MATKEY_COLOR_AMBIENT, Nz::MaterialData::AmbientColor);
				ConvertColor(AI_MATKEY_COLOR_DIFFUSE, Nz::MaterialData::DiffuseColor);
				ConvertColor(AI_MATKEY_COLOR_SPECULAR, Nz::MaterialData::SpecularColor);

				ConvertTexture(aiTextureType_DIFFUSE, Nz::MaterialData::DiffuseTexturePath, Nz::MaterialData::DiffuseWrap);
				ConvertTexture(aiTextureType_EMISSIVE, Nz::MaterialData::EmissiveTexturePath);
				ConvertTexture(aiTextureType_HEIGHT, Nz::MaterialData::HeightTexturePath);
				ConvertTexture(aiTextureType_NORMALS, Nz::MaterialData::NormalTexturePath);
				ConvertTexture(aiTextureType_OPACITY, Nz::MaterialData::AlphaTexturePath);
				ConvertTexture(aiTextureType_SPECULAR, Nz::MaterialData::SpecularTexturePath, Nz::MaterialData::SpecularWrap);

				aiString name;
				if (aiGetMaterialString(aiMat, AI_MATKEY_NAME, &name) == aiReturn_SUCCESS)
					matData.SetParameter(Nz::MaterialData::Name, std::string(name.data, name.length));

				int iValue;
				if (aiGetMaterialInteger(aiMat, AI_MATKEY_TWOSIDED, &iValue) == aiReturn_SUCCESS)
					matData.SetParameter(Nz::MaterialData::FaceCulling, !iValue);

				matIt = materials.insert(std::make_pair(iMesh->mMaterialIndex, std::make_pair(Nz::UInt32(materials.size()), std::move(matData)))).first;
			}

			subMesh->SetMaterialIndex(matIt->first);

			mesh->AddSubMesh(subMesh);

			mesh->SetMaterialCount(std::max<Nz::UInt32>(Nz::UInt32(materials.size()), 1));
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
