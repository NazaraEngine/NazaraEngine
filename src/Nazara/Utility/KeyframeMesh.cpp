// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/KeyframeMesh.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <vector>
#include <Nazara/Utility/Debug.hpp>

struct NzKeyframeMeshImpl
{
	nzBufferAccess lockAccess;
	NzAxisAlignedBox* aabb;
	NzMeshVertex* lockBuffer;
	NzVector3f* normals;
	NzVector3f* positions;
	NzVector3f* tangents;
	const NzIndexBuffer* indexBuffer = nullptr;
	NzVertexBuffer* vertexBuffer;
	unsigned int frameCount;
	unsigned int lockLevel = 0;
};

NzKeyframeMesh::NzKeyframeMesh(const NzMesh* parent) :
NzSubMesh(parent)
{
}

NzKeyframeMesh::~NzKeyframeMesh()
{
	Destroy();
}

bool NzKeyframeMesh::Create(NzVertexBuffer* vertexBuffer, unsigned int frameCount, bool lock)
{
	Destroy();

	#if NAZARA_UTILITY_SAFE
	if (!vertexBuffer)
	{
		NazaraError("Invalid vertex buffer");
		return false;
	}

	if (vertexBuffer->GetTypeSize() < sizeof(NzMeshVertex))
	{
		NazaraError("Vertex buffer type size must be greater or equal than size of mesh vertex");
		return false;
	}

	if (frameCount == 0)
	{
		NazaraError("Frame count must be over 0");
		return false;
	}
	#endif

	vertexBuffer->AddResourceReference();

	m_impl = new NzKeyframeMeshImpl;
	m_impl->aabb = new NzAxisAlignedBox[frameCount+1]; // La première case représente l'AABB interpolée
	m_impl->frameCount = frameCount;
	m_impl->vertexBuffer = vertexBuffer;

	unsigned int vertexCount = vertexBuffer->GetVertexCount();

	m_impl->positions = new NzVector3f[frameCount*vertexCount];
	m_impl->normals = new NzVector3f[frameCount*vertexCount];
	m_impl->tangents = new NzVector3f[frameCount*vertexCount];

	if (lock && !Lock(nzBufferAccess_DiscardAndWrite))
	{
		NazaraError("Failed to lock buffer");
		Destroy();

		return false;
	}

	NotifyCreated();
	return true;
}

void NzKeyframeMesh::Destroy()
{
	if (m_impl)
	{
		NotifyDestroy();

		if (m_impl->indexBuffer)
			m_impl->indexBuffer->RemoveResourceReference();

		m_impl->vertexBuffer->RemoveResourceReference();

		delete[] m_impl->aabb;
		delete[] m_impl->normals;
		delete[] m_impl->positions;
		delete[] m_impl->tangents;

		delete m_impl;
		m_impl = nullptr;
	}
}

void NzKeyframeMesh::Finish()
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Keyframe mesh not created");
		return;
	}
	#endif

	InterpolateImpl(0, 0, 0.f);
}

const NzAxisAlignedBox& NzKeyframeMesh::GetAABB() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Keyframe mesh not created");
		return NzAxisAlignedBox::Null;
	}
	#endif

	return m_impl->aabb[0];
}

nzAnimationType NzKeyframeMesh::GetAnimationType() const
{
	return nzAnimationType_Keyframe;
}

unsigned int NzKeyframeMesh::GetFrameCount() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Keyframe mesh not created");
		return 0;
	}
	#endif

	return m_impl->frameCount;
}

const NzIndexBuffer* NzKeyframeMesh::GetIndexBuffer() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Keyframe mesh not created");
		return nullptr;
	}
	#endif

	return m_impl->indexBuffer;
}

