// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/KeyframeMesh.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Utility/Animation.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <vector>
#include <Nazara/Utility/Debug.hpp>

struct NzKeyframeMeshImpl
{
	NzCubef* aabb;
	NzVector2f* uv;
	NzVector3f* normals;
	NzVector3f* positions;
	NzVector3f* tangents;
	const NzIndexBuffer* indexBuffer = nullptr;
	NzVertexBuffer* vertexBuffer;
	unsigned int frameCount;
};

NzKeyframeMesh::NzKeyframeMesh(const NzMesh* parent) :
NzSubMesh(parent)
{
}

NzKeyframeMesh::~NzKeyframeMesh()
{
	Destroy();
}

bool NzKeyframeMesh::Create(NzVertexBuffer* vertexBuffer, unsigned int frameCount)
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
	m_impl->aabb = new NzCubef[frameCount+1]; // La première case représente l'AABB interpolée
	m_impl->frameCount = frameCount;
	m_impl->vertexBuffer = vertexBuffer;

	unsigned int vertexCount = vertexBuffer->GetVertexCount();

	m_impl->positions = new NzVector3f[frameCount*vertexCount];
	m_impl->normals = new NzVector3f[frameCount*vertexCount];
	m_impl->tangents = new NzVector3f[frameCount*vertexCount];
	m_impl->uv = new NzVector2f[vertexCount];

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
		delete[] m_impl->uv;

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

	GenerateAABBs();
	InterpolateImpl(0, 0, 0.f);
}

void NzKeyframeMesh::GenerateAABBs()
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Keyframe mesh not created");
		return;
	}
	#endif

	unsigned int vertexCount = m_impl->vertexBuffer->GetVertexCount();
	for (unsigned int i = 0; i < m_impl->frameCount; ++i)
	{
		NzCubef& aabb = m_impl->aabb[i+1]; // l'AABB 0 est celle qui est interpolée

		// Génération de l'AABB selon la position
		unsigned int index = i*vertexCount;
		for (unsigned int j = 0; j < vertexCount; ++j)
			aabb.ExtendTo(m_impl->positions[index+j]);
	}
}

const NzCubef& NzKeyframeMesh::GetAABB() const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Keyframe mesh not created");

		static NzCubef dummy;
		return dummy;
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

NzVector3f NzKeyframeMesh::GetNormal(unsigned int frameIndex, unsigned int vertexIndex) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Keyframe mesh not created");
		return NzVector3f();
	}

	if (frameIndex >= m_impl->frameCount)
	{
		NazaraError("Frame index out of bounds (" + NzString::Number(frameIndex) + " >= " + NzString::Number(m_impl->frameCount) + ')');
		return NzVector3f();
	}
	#endif

	unsigned int vertexCount = m_impl->vertexBuffer->GetVertexCount();

	#if NAZARA_UTILITY_SAFE
	if (vertexIndex >= vertexCount)
	{
		NazaraError("Vertex index out of bounds (" + NzString::Number(vertexIndex) + " >= " + NzString::Number(vertexCount) + ')');
		return NzVector3f();
	}
	#endif

	unsigned int index = frameIndex*vertexCount + vertexIndex;

	return m_impl->normals[index];
}

NzVector3f NzKeyframeMesh::GetPosition(unsigned int frameIndex, unsigned int vertexIndex) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Keyframe mesh not created");
		return NzVector3f();
	}

	if (frameIndex >= m_impl->frameCount)
	{
		NazaraError("Frame index out of bounds (" + NzString::Number(frameIndex) + " >= " + NzString::Number(m_impl->frameCount) + ')');
		return NzVector3f();
	}
	#endif

	unsigned int vertexCount = m_impl->vertexBuffer->GetVertexCount();

	#if NAZARA_UTILITY_SAFE
	if (vertexIndex >= vertexCount)
	{
		NazaraError("Vertex index out of bounds (" + NzString::Number(vertexIndex) + " >= " + NzString::Number(vertexCount) + ')');
		return NzVector3f();
	}
	#endif

	unsigned int index = frameIndex*vertexCount + vertexIndex;

	return m_impl->positions[index];
}

NzVector3f NzKeyframeMesh::GetTangent(unsigned int frameIndex, unsigned int vertexIndex) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Keyframe mesh not created");
		return NzVector3f();
	}

	if (frameIndex >= m_impl->frameCount)
	{
		NazaraError("Frame index out of bounds (" + NzString::Number(frameIndex) + " >= " + NzString::Number(m_impl->frameCount) + ')');
		return NzVector3f();
	}
	#endif

	unsigned int vertexCount = m_impl->vertexBuffer->GetVertexCount();

	#if NAZARA_UTILITY_SAFE
	if (vertexIndex >= vertexCount)
	{
		NazaraError("Vertex index out of bounds (" + NzString::Number(vertexIndex) + " >= " + NzString::Number(vertexCount) + ')');
		return NzVector3f();
	}
	#endif

	unsigned int index = frameIndex*vertexCount + vertexIndex;

	return m_impl->tangents[index];
}

