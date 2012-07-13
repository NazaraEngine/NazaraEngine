// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Core/Error.hpp>
#include <stdexcept>
#include <Nazara/Utility/Debug.hpp>

NzStaticMesh::NzStaticMesh(const NzMesh* parent) :
NzSubMesh(parent)
{
}

NzStaticMesh::NzStaticMesh(const NzMesh* parent, const NzVertexBuffer* vertexBuffer, const NzVertexDeclaration* vertexDeclaration, const NzIndexBuffer* indexBuffer) :
NzSubMesh(parent)
{
	#ifdef NAZARA_DEBUG
	if (!Create(vertexBuffer, vertexDeclaration, indexBuffer))
	{
		NazaraError("Failed to create mesh");
		throw std::runtime_error("Constructor failed");
	}
	#else
	Create(vertexBuffer, vertexDeclaration, indexBuffer);
	#endif
}

NzStaticMesh::~NzStaticMesh()
{
	Destroy();
}

bool NzStaticMesh::Create(const NzVertexBuffer* vertexBuffer, const NzVertexDeclaration* vertexDeclaration, const NzIndexBuffer* indexBuffer)
{
	Destroy();

	#if NAZARA_UTILITY_SAFE
	if (!vertexBuffer)
	{
		NazaraError("Vertex buffer is null");
		return false;
	}

	if (!vertexDeclaration)
	{
		NazaraError("Vertex declaration is null");
		return false;
	}
	#endif

	if (indexBuffer)
	{
		m_indexBuffer = indexBuffer;
		m_indexBuffer->AddResourceReference();
	}

	m_vertexBuffer = vertexBuffer;
	m_vertexBuffer->AddResourceReference();

	m_vertexDeclaration = vertexDeclaration;
	m_vertexDeclaration->AddResourceReference();

	return true;
}

void NzStaticMesh::Destroy()
{
	if (m_indexBuffer)
	{
		m_indexBuffer->RemoveResourceReference();
		m_indexBuffer = nullptr;
	}

	if (m_vertexBuffer)
	{
		m_vertexBuffer->RemoveResourceReference();
		m_vertexBuffer = nullptr;
	}

	if (m_vertexDeclaration)
	{
		m_vertexDeclaration->RemoveResourceReference();
		m_vertexDeclaration = nullptr;
	}
}

nzAnimationType NzStaticMesh::GetAnimationType() const
{
	return nzAnimationType_Static;
}

unsigned int NzStaticMesh::GetFrameCount() const
{
	return 1;
}

const NzIndexBuffer* NzStaticMesh::GetIndexBuffer() const
{
	return m_indexBuffer;
}

nzPrimitiveType NzStaticMesh::GetPrimitiveType() const
{
	return m_primitiveType;
}

const NzVertexBuffer* NzStaticMesh::GetVertexBuffer() const
{
	return m_vertexBuffer;
}

const NzVertexDeclaration* NzStaticMesh::GetVertexDeclaration() const
{
	return m_vertexDeclaration;
}

bool NzStaticMesh::IsAnimated() const
{
	return false;
}

bool NzStaticMesh::IsValid() const
{
	return m_vertexBuffer != nullptr && m_vertexDeclaration != nullptr;
}

void NzStaticMesh::SetPrimitiveType(nzPrimitiveType primitiveType)
{
	m_primitiveType = primitiveType;
}

void NzStaticMesh::AnimateImpl(unsigned int frameA, unsigned int frameB, float interpolation)
{
	NazaraUnused(frameA);
	NazaraUnused(frameB);
	NazaraUnused(interpolation);

	// Le safe mode est censé nous protéger de cet appel
	NazaraError("Static mesh have no animation, please enable safe mode");
}
