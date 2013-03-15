// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Clock.hpp>

#include <Nazara/Utility/SkeletalMesh.hpp>
#include <Nazara/Core/TaskScheduler.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <vector>
#include <Nazara/Utility/Debug.hpp>

namespace
{
	struct SkinningInfos
	{
		const NzJoint* joints;
		const NzMeshVertex* inputVertex;
		NzMeshVertex* outputVertex;
		const NzVertexWeight* vertexWeights;
		const NzWeight* weights;
	};

	void Skin_Position(const SkinningInfos& skinningInfos, unsigned int startVertex, unsigned int vertexCount)
	{
		const NzMeshVertex* inputVertex = &skinningInfos.inputVertex[startVertex];
		NzMeshVertex* outputVertex = &skinningInfos.outputVertex[startVertex];

		unsigned int endVertex = startVertex + vertexCount - 1;
		for (unsigned int i = startVertex; i <= endVertex; ++i)
		{
			NzVector3f finalPosition(NzVector3f::Zero());

			unsigned int weightCount = skinningInfos.vertexWeights[i].weights.size();
			for (unsigned int j = 0; j < weightCount; ++j)
			{
				const NzWeight& weight = skinningInfos.weights[skinningInfos.vertexWeights[i].weights[j]];

				NzMatrix4f mat(skinningInfos.joints[weight.jointIndex].GetInverseBindMatrix());
				mat.ConcatenateAffine(skinningInfos.joints[weight.jointIndex].GetTransformMatrix());
				mat *= weight.weight;

				finalPosition += mat.Transform(inputVertex->position);
			}

			outputVertex->position = finalPosition;
			outputVertex->uv = inputVertex->uv;

			inputVertex++;
			outputVertex++;
		}
	}

	void Skin_PositionNormal(const SkinningInfos& skinningInfos, unsigned int startVertex, unsigned int vertexCount)
	{
		const NzMeshVertex* inputVertex = &skinningInfos.inputVertex[startVertex];
		NzMeshVertex* outputVertex = &skinningInfos.outputVertex[startVertex];

		unsigned int endVertex = startVertex + vertexCount - 1;
		for (unsigned int i = startVertex; i <= endVertex; ++i)
		{
			NzVector3f finalPosition(NzVector3f::Zero());
			NzVector3f finalNormal(NzVector3f::Zero());

			unsigned int weightCount = skinningInfos.vertexWeights[i].weights.size();
			for (unsigned int j = 0; j < weightCount; ++j)
			{
				const NzWeight& weight = skinningInfos.weights[skinningInfos.vertexWeights[i].weights[j]];

				NzMatrix4f mat(skinningInfos.joints[weight.jointIndex].GetInverseBindMatrix());
				mat.ConcatenateAffine(skinningInfos.joints[weight.jointIndex].GetTransformMatrix());
				mat *= weight.weight;

				finalPosition += mat.Transform(inputVertex->position);
				finalNormal += mat.Transform(inputVertex->normal, 0.f);
			}

			finalNormal.Normalize();

			outputVertex->normal = finalNormal;
			outputVertex->position = finalPosition;
			outputVertex->uv = inputVertex->uv;

			inputVertex++;
			outputVertex++;
		}
	}

	void Skin_PositionNormalTangent(const SkinningInfos& skinningInfos, unsigned int startVertex, unsigned int vertexCount)
	{
		const NzMeshVertex* inputVertex = &skinningInfos.inputVertex[startVertex];
		NzMeshVertex* outputVertex = &skinningInfos.outputVertex[startVertex];

		unsigned int endVertex = startVertex + vertexCount - 1;
		for (unsigned int i = startVertex; i <= endVertex; ++i)
		{
			NzVector3f finalPosition(NzVector3f::Zero());
			NzVector3f finalNormal(NzVector3f::Zero());
			NzVector3f finalTangent(NzVector3f::Zero());

			unsigned int weightCount = skinningInfos.vertexWeights[i].weights.size();
			for (unsigned int j = 0; j < weightCount; ++j)
			{
				const NzWeight& weight = skinningInfos.weights[skinningInfos.vertexWeights[i].weights[j]];

				NzMatrix4f mat(skinningInfos.joints[weight.jointIndex].GetInverseBindMatrix());
				mat.ConcatenateAffine(skinningInfos.joints[weight.jointIndex].GetTransformMatrix());
				mat *= weight.weight;

				finalPosition += mat.Transform(inputVertex->position);
				finalNormal += mat.Transform(inputVertex->normal, 0.f);
				finalTangent += mat.Transform(inputVertex->tangent, 0.f);
			}

			finalNormal.Normalize();
			finalTangent.Normalize();

			outputVertex->normal = finalNormal;
			outputVertex->position = finalPosition;
			outputVertex->tangent = finalTangent;
			outputVertex->uv = inputVertex->uv;

			inputVertex++;
			outputVertex++;
		}
	}
}

struct NzSkeletalMeshImpl
{
	std::vector<NzVertexWeight> vertexWeights;
	std::vector<NzWeight> weights;
	NzCubef aabb;
	nzUInt8* bindPoseBuffer;
	NzIndexBufferConstRef indexBuffer;
	unsigned int vertexCount;
};

NzSkeletalMesh::NzSkeletalMesh(const NzMesh* parent) :
NzSubMesh(parent)
{
}