bool NzKeyframeMesh::GetVertex(NzMeshVertex* dest, unsigned int frameIndex, unsigned int vertexIndex, bool queryUV) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Keyframe mesh not created");
		return false;
	}

	if (frameIndex >= m_impl->frameCount)
	{
		NazaraError("Frame index out of bounds (" + NzString::Number(frameIndex) + " >= " + NzString::Number(m_impl->frameCount) + ')');
		return false;
	}
	#endif

	unsigned int vertexCount = m_impl->vertexBuffer->GetVertexCount();

	#if NAZARA_UTILITY_SAFE
	if (vertexIndex >= vertexCount)
	{
		NazaraError("Vertex index out of bounds (" + NzString::Number(vertexIndex) + " >= " + NzString::Number(vertexCount) + ')');
		return false;
	}
	#endif

	unsigned int index = frameIndex*vertexCount + vertexIndex;

	dest->normal = m_impl->normals[index];
	dest->position = m_impl->positions[index];
	dest->tangent = m_impl->tangents[index];

	if (queryUV)
	{
		if (!Lock(nzBufferAccess_ReadOnly))
		{
			NazaraError("Failed to query UV");
			return false;
		}

		NzMeshVertex& vertex = m_impl->lockBuffer[vertexIndex];
		dest->uv = vertex.uv;

		Unlock();
	}

	return true;
}

const NzVertexBuffer* NzKeyframeMesh::GetVertexBuffer() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Keyframe mesh not created");
		return nullptr;
	}
	#endif

	return m_impl->vertexBuffer;
}

void NzKeyframeMesh::Interpolate(const NzAnimation* animation, unsigned int frameA, unsigned int frameB, float interpolation) const
{
	#if NAZARA_UTILITY_SAFE
	if (!animation || !animation->IsValid())
	{
		NazaraError("Animation must be valid");
		return;
	}

	if (animation->GetType() != nzAnimationType_Keyframe)
	{
		NazaraError("Animation must be of keyframe type");
		return;
	}

	unsigned int frameCount = animation->GetFrameCount();
	if (frameA >= frameCount)
	{
		NazaraError("Frame A is out of range (" + NzString::Number(frameA) + " >= " + NzString::Number(frameCount) + ')');
		return;
	}

	if (frameB >= frameCount)
	{
		NazaraError("Frame B is out of range (" + NzString::Number(frameB) + " >= " + NzString::Number(frameCount) + ')');
		return;
	}
	#endif

	#ifdef NAZARA_DEBUG
	if (interpolation < 0.f || interpolation > 1.f)
	{
		NazaraError("Interpolation must be in range [0..1] (Got " + NzString::Number(interpolation) + ')');
		return;
	}
	#endif

	InterpolateImpl(frameA, frameB, interpolation);

	m_parent->InvalidateAABB();
}

bool NzKeyframeMesh::IsAnimated() const
{
	return true;
}

bool NzKeyframeMesh::IsValid()
{
	return m_impl != nullptr;
}

bool NzKeyframeMesh::Lock(nzBufferAccess access) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Keyframe mesh not created");
		return false;
	}
	#endif

	if (m_impl->lockLevel == 0)
	{
		m_impl->lockBuffer = reinterpret_cast<NzMeshVertex*>(m_impl->vertexBuffer->Map(access));
		if (!m_impl->lockBuffer)
		{
			NazaraError("Failed to lock vertex buffer");
			m_impl->lockLevel = 0;

			return false;
		}

		m_impl->lockAccess = access;
	}
	else
	{
		///FIXME: Vérifier cette condition
		if (m_impl->lockAccess != access &&
		   (m_impl->lockAccess != nzBufferAccess_ReadWrite || access != nzBufferAccess_WriteOnly) &&
		   (m_impl->lockAccess != nzBufferAccess_ReadWrite || access != nzBufferAccess_ReadOnly) &&
		   (m_impl->lockAccess != nzBufferAccess_DiscardAndWrite || access != nzBufferAccess_WriteOnly))
		{
			NazaraError("Vertex buffer already locked by an incompatible lock access");
			return false;
		}
	}

	m_impl->lockLevel++;

	return true;
}

