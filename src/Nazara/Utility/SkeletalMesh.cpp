// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/SkeletalMesh.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/Skeleton.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <vector>
#include <Nazara/Utility/Debug.hpp>

struct NzSkeletalMeshImpl
{
	std::vector<NzVertexWeight> vertexWeights;
	std::vector<NzWeight> weights;
	NzAxisAlignedBox aabb;
	nzUInt8* bindPoseBuffer;
	const NzIndexBuffer* indexBuffer = nullptr;
	NzVertexBuffer* vertexBuffer;
};

NzSkeletalMesh::NzSkeletalMesh(const NzMesh* parent) :
NzSubMesh(parent)
{
}

NzSkeletalMesh::~NzSkeletalMesh()
{
	Destroy();
}

bool NzSkeletalMesh::Create(NzVertexBuffer* vertexBuffer, unsigned int weightCount)
{
	Destroy();

	#if NAZARA_UTILITY_SAFE
	if (!vertexBuffer)
	{
		NazaraError("Invalid vertex buffer");
		return false;
	}

	if (weightCount == 0)
	{
		NazaraError("Weight count must be over 0");
		return false;
	}
	#endif

	vertexBuffer->AddResourceReference();

	unsigned int vertexCount = vertexBuffer->GetVertexCount();

	m_impl = new NzSkeletalMeshImpl;
	m_impl->bindPoseBuffer = new nzUInt8[vertexCount*vertexBuffer->GetTypeSize()];
	m_impl->vertexBuffer = vertexBuffer;
	m_impl->vertexWeights.resize(vertexCount);
	m_impl->weights.resize(weightCount);

	return true;
}

void NzSkeletalMesh::Destroy()
{
	if (m_impl)
	{
		if (m_impl->indexBuffer)
			m_impl->indexBuffer->RemoveResourceReference();

		if (m_impl->vertexBuffer)
			m_impl->vertexBuffer->RemoveResourceReference();

		delete[] m_impl->bindPoseBuffer;
		delete m_impl;
		m_impl = nullptr;
	}
}

const NzAxisAlignedBox& NzSkeletalMesh::GetAABB() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeletal mesh not created");
		return NzAxisAlignedBox::Null;
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

const NzVertexBuffer* NzSkeletalMesh::GetVertexBuffer() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeletal mesh not created");
		return nullptr;
	}
	#endif

	return m_impl->vertexBuffer;
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

void NzSkeletalMesh::Skin() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeletal mesh not created");
		return;
	}
	#endif

	Skin(m_parent->GetSkeleton());
}

void NzSkeletalMesh::Skin(const NzSkeleton* skeleton) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeletal mesh not created");
		return;
	}
	#endif

	void* outputBuffer = m_impl->vertexBuffer->Map(nzBufferAccess_DiscardAndWrite);
	if (!outputBuffer)
	{
		NazaraError("Failed to map vertex buffer");
		return;
	}

	NzVertexStruct_XYZ_Normal_UV_Tangent* inputVertex = reinterpret_cast<NzVertexStruct_XYZ_Normal_UV_Tangent*>(m_impl->bindPoseBuffer);
	NzVertexStruct_XYZ_Normal_UV_Tangent* outputVertex = reinterpret_cast<NzVertexStruct_XYZ_Normal_UV_Tangent*>(outputBuffer);

	const NzJoint* joints = skeleton->GetJoints();
	unsigned int vertexCount = m_impl->vertexBuffer->GetVertexCount();
	for (unsigned int i = 0; i < vertexCount; ++i)
	{
		NzVector3f finalPosition(NzVector3f::Zero());
		NzVector3f finalNormal(NzVector3f::Zero());
		NzVector3f finalTangent(NzVector3f::Zero());

		unsigned int weightCount = m_impl->vertexWeights[i].weights.size();
		for (unsigned int j = 0; j < weightCount; ++j)
		{
			const NzWeight& weight = m_impl->weights[m_impl->vertexWeights[i].weights[j]];

			NzMatrix4f mat(joints[weight.jointIndex].GetInverseBindMatrix());
			mat.ConcatenateAffine(joints[weight.jointIndex].GetTransformMatrix());
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

	if (!m_impl->vertexBuffer->Unmap())
		NazaraWarning("Failed to unmap vertex buffer");

	m_impl->aabb = skeleton->GetAABB();
}

void NzSkeletalMesh::SetIndexBuffer(const NzIndexBuffer* indexBuffer)
{
	if (m_impl->indexBuffer)
		m_impl->indexBuffer->RemoveResourceReference();

	if (indexBuffer)
		indexBuffer->AddResourceReference();

	m_impl->indexBuffer = indexBuffer;
}
