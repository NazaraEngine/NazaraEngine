/*
Nazara Engine - Assimp Plugin

Copyright (C) 2022 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)

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
#include <NazaraUtils/Bitset.hpp>
#include <NazaraUtils/CallOnExit.hpp>
#include <NazaraUtils/StringHash.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Animation.hpp>
#include <Nazara/Core/Mesh.hpp>
#include <Nazara/Core/Image.hpp>
#include <Nazara/Core/IndexIterator.hpp>
#include <Nazara/Core/IndexMapper.hpp>
#include <Nazara/Core/Joint.hpp>
#include <Nazara/Core/MaterialData.hpp>
#include <Nazara/Core/PixelFormat.hpp>
#include <Nazara/Core/SkeletalMesh.hpp>
#include <Nazara/Core/Skeleton.hpp>
#include <Nazara/Core/StaticMesh.hpp>
#include <Nazara/Core/VertexMapper.hpp>
#include <Nazara/Core/Core.hpp>
#include <Nazara/Core/Plugins/AssimpPlugin.hpp>
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

constexpr unsigned int AssimpFlags = aiProcess_CalcTangentSpace      | aiProcess_FixInfacingNormals
                                   | aiProcess_FlipUVs               | aiProcess_GenSmoothNormals   | aiProcess_GenUVCoords
                                   | aiProcess_JoinIdenticalVertices | aiProcess_RemoveComponent    | aiProcess_SortByPType
                                   | aiProcess_TransformUVCoords     | aiProcess_Triangulate;

Nz::Color FromAssimp(const aiColor4D& color)
{
	return Nz::Color(color.r, color.g, color.b, color.a);
}

Nz::Vector3f FromAssimp(const aiVector3D& vec)
{
	return Nz::Vector3f(vec.x, vec.y, vec.z);
}

Nz::Quaternionf FromAssimp(const aiQuaternion& quat)
{
	return Nz::Quaternionf(quat.w, quat.x, quat.y, quat.z);
}

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
		std::unordered_map<std::string, unsigned int, Nz::StringHash<>, std::equal_to<>> bones;
	};

	struct StaticMesh
	{
		const aiMesh* mesh;
		std::size_t nodeIndex;
	};

	std::size_t skeletonRootIndex;
	std::unordered_map<const aiBone*, unsigned int> assimpBoneToJointIndex;
	std::unordered_multimap<std::string, std::size_t, Nz::StringHash<>, std::equal_to<>> nodeByName;
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

bool FindSkeletonRoot(SceneInfo& sceneInfo, const aiNode* node)
{
	for (auto& skeletalMesh : sceneInfo.skeletalMeshes)
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
				NazaraError("failed to identify skeleton root node: {0} node(s) matched", std::distance(range.first, range.second));
				return false;
			}

			sceneInfo.skeletonRootIndex = range.first->second;
			return true;
		}
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		if (FindSkeletonRoot(sceneInfo, node->mChildren[i]))
			return true;
	}

	return false;
}

void ProcessJoints(const Nz::MeshParams& parameters, const Nz::Matrix4f& transformMatrix, const Nz::Matrix4f& invTransformMatrix, SceneInfo::SkeletalMesh& skeletalMesh, Nz::Skeleton* skeleton, const aiNode* node, unsigned int& nextJointIndex, std::unordered_map<const aiBone*, unsigned int>& boneToJointIndex, std::unordered_set<const aiNode*>& seenNodes)
{
	Nz::Joint* joint;
	unsigned int currentJointIndex;

	if (seenNodes.find(node) != seenNodes.end())
	{
		currentJointIndex = Nz::SafeCast<unsigned int>(skeleton->GetJointIndex(node->mName.C_Str()));
		joint = skeleton->GetJoint(currentJointIndex);
	}
	else
	{
		seenNodes.insert(node);

		currentJointIndex = nextJointIndex++;

		joint = skeleton->GetJoint(currentJointIndex);
		joint->SetName(node->mName.C_Str());
		joint->SetInverseBindMatrix(Nz::Matrix4f::Identity());

		aiQuaternion rotation;
		aiVector3D position;
		aiVector3D scaling;

		node->mTransformation.Decompose(scaling, rotation, position);

		if (currentJointIndex == 0)
		{
			// Root joint gets transformations
			joint->SetPosition(Nz::TransformPositionSRT(parameters.vertexOffset, parameters.vertexRotation, parameters.vertexScale, FromAssimp(position)));
			joint->SetRotation(Nz::TransformRotationSRT(parameters.vertexRotation, parameters.vertexScale, FromAssimp(rotation)));
			joint->SetScale(Nz::TransformScaleSRT(parameters.vertexScale, FromAssimp(scaling)));
		}
		else
		{
			joint->SetPosition(Nz::TransformPositionSRT({}, Nz::Quaternionf::Identity(), parameters.vertexScale, FromAssimp(position)));
			joint->SetRotation(FromAssimp(rotation));
			joint->SetScale(FromAssimp(scaling));
		}

		if (currentJointIndex != 0)
			joint->SetParent(skeleton->GetJoint(node->mParent->mName.C_Str()));
	}

	if (auto it = skeletalMesh.bones.find(node->mName.C_Str()); it != skeletalMesh.bones.end())
	{
		const aiBone* bone = skeletalMesh.mesh->mBones[it->second];
		boneToJointIndex.emplace(bone, currentJointIndex);

		Nz::Matrix4f offsetMatrix(bone->mOffsetMatrix.a1, bone->mOffsetMatrix.b1, bone->mOffsetMatrix.c1, bone->mOffsetMatrix.d1,
		                          bone->mOffsetMatrix.a2, bone->mOffsetMatrix.b2, bone->mOffsetMatrix.c2, bone->mOffsetMatrix.d2,
		                          bone->mOffsetMatrix.a3, bone->mOffsetMatrix.b3, bone->mOffsetMatrix.c3, bone->mOffsetMatrix.d3,
		                          bone->mOffsetMatrix.a4, bone->mOffsetMatrix.b4, bone->mOffsetMatrix.c4, bone->mOffsetMatrix.d4);

		joint->SetInverseBindMatrix(Nz::Matrix4f::ConcatenateTransform(Nz::Matrix4f::ConcatenateTransform(invTransformMatrix, offsetMatrix), transformMatrix));
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
		ProcessJoints(parameters, transformMatrix, invTransformMatrix, skeletalMesh, skeleton, node->mChildren[i], nextJointIndex, boneToJointIndex, seenNodes);
}

bool IsSupported(std::string_view extension)
{
	return (aiIsExtensionSupported(extension.data()) == AI_TRUE);
}

/************************************************************************/
/*                           Material loading                           */
/************************************************************************/