NzVector2f NzKeyframeMesh::GetTexCoords(unsigned int vertexIndex) const
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Keyframe mesh not created");
		return NzVector2f();
	}

	if (vertexIndex >= m_impl->vertexBuffer->GetVertexCount())
	{
		NazaraError("Vertex index out of bounds (" + NzString::Number(vertexIndex) + " >= " + NzString::Number(m_impl->vertexBuffer->GetVertexCount()) + ')');
		return NzVector2f();
	}
	#endif

	return m_impl->uv[vertexIndex];
}

void NzKeyframeMesh::GetVertex(unsigned int frameIndex, unsigned int vertexIndex, NzMeshVertex* dest) const
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

	unsigned int vertexCount = m_impl->vertexBuffer->GetVertexCount();

	#if NAZARA_UTILITY_SAFE
	if (vertexIndex >= vertexCount)
	{
		NazaraError("Vertex index out of bounds (" + NzString::Number(vertexIndex) + " >= " + NzString::Number(vertexCount) + ')');
		return;
	}
	#endif

	unsigned int index = frameIndex*vertexCount + vertexIndex;

	dest->normal = m_impl->normals[index];
	dest->position = m_impl->positions[index];
	dest->tangent = m_impl->tangents[index];
	dest->uv = m_impl->uv[vertexIndex];
}

NzVertexBuffer* NzKeyframeMesh::GetVertexBuffer()
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

void NzKeyframeMesh::SetAABB(unsigned int frameIndex, const NzCubef& aabb)
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

void NzKeyframeMesh::SetNormal(unsigned int frameIndex, unsigned int vertexIndex, const NzVector3f& normal)
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

	unsigned int vertexCount = m_impl->vertexBuffer->GetVertexCount();

	#if NAZARA_UTILITY_SAFE
	if (vertexIndex >= vertexCount)
	{
		NazaraError("Vertex index out of bounds (" + NzString::Number(vertexIndex) + " >= " + NzString::Number(vertexCount) + ')');
		return;
	}
	#endif

	unsigned int index = frameIndex*vertexCount + vertexIndex;

	m_impl->normals[index] = normal;
}

void NzKeyframeMesh::SetPosition(unsigned int frameIndex, unsigned int vertexIndex, const NzVector3f& position)
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

	unsigned int vertexCount = m_impl->vertexBuffer->GetVertexCount();

	#if NAZARA_UTILITY_SAFE
	if (vertexIndex >= vertexCount)
	{
		NazaraError("Vertex index out of bounds (" + NzString::Number(vertexIndex) + " >= " + NzString::Number(vertexCount) + ')');
		return;
	}
	#endif

	unsigned int index = frameIndex*vertexCount + vertexIndex;

	m_impl->positions[index] = position;
}

void NzKeyframeMesh::SetTangent(unsigned int frameIndex, unsigned int vertexIndex, const NzVector3f& tangent)
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

	unsigned int vertexCount = m_impl->vertexBuffer->GetVertexCount();

	#if NAZARA_UTILITY_SAFE
	if (vertexIndex >= vertexCount)
	{
		NazaraError("Vertex index out of bounds (" + NzString::Number(vertexIndex) + " >= " + NzString::Number(vertexCount) + ')');
		return;
	}
	#endif

	unsigned int index = frameIndex*vertexCount + vertexIndex;

	m_impl->tangents[index] = tangent;
}

void NzKeyframeMesh::SetTexCoords(unsigned int vertexIndex, const NzVector2f& uv)
{
	#if NAZARA_UTILITY_SAFE
	if (!m_impl)
	{
		NazaraError("Keyframe mesh not created");
		return;
	}

	if (vertexIndex >= m_impl->vertexBuffer->GetVertexCount())
	{
		NazaraError("Vertex index out of bounds (" + NzString::Number(vertexIndex) + " >= " + NzString::Number(m_impl->vertexBuffer->GetVertexCount()) + ')');
		return;
	}
	#endif

	m_impl->uv[vertexIndex] = uv;
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
	m_impl->aabb[0] = NzCubef::Lerp(m_impl->aabb[frameA+1], m_impl->aabb[frameB+1], interpolation);

	NzMeshVertex* vertex = reinterpret_cast<NzMeshVertex*>(m_impl->vertexBuffer->Map(nzBufferAccess_DiscardAndWrite));
	if (!vertex)
	{
		NazaraError("Failed to map vertex buffer");
		return;
	}

	unsigned int vertexCount = m_impl->vertexBuffer->GetVertexCount();

	// Adaptation des indices
	frameA *= vertexCount;
	frameB *= vertexCount;

	for (unsigned int i = 0; i < vertexCount; ++i)
	{
		vertex->normal = NzVector3f::Lerp(m_impl->positions[frameA+i], m_impl->positions[frameB+i], interpolation);
		vertex->position = NzVector3f::Lerp(m_impl->positions[frameA+i], m_impl->positions[frameB+i], interpolation);
		vertex->tangent = NzVector3f::Lerp(m_impl->positions[frameA+i], m_impl->positions[frameB+i], interpolation);
		vertex->uv = m_impl->uv[i];

		vertex->normal.Normalize();
		vertex->tangent.Normalize();

		vertex++;
	}

	m_impl->vertexBuffer->Unmap();
}