void NzKeyframeMesh::SetAABB(unsigned int frameIndex, const NzAxisAlignedBox& aabb)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Keyframe mesh not created");
		return;
	}

	if (frameIndex >= m_impl->frameCount)
	{
		NazaraError("Frame index out of bounds (" + NzString::Number(frameIndex) + " >= " + NzString::Number(m_impl->frameCount) + ')');
		return;
	}
	#endif

	m_impl->aabb[frameIndex+1] = aabb;
}

void NzKeyframeMesh::SetIndexBuffer(const NzIndexBuffer* indexBuffer)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Keyframe mesh not created");
		return;
	}
	#endif

	if (m_impl->indexBuffer)
		m_impl->indexBuffer->RemoveResourceReference();

	if (indexBuffer)
		indexBuffer->AddResourceReference();

	m_impl->indexBuffer = indexBuffer;
}

bool NzKeyframeMesh::SetVertex(const NzMeshVertex& source, unsigned int frameIndex, unsigned int vertexIndex, bool setUV)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Keyframe mesh not created");
		return false;
	}

	if (frameIndex >= m_impl->frameCount)
	{
		NazaraError("Frame index out of bounds (" + NzString::Number(frameIndex) + " >= " + NzString::Number(m_impl->frameCount) + ')');
		return false;
	}
	#endif

	unsigned int vertexCount = m_impl->vertexBuffer->GetVertexCount();

	#if NAZARA_UTILITY_SAFE
	if (vertexIndex >= vertexCount)
	{
		NazaraError("Vertex index out of bounds (" + NzString::Number(vertexIndex) + " >= " + NzString::Number(vertexCount) + ')');
		return false;
	}
	#endif

	unsigned int index = frameIndex*vertexCount + vertexIndex;

	m_impl->normals[index] = source.normal;
	m_impl->positions[index] = source.position;
	m_impl->tangents[index] = source.tangent;

	if (setUV)
	{
		if (!Lock(nzBufferAccess_WriteOnly))
		{
			NazaraError("Failed to write UV");
			return false;
		}

		NzMeshVertex& vertex = m_impl->lockBuffer[vertexIndex];
		vertex.uv = source.uv;

		Unlock();
	}

	return true;
}

void NzKeyframeMesh::Unlock() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Keyframe mesh not created");
		return;
	}

	if (m_impl->lockLevel == 0)
	{
		NazaraWarning("Unlock called on non-locked texture");
		return;
	}
	#endif

	if (--m_impl->lockLevel == 0)
	{
		if (!m_impl->vertexBuffer->Unmap())
			NazaraWarning("Failed to unmap vertex buffer, expect mesh corruption");
	}
}

void NzKeyframeMesh::InterpolateImpl(unsigned int frameA, unsigned int frameB, float interpolation) const
{
	#ifdef NAZARA_DEBUG
	if (!m_impl)
	{
		NazaraError("Keyframe mesh not created");
		return;
	}
	#endif

	// Interpolation de l'AABB
	m_impl->aabb[0] = NzAxisAlignedBox::Lerp(m_impl->aabb[frameA+1], m_impl->aabb[frameB+1], interpolation);

	if (!Lock(nzBufferAccess_WriteOnly))
	{
		NazaraError("Failed to lock vertex buffer");
		return;
	}

	unsigned int vertexCount = m_impl->vertexBuffer->GetVertexCount();

	// Adaptation des indices
	frameA *= vertexCount;
	frameB *= vertexCount;

	NzMeshVertex* vertex = m_impl->lockBuffer;
	for (unsigned int i = 0; i < vertexCount; ++i)
	{
		vertex->normal = NzVector3f::Lerp(m_impl->positions[frameA+i], m_impl->positions[frameB+i], interpolation);
		vertex->position = NzVector3f::Lerp(m_impl->positions[frameA+i], m_impl->positions[frameB+i], interpolation);
		vertex->tangent = NzVector3f::Lerp(m_impl->positions[frameA+i], m_impl->positions[frameB+i], interpolation);

		vertex++;
	}

	Unlock();
}