Nz::Result<std::shared_ptr<Nz::Animation>, Nz::ResourceLoadingError> LoadAnimation(Nz::Stream& stream, const Nz::AnimationParams& parameters)
{
	NazaraAssertMsg(parameters.IsValid(), "invalid animation parameters");

	std::string streamPath = Nz::PathToString(stream.GetPath());

	FileIOUserdata userdata;
	userdata.originalFilePath = (!streamPath.empty()) ? streamPath.data() : StreamPath;
	userdata.originalStream = &stream;

	aiFileIO fileIO;
	fileIO.CloseProc = StreamCloser;
	fileIO.OpenProc = StreamOpener;
	fileIO.UserData = reinterpret_cast<char*>(&userdata);

	const aiScene* scene = aiImportFileEx(userdata.originalFilePath, AssimpFlags, &fileIO);
	Nz::CallOnExit releaseScene([&] { aiReleaseImport(scene); });

	if (!scene)
	{
		NazaraError("Assimp failed to import file: {0}", aiGetErrorString());
		return Nz::Err(Nz::ResourceLoadingError::DecodingError);
	}

	if (!scene->HasAnimations())
	{
		NazaraError("File has no animation");
		return Nz::Err(Nz::ResourceLoadingError::DecodingError);
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

	Nz::Animation::Sequence sequence;
	sequence.firstFrame = 0;
	sequence.frameCount = maxFrameCount;
	sequence.frameRate = static_cast<Nz::UInt32>((animation->mTicksPerSecond != 0.0) ? animation->mTicksPerSecond : 24.0);

	anim->AddSequence(sequence);

	Nz::Bitset<> identityJoints(parameters.skeleton->GetJointCount(), true);

	for (unsigned int i = 0; i < animation->mNumChannels; ++i)
	{
		const aiNodeAnim* nodeAnim = animation->mChannels[i];

		std::size_t jointIndex = parameters.skeleton->GetJointIndex(nodeAnim->mNodeName.C_Str());
		if (jointIndex == Nz::Skeleton::InvalidJointIndex)
		{
			NazaraError("animation references joint {0} which is not part of the skeleton", nodeAnim->mNodeName.C_Str());
			continue;
		}

		identityJoints.Set(jointIndex, false);

		// First key time is assumed to be 0
		unsigned int currentPosKey = 0;
		unsigned int currentRotKey = 0;
		unsigned int currentScaleKey = 0;

		for (unsigned int frameIndex = 0; frameIndex < maxFrameCount; ++frameIndex)
		{
			Nz::Animation::SequenceJoint* sequenceJoints = anim->GetSequenceJoints(frameIndex);

			double frameTime = frameIndex * animation->mDuration / maxFrameCount;

			auto HandleAnimAdvance = [frameTime](unsigned int& currentKey, unsigned int& nextKey, unsigned int numKeys, const auto& keys) -> float
			{
				float delta = 0.f;
				while (currentKey + 1 < numKeys && keys[currentKey + 1].mTime < frameTime)
					currentKey++;

				nextKey = currentKey + 1;

				if (nextKey < numKeys)
					delta = static_cast<float>((frameTime - keys[currentKey].mTime) / (keys[currentKey + 1].mTime - keys[currentKey].mTime));
				else
				{
					nextKey = currentKey;
					delta = 1.f;
				}

				return delta;
			};

			unsigned int nextPosKey, nextRotKey, nextScaleKey;

			float posDelta   = HandleAnimAdvance(currentPosKey,   nextPosKey,   nodeAnim->mNumPositionKeys, nodeAnim->mPositionKeys);
			float rotDelta   = HandleAnimAdvance(currentRotKey,   nextRotKey,   nodeAnim->mNumRotationKeys, nodeAnim->mRotationKeys);
			float scaleDelta = HandleAnimAdvance(currentScaleKey, nextScaleKey, nodeAnim->mNumScalingKeys,  nodeAnim->mScalingKeys);

			Nz::Vector3f interpolatedPosition    = Nz::Vector3f::Lerp(FromAssimp(nodeAnim->mPositionKeys[currentPosKey].mValue), FromAssimp(nodeAnim->mPositionKeys[nextPosKey].mValue), posDelta);
			Nz::Quaternionf interpolatedRotation = Nz::Quaternionf::Slerp(FromAssimp(nodeAnim->mRotationKeys[currentRotKey].mValue), FromAssimp(nodeAnim->mRotationKeys[nextRotKey].mValue), rotDelta).Normalize();
			Nz::Vector3f interpolatedScale       = Nz::Vector3f::Lerp(FromAssimp(nodeAnim->mScalingKeys[currentScaleKey].mValue), FromAssimp(nodeAnim->mScalingKeys[nextScaleKey].mValue), scaleDelta);

			if (jointIndex == 0)
			{
				sequenceJoints[jointIndex].position = Nz::TransformPositionSRT(parameters.jointOffset, parameters.jointRotation, parameters.jointScale, interpolatedPosition);
				sequenceJoints[jointIndex].rotation = Nz::TransformRotationSRT(parameters.jointRotation, parameters.jointScale, interpolatedRotation);
				sequenceJoints[jointIndex].scale = Nz::TransformScaleSRT(parameters.jointScale, interpolatedScale);
			}
			else
			{
				sequenceJoints[jointIndex].position = parameters.jointScale * interpolatedPosition;
				sequenceJoints[jointIndex].rotation = interpolatedRotation;
				sequenceJoints[jointIndex].scale = interpolatedScale;
			}
		}
	}

	for (std::size_t jointIndex : identityJoints.IterBits())
	{
		const Nz::Joint* joint = parameters.skeleton->GetJoint(jointIndex);

		for (unsigned int frameIndex = 0; frameIndex < maxFrameCount; ++frameIndex)
		{
			Nz::Animation::SequenceJoint* sequenceJoints = anim->GetSequenceJoints(frameIndex);
			sequenceJoints[jointIndex].position = joint->GetPosition();
			sequenceJoints[jointIndex].rotation = joint->GetRotation();
			sequenceJoints[jointIndex].scale    = joint->GetScale();
		}
	}

	return anim;
}

/************************************************************************/
/*                             Mesh loading                             */
/************************************************************************/

using EmbeddedTextures = std::unordered_map<const aiTexture*, std::filesystem::path>;
using MaterialData = std::unordered_map<unsigned int, std::pair<Nz::UInt32, Nz::ParameterList>>;

std::shared_ptr<Nz::SubMesh> ProcessSubMesh(const std::filesystem::path& originPath, const Nz::MeshParams& parameters, const aiScene* scene, const aiMesh* meshData, bool isSkeletalMesh, MaterialData& materialData, const std::unordered_map<const aiBone*, unsigned int>& boneToJointIndex, EmbeddedTextures& embeddedTextures)
{
	unsigned int indexCount = meshData->mNumFaces * 3;
	unsigned int vertexCount = meshData->mNumVertices;

	// Index buffer
	bool largeIndices = (vertexCount > std::numeric_limits<Nz::UInt16>::max());

	std::shared_ptr<Nz::IndexBuffer> indexBuffer = std::make_shared<Nz::IndexBuffer>((largeIndices) ? Nz::IndexType::U32 : Nz::IndexType::U16, indexCount, parameters.indexBufferFlags, parameters.bufferFactory);

	Nz::IndexMapper indexMapper(*indexBuffer);
	Nz::IndexIterator index = indexMapper.begin();

	if (parameters.reverseWinding)
	{
		for (unsigned int faceIndex = 0; faceIndex < meshData->mNumFaces; ++faceIndex)
		{
			const aiFace& face = meshData->mFaces[faceIndex];
			if (face.mNumIndices != 3)
				NazaraWarning("Assimp plugin: This face is not a triangle!");

			*index++ = face.mIndices[0];
			*index++ = face.mIndices[2];
			*index++ = face.mIndices[1];
		}
	}
	else
	{
		for (unsigned int faceIndex = 0; faceIndex < meshData->mNumFaces; ++faceIndex)
		{
			const aiFace& face = meshData->mFaces[faceIndex];
			if (face.mNumIndices != 3)
				NazaraWarning("Assimp plugin: This face is not a triangle!");

			*index++ = face.mIndices[0];
			*index++ = face.mIndices[1];
			*index++ = face.mIndices[2];
		}
	}

	indexMapper.Unmap();

	std::shared_ptr<Nz::VertexBuffer> vertexBuffer = std::make_shared<Nz::VertexBuffer>(parameters.vertexDeclaration, vertexCount, parameters.vertexBufferFlags, parameters.bufferFactory);

	Nz::VertexMapper vertexMapper(*vertexBuffer);

	Nz::Boxf aabb = Nz::Boxf::Zero();

	// Vertex positions
	if (auto posPtr = vertexMapper.GetComponentPtr<Nz::Vector3f>(Nz::VertexComponent::Position))
	{
		for (unsigned int vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
			posPtr[vertexIndex] = Nz::TransformPositionSRT(parameters.vertexOffset, parameters.vertexRotation, parameters.vertexScale, FromAssimp(meshData->mVertices[vertexIndex]));

		aabb = Nz::ComputeAABB(posPtr, vertexCount);
	}

	// Vertex normals
	if (auto normalPtr = vertexMapper.GetComponentPtr<Nz::Vector3f>(Nz::VertexComponent::Normal))
	{
		for (unsigned int vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
			*normalPtr++ = Nz::TransformDirectionSRT(parameters.vertexRotation, parameters.vertexScale, FromAssimp(meshData->mNormals[vertexIndex]));
	}

	// Vertex tangents
	bool generateTangents = false;
	if (auto tangentPtr = vertexMapper.GetComponentPtr<Nz::Vector3f>(Nz::VertexComponent::Tangent))
	{
		if (meshData->HasTangentsAndBitangents())
		{
			for (unsigned int vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
				*tangentPtr++ = Nz::TransformDirectionSRT(parameters.vertexRotation, parameters.vertexScale, FromAssimp(meshData->mTangents[vertexIndex]));
		}
		else
			generateTangents = true;
	}

	// Vertex UVs
	if (auto uvPtr = vertexMapper.GetComponentPtr<Nz::Vector2f>(Nz::VertexComponent::TexCoord))
	{
		if (meshData->HasTextureCoords(0))
		{
			for (unsigned int vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
			{
				const aiVector3D& uv = meshData->mTextureCoords[0][vertexIndex];
				*uvPtr++ = parameters.texCoordOffset + Nz::Vector2f(uv.x, uv.y) * parameters.texCoordScale;
			}
		}
		else
		{
			for (unsigned int vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
				*uvPtr++ = Nz::Vector2f::Zero();
		}
	}

	// Vertex colors
	if (auto colorPtr = vertexMapper.GetComponentPtr<Nz::Color>(Nz::VertexComponent::Color))
	{
		if (meshData->HasVertexColors(0))
		{
			for (unsigned int vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
				*colorPtr++ = FromAssimp(meshData->mColors[0][vertexIndex]);
		}
		else
		{
			for (unsigned int vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
				*colorPtr++ = Nz::Color::White();
		}
	}

	if (isSkeletalMesh)
	{
		auto jointIndicesPtr = vertexMapper.GetComponentPtr<Nz::Vector4i32>(Nz::VertexComponent::JointIndices);
		auto jointWeightPtr = vertexMapper.GetComponentPtr<Nz::Vector4f>(Nz::VertexComponent::JointWeights);

		if (jointIndicesPtr || jointWeightPtr)
		{
			constexpr std::size_t MaxJointPerVertex = 4;

			struct VertexJoint
			{
				Nz::Int32 jointIndex;
				float weight = 0.f;
			};

			// Use temporary vector to re-normalize if needed
			std::vector<std::vector<VertexJoint>> weightIndices(vertexCount);

			for (unsigned int boneIndex = 0; boneIndex < meshData->mNumBones; ++boneIndex)
			{
				const aiBone* bone = meshData->mBones[boneIndex];

				auto it = boneToJointIndex.find(bone);
				if (it == boneToJointIndex.end())
				{
					// Some nodes are not attached to vertices but may influence other nodes or serve as attachment points
					assert(bone->mNumWeights == 0);
					continue;
				}

				unsigned int jointIndex = it->second;

				for (unsigned int weightIndex = 0; weightIndex < bone->mNumWeights; ++weightIndex)
				{
					const aiVertexWeight& vertexWeight = bone->mWeights[weightIndex];

					VertexJoint& vertexJoint = weightIndices[vertexWeight.mVertexId].emplace_back();
					vertexJoint.jointIndex = jointIndex;
					vertexJoint.weight = vertexWeight.mWeight;
				}
			}

			for (auto& indices : weightIndices)
			{
				if (indices.size() > MaxJointPerVertex)
				{
					std::sort(indices.begin(), indices.end(), [](const VertexJoint& lhs, const VertexJoint& rhs)
					{
						return lhs.weight > rhs.weight;
					});

					float invTotalWeight = 0.f;
					for (unsigned int i = 0; i < MaxJointPerVertex; ++i)
						invTotalWeight += indices[i].weight;

					invTotalWeight = 1.f / invTotalWeight;

					for (std::size_t i = 0; i < MaxJointPerVertex; ++i)
						indices[i].weight *= invTotalWeight;
				}

				// Always use MaxJointPerVertex indices
				indices.resize(MaxJointPerVertex);
			}

			for (unsigned int vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
			{
				for (unsigned int i = 0; i < MaxJointPerVertex; ++i)
				{
					const VertexJoint& vertexJoint = weightIndices[vertexIndex][i];

					if (jointIndicesPtr)
						jointIndicesPtr[vertexIndex][i] = vertexJoint.jointIndex;

					if (jointWeightPtr)
						jointWeightPtr[vertexIndex][i] = vertexJoint.weight;
				}
			}
		}
	}

	vertexMapper.Unmap();

	// Submesh
	std::shared_ptr<Nz::SubMesh> subMesh;
	if (isSkeletalMesh)
	{
		std::shared_ptr<Nz::SkeletalMesh> skeletalMesh = std::make_shared<Nz::SkeletalMesh>(std::move(vertexBuffer), std::move(indexBuffer));
		skeletalMesh->SetAABB(aabb);

		subMesh = std::move(skeletalMesh);
	}
	else
	{
		std::shared_ptr<Nz::StaticMesh> staticMesh = std::make_shared<Nz::StaticMesh>(std::move(vertexBuffer), std::move(indexBuffer));
		staticMesh->SetAABB(aabb);

		subMesh = std::move(staticMesh);
	}

	if (generateTangents)
		subMesh->GenerateTangents();

	subMesh->SetMaterialIndex(meshData->mMaterialIndex);

	auto matIt = materialData.find(meshData->mMaterialIndex);
	if (matIt == materialData.end())
	{
		Nz::ParameterList matData;
		const aiMaterial* aiMat = scene->mMaterials[meshData->mMaterialIndex];

		auto ConvertColor = [&] (const char* aiKey, unsigned int aiType, unsigned int aiIndex, const char* colorKey)
		{
			aiColor4D color;
			if (aiGetMaterialColor(aiMat, aiKey, aiType, aiIndex, &color) == aiReturn_SUCCESS)
			{
				matData.SetParameter(colorKey, Nz::Color(color.r, color.g, color.b, color.a));
				return true;
			}

			return false;
		};

		auto SaveEmbeddedTextureToFile = [](const aiTexture* embeddedTexture, const std::filesystem::path& basePath, const char* filename) -> std::filesystem::path
		{
			if (basePath.empty())
			{
				NazaraError("can't create embedded resource folder (empty base path)");
				return {};
			}

			std::filesystem::path targetPath = basePath / "embedded";

			if (!std::filesystem::is_directory(targetPath))
			{
				if (!std::filesystem::create_directory(targetPath))
				{
					NazaraError("can't create embedded resource folder (folder creation failed)");
					return {};
				}
			}

			targetPath /= Nz::Utf8Path(filename);

			if (embeddedTexture->mHeight == 0)
			{
				// Compressed data (PNG, JPG, etc.)
				if (!embeddedTexture->achFormatHint[0])
				{
					NazaraError("can't create embedded texture file (no format hint)");
					return {};
				}

				targetPath.replace_extension(Nz::Utf8Path(embeddedTexture->achFormatHint));

				if (!Nz::File::WriteWhole(targetPath, embeddedTexture->pcData, embeddedTexture->mWidth))
					return {};

				return targetPath;
			}
			else
			{
				// Uncompressed data (always ARGB8 it seems)
				Nz::Image uncompressedData(Nz::ImageType::E2D, Nz::PixelFormat::RGBA8_SRGB, embeddedTexture->mWidth, embeddedTexture->mHeight);
				const aiTexel* sourceData = embeddedTexture->pcData;
				Nz::UInt8* imageData = uncompressedData.GetPixels();
				for (unsigned int y = 0; y < embeddedTexture->mHeight; ++y)
				{
					for (unsigned int x = 0; x < embeddedTexture->mWidth; ++x)
					{
						*imageData++ = sourceData->r;
						*imageData++ = sourceData->g;
						*imageData++ = sourceData->b;
						*imageData++ = sourceData->a;

						++sourceData;
					}
				}

				// Compress to PNG
				targetPath.replace_extension(".png");

				if (!uncompressedData.SaveToFile(targetPath))
					return {};

				return targetPath;
			}
		};

		auto ConvertTexture = [&] (aiTextureType aiType, const char* textureKey, const char* wrapKey = nullptr)
		{
			aiString path;
			aiTextureMapMode mapMode[3];
			if (aiGetMaterialTexture(aiMat, aiType, 0, &path, nullptr, nullptr, nullptr, nullptr, &mapMode[0], nullptr) == aiReturn_SUCCESS)
			{
				if (const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(path.C_Str()))
				{
					std::filesystem::path embeddedTexturePath;
					if (auto it = embeddedTextures.find(embeddedTexture); it == embeddedTextures.end())
					{
						embeddedTexturePath = SaveEmbeddedTextureToFile(embeddedTexture, originPath, aiScene::GetShortFilename(path.C_Str()));
						if (embeddedTexturePath.empty())
							NazaraError("failed to save embedded texture to file");

						embeddedTextures.emplace(embeddedTexture, embeddedTexturePath);
					}
					else
						embeddedTexturePath = it->second;

					matData.SetParameter(textureKey, Nz::PathToString(embeddedTexturePath));
				}
				else
					matData.SetParameter(textureKey, Nz::PathToString((originPath / Nz::Utf8Path(std::string_view(path.data, path.length)))));

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
							NazaraWarning("Assimp texture map mode {0:#x} not handled", Nz::UnderlyingCast(mapMode[0]));
							break;
					}

					matData.SetParameter(wrapKey, static_cast<long long>(wrap));
				}

				return true;
			}

			return false;
		};

		ConvertColor(AI_MATKEY_COLOR_AMBIENT,  Nz::MaterialData::AmbientColor);

		if (!ConvertColor(AI_MATKEY_BASE_COLOR, Nz::MaterialData::BaseColor))
			ConvertColor(AI_MATKEY_COLOR_DIFFUSE, Nz::MaterialData::BaseColor);

		ConvertColor(AI_MATKEY_COLOR_SPECULAR, Nz::MaterialData::SpecularColor);

		if (!ConvertTexture(aiTextureType_BASE_COLOR, Nz::MaterialData::BaseColorTexturePath, Nz::MaterialData::BaseColorTextureWrap))
			ConvertTexture(aiTextureType_DIFFUSE, Nz::MaterialData::BaseColorTexturePath, Nz::MaterialData::BaseColorTextureWrap);

		ConvertTexture(aiTextureType_DIFFUSE_ROUGHNESS, Nz::MaterialData::RoughnessTexturePath, Nz::MaterialData::RoughnessTextureWrap);
		ConvertTexture(aiTextureType_EMISSIVE,          Nz::MaterialData::EmissiveTexturePath,  Nz::MaterialData::EmissiveTextureWrap);
		ConvertTexture(aiTextureType_HEIGHT,            Nz::MaterialData::HeightTexturePath,    Nz::MaterialData::HeightTextureWrap);
		ConvertTexture(aiTextureType_METALNESS,         Nz::MaterialData::MetallicTexturePath,  Nz::MaterialData::MetallicTextureWrap);
		ConvertTexture(aiTextureType_NORMALS,           Nz::MaterialData::NormalTexturePath,    Nz::MaterialData::NormalTextureWrap);
		ConvertTexture(aiTextureType_OPACITY,           Nz::MaterialData::AlphaTexturePath,     Nz::MaterialData::AlphaTextureWrap);
		ConvertTexture(aiTextureType_SPECULAR,          Nz::MaterialData::SpecularTexturePath,  Nz::MaterialData::SpecularTextureWrap);

		if (aiString name; aiGetMaterialString(aiMat, AI_MATKEY_NAME, &name) == aiReturn_SUCCESS)
			matData.SetParameter(Nz::MaterialData::Name, std::string(name.data, name.length));

		if (int iValue; aiGetMaterialInteger(aiMat, AI_MATKEY_TWOSIDED, &iValue) == aiReturn_SUCCESS)
			matData.SetParameter(Nz::MaterialData::FaceCulling, !iValue);

		if (int shadingMode; aiGetMaterialInteger(aiMat, AI_MATKEY_SHADING_MODEL, &shadingMode) == aiReturn_SUCCESS)
		{
			switch (shadingMode)
			{
				case aiShadingMode_Flat:
				case aiShadingMode_Gouraud:
				case aiShadingMode_Phong:
				case aiShadingMode_Blinn:
					matData.SetParameter(Nz::MaterialData::Type, "Phong");
					break;

				case aiShadingMode_CookTorrance:
				case aiShadingMode_PBR_BRDF:
					matData.SetParameter(Nz::MaterialData::Type, "PhysicallyBased");
					break;

				case aiShadingMode_NoShading:
				default:
					matData.SetParameter(Nz::MaterialData::Type, "Basic");
					break;
			}
		}
		else
			matData.SetParameter(Nz::MaterialData::Type, "Phong");

		matIt = materialData.insert(std::make_pair(meshData->mMaterialIndex, std::make_pair(Nz::UInt32(materialData.size()), std::move(matData)))).first;
	}

	return subMesh;
}

Nz::Result<std::shared_ptr<Nz::Mesh>, Nz::ResourceLoadingError> LoadMesh(Nz::Stream& stream, const Nz::MeshParams& parameters)
{
	std::string streamPath = Nz::PathToString(stream.GetPath());

	FileIOUserdata userdata;
	userdata.originalFilePath = (!streamPath.empty()) ? streamPath.data() : StreamPath;
	userdata.originalStream = &stream;

	aiFileIO fileIO;
	fileIO.CloseProc = StreamCloser;
	fileIO.OpenProc = StreamOpener;
	fileIO.UserData = reinterpret_cast<char*>(&userdata);

	unsigned int postProcess = AssimpFlags;

	if (parameters.optimizeIndexBuffers)
		postProcess |= aiProcess_ImproveCacheLocality;

	double smoothingAngle = parameters.custom.GetDoubleParameter("AssimpLoader_SmoothingAngle").GetValueOr(80.0);
	long long triangleLimit = parameters.custom.GetIntegerParameter("AssimpLoader_TriangleLimit").GetValueOr(1'000'000);
	long long vertexLimit = parameters.custom.GetIntegerParameter("AssimpLoader_VertexLimit").GetValueOr(1'000'000);

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
	aiSetImportPropertyInteger(properties, AI_CONFIG_PP_SLM_TRIANGLE_LIMIT,      int(triangleLimit));
	aiSetImportPropertyInteger(properties, AI_CONFIG_PP_SLM_VERTEX_LIMIT,        int(vertexLimit));
	aiSetImportPropertyInteger(properties, AI_CONFIG_PP_RVC_FLAGS,               excludedComponents);
	aiSetImportPropertyInteger(properties, AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);

	const aiScene* scene = aiImportFileExWithProperties(userdata.originalFilePath, postProcess, &fileIO, properties);
	Nz::CallOnExit releaseScene([&] { aiReleaseImport(scene); });

	releaseProperties.CallAndReset();

	if (!scene)
	{
		NazaraError("Assimp failed to import file: {0}", aiGetErrorString());
		return Nz::Err(Nz::ResourceLoadingError::DecodingError);
	}

	SceneInfo sceneInfo;
	VisitNodes(sceneInfo, scene, scene->mRootNode);

	bool handleSkeletalMeshes = parameters.animated && !sceneInfo.skeletalMeshes.empty();
	if (handleSkeletalMeshes && !FindSkeletonRoot(sceneInfo, scene->mRootNode))
		return Nz::Err(Nz::ResourceLoadingError::DecodingError);

	std::shared_ptr<Nz::Mesh> mesh = std::make_shared<Nz::Mesh>();

	EmbeddedTextures embeddedTextures;
	MaterialData materialData;

	if (handleSkeletalMeshes)
	{
		auto& skeletalRoot = sceneInfo.nodes[sceneInfo.skeletonRootIndex];

		unsigned int jointIndex = 0;
		std::unordered_set<const aiNode*> seenNodes;

		Nz::Matrix4f transformMatrix = Nz::Matrix4f::Scale(parameters.vertexScale);
		Nz::Matrix4f invTransformMatrix = Nz::Matrix4f::TransformInverse(parameters.vertexOffset, parameters.vertexRotation, parameters.vertexScale);

		mesh->CreateSkeletal(Nz::SafeCast<Nz::UInt32>(skeletalRoot.totalChildrenCount + 1));
		for (auto& skeletalMesh : sceneInfo.skeletalMeshes)
			ProcessJoints(parameters, transformMatrix, invTransformMatrix, skeletalMesh, mesh->GetSkeleton(), sceneInfo.nodes[sceneInfo.skeletonRootIndex].node, jointIndex, sceneInfo.assimpBoneToJointIndex, seenNodes);

		for (auto& skeletalMesh : sceneInfo.skeletalMeshes)
			mesh->AddSubMesh(ProcessSubMesh(stream.GetDirectory(), parameters, scene, skeletalMesh.mesh, true, materialData, sceneInfo.assimpBoneToJointIndex, embeddedTextures));
	}
	else
	{
		mesh->CreateStatic();
		for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
			mesh->AddSubMesh(ProcessSubMesh(stream.GetDirectory(), parameters, scene, scene->mMeshes[meshIndex], false, materialData, {}, embeddedTextures));

		if (parameters.center)
			mesh->Recenter();
	}

	mesh->SetMaterialCount(std::max(Nz::UInt32(materialData.size()), Nz::UInt32(1)));
	for (const auto& pair : materialData)
		mesh->SetMaterialData(pair.second.first, pair.second.second);

	return mesh;
}

namespace
{
	class AssimpPluginImpl final : public Nz::AssimpPlugin
	{
		public:
			bool ActivateImpl() override
			{
				Nz::Core* core = Nz::Core::Instance();
				NazaraAssertMsg(core, "core module is not instancied");

				Nz::AnimationLoader::Entry animationLoaderEntry;
				animationLoaderEntry.extensionSupport = IsSupported;
				animationLoaderEntry.streamLoader = LoadAnimation;
				animationLoaderEntry.parameterFilter = [](const Nz::AnimationParams& parameters)
				{
					if (auto result = parameters.custom.GetBooleanParameter("SkipAssimpLoader"); result.GetValueOr(false))
						return false;

					return true;
				};

				Nz::AnimationLoader& animationLoader = core->GetAnimationLoader();
				m_animationLoaderEntry = animationLoader.RegisterLoader(std::move(animationLoaderEntry));
				
				Nz::MeshLoader::Entry meshLoaderEntry;
				meshLoaderEntry.extensionSupport = IsSupported;
				meshLoaderEntry.streamLoader = LoadMesh;
				meshLoaderEntry.parameterFilter = [](const Nz::MeshParams& parameters)
				{
					if (auto result = parameters.custom.GetBooleanParameter("SkipAssimpLoader"); result.GetValueOr(false))
						return false;

					return true;
				};

				Nz::MeshLoader& meshLoader = core->GetMeshLoader();
				m_meshLoaderEntry = meshLoader.RegisterLoader(std::move(meshLoaderEntry));

				return true;
			}

			void DeactivateImpl() override
			{
				Nz::Core* core = Nz::Core::Instance();
				NazaraAssertMsg(core, "core module is not instanced");

				Nz::AnimationLoader& animationLoader = core->GetAnimationLoader();
				animationLoader.UnregisterLoader(m_animationLoaderEntry);

				Nz::MeshLoader& meshLoader = core->GetMeshLoader();
				meshLoader.UnregisterLoader(m_meshLoaderEntry);
			}

			std::string_view GetDescription() const override
			{
				return "Adds supports to load meshes and animations using Assimp";
			}

			std::string_view GetName() const override
			{
				return "Assimp loader";
			}

			Nz::UInt32 GetVersion() const override
			{
				return 100;
			}

		private:
			const Nz::AnimationLoader::Entry* m_animationLoaderEntry = nullptr;
			const Nz::MeshLoader::Entry* m_meshLoaderEntry = nullptr;
	};
}

#ifdef NAZARA_PLUGINS_STATIC
namespace Nz
{
	std::unique_ptr<AssimpPlugin> StaticPluginProvider<AssimpPlugin>::Instantiate()
	{
		return std::make_unique<AssimpPluginImpl>();
	}
}
#else
extern "C"
{
	NAZARA_EXPORT Nz::PluginInterface* PluginLoad()
	{
		Nz::Core* core = Nz::Core::Instance();
		if (!core)
		{
			NazaraError("Core module must be initialized");
			return nullptr;
		}

		std::unique_ptr<AssimpPluginImpl> plugin = std::make_unique<AssimpPluginImpl>();
		return plugin.release();
	}
}
#endif