NzSkeletalMesh::~NzSkeletalMesh()
{
	Destroy();
}

bool NzSkeletalMesh::Create(unsigned int vertexCount, unsigned int weightCount)
{
	Destroy();

	#if NAZARA_UTILITY_SAFE
	if (vertexCount == 0)
	{
		NazaraError("Vertex count must be over 0");
		return false;
	}

	if (weightCount == 0)
	{
		NazaraError("Weight count must be over 0");
		return false;
	}
	#endif

	m_impl = new NzSkeletalMeshImpl;
	m_impl->bindPoseBuffer = new nzUInt8[vertexCount*sizeof(NzMeshVertex)];
	m_impl->vertexCount = vertexCount;
	m_impl->vertexWeights.resize(vertexCount);
	m_impl->weights.resize(weightCount);

	return true;
}

void NzSkeletalMesh::Destroy()
{
	if (m_impl)
	{
		delete[] m_impl->bindPoseBuffer;
		delete m_impl;
		m_impl = nullptr;
	}
}

void NzSkeletalMesh::Finish()
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeletal mesh not created");
		return;
	}
	#endif

	// Rien à faire de particulier
}

const NzCubef& NzSkeletalMesh::GetAABB() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeletal mesh not created");

		static NzCubef dummy;
		return dummy;
	}
	#endif

	return m_impl->aabb;
}

nzAnimationType NzSkeletalMesh::GetAnimationType() const
{
	return nzAnimationType_Skeletal;
}

void* NzSkeletalMesh::GetBindPoseBuffer()
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeletal mesh not created");
		return nullptr;
	}
	#endif

	return m_impl->bindPoseBuffer;
}

const void* NzSkeletalMesh::GetBindPoseBuffer() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeletal mesh not created");
		return nullptr;
	}
	#endif

	return m_impl->bindPoseBuffer;
}

const NzIndexBuffer* NzSkeletalMesh::GetIndexBuffer() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeletal mesh not created");
		return nullptr;
	}
	#endif

	return m_impl->indexBuffer;
}

unsigned int NzSkeletalMesh::GetVertexCount() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeletal mesh not created");
		return 0;
	}
	#endif

	return m_impl->vertexCount;
}

NzVertexWeight* NzSkeletalMesh::GetVertexWeight(unsigned int vertexIndex)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeletal mesh not created");
		return nullptr;
	}
	#endif

	return &m_impl->vertexWeights[vertexIndex];
}

const NzVertexWeight* NzSkeletalMesh::GetVertexWeight(unsigned int vertexIndex) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeletal mesh not created");
		return nullptr;
	}
	#endif

	return &m_impl->vertexWeights[vertexIndex];
}

NzWeight* NzSkeletalMesh::GetWeight(unsigned int weightIndex)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeletal mesh not created");
		return nullptr;
	}
	#endif

	return &m_impl->weights[weightIndex];
}

const NzWeight* NzSkeletalMesh::GetWeight(unsigned int weightIndex) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeletal mesh not created");
		return nullptr;
	}
	#endif

	return &m_impl->weights[weightIndex];
}

unsigned int NzSkeletalMesh::GetWeightCount() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeletal mesh not created");
		return 0;
	}
	#endif

	return m_impl->weights.size();
}

bool NzSkeletalMesh::IsAnimated() const
{
	return true;
}

bool NzSkeletalMesh::IsValid() const
{
	return m_impl != nullptr;
}

void NzSkeletalMesh::Skin(NzMeshVertex* outputBuffer) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeletal mesh not created");
		return;
	}
	#endif

	Skin(outputBuffer, m_parent->GetSkeleton());
}

void NzSkeletalMesh::Skin(NzMeshVertex* outputBuffer, const NzSkeleton* skeleton) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeletal mesh not created");
		return;
	}
	#endif

	SkinningInfos skinningInfos;
	skinningInfos.inputVertex = reinterpret_cast<const NzMeshVertex*>(m_impl->bindPoseBuffer);
	skinningInfos.outputVertex = outputBuffer;
	skinningInfos.joints = skeleton->GetJoints();
	skinningInfos.vertexWeights = &m_impl->vertexWeights[0];
	skinningInfos.weights = &m_impl->weights[0];

	#if NAZARA_UTILITY_MULTITHREADED_SKINNING
	unsigned int jointCount = skeleton->GetJointCount();
	for (unsigned int i = 0; i < jointCount; ++i)
		skinningInfos.joints[i].EnsureTransformMatrixUpdate();

	unsigned int workerCount = NzTaskScheduler::GetWorkerCount();

	std::ldiv_t div = std::ldiv(m_impl->vertexCount, workerCount); // Qui sait, peut-être que ça permet des optimisations plus efficaces
	for (unsigned int i = 0; i < workerCount; ++i)
		NzTaskScheduler::AddTask(Skin_PositionNormalTangent, skinningInfos, i*div.quot, (i == workerCount-1) ? div.quot + div.rem : div.quot);

	NzTaskScheduler::WaitForTasks();
	#else
	Skin_PositionNormalTangent(skinningInfos, 0, m_impl->vertexCount);
	#endif

	m_impl->aabb = skeleton->GetAABB();
}

void NzSkeletalMesh::SetIndexBuffer(const NzIndexBuffer* indexBuffer)
{
	m_impl->indexBuffer = indexBuffer;
}
