// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/SkeletalMesh.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <memory>
#include <vector>
#include <Nazara/Utility/Debug.hpp>

struct NzSkeletalMeshImpl
{
	std::unique_ptr<NzMeshVertex[]> bindPoseBuffer;
	std::vector<NzVertexWeight> vertexWeights;
	std::vector<NzWeight> weights;
	NzBoxf aabb;
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
	m_impl->bindPoseBuffer.reset(new NzMeshVertex[vertexCount]);
	m_impl->vertexCount = vertexCount;
	m_impl->vertexWeights.resize(vertexCount);
	m_impl->weights.resize(weightCount);

	return true;
}

void NzSkeletalMesh::Destroy()
{
	if (m_impl)
	{
		NotifyDestroy();

		delete m_impl;
		m_impl = nullptr;
	}
}

const NzBoxf& NzSkeletalMesh::GetAABB() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeletal mesh not created");

		static NzBoxf dummy;
		return dummy;
	}
	#endif

	return m_impl->aabb;
}

nzAnimationType NzSkeletalMesh::GetAnimationType() const
{
	return nzAnimationType_Skeletal;
}

NzMeshVertex* NzSkeletalMesh::GetBindPoseBuffer()
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeletal mesh not created");
		return nullptr;
	}
	#endif

	return m_impl->bindPoseBuffer.get();
}

const NzMeshVertex* NzSkeletalMesh::GetBindPoseBuffer() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Skeletal mesh not created");
		return nullptr;
	}
	#endif

	return m_impl->bindPoseBuffer.get();
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

void NzSkeletalMesh::SetIndexBuffer(const NzIndexBuffer* indexBuffer)
{
	m_impl->indexBuffer = indexBuffer;
}
