// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Core/Error.hpp>
#include <stdexcept>
#include <Nazara/Utility/Debug.hpp>

NzStaticMesh::NzStaticMesh(const NzMesh* parent) :
NzSubMesh(parent)
{
}

NzStaticMesh::NzStaticMesh(const NzMesh* parent, const NzVertexDeclaration* vertexDeclaration, NzVertexBuffer* vertexBuffer, NzIndexBuffer* indexBuffer) :
NzSubMesh(parent)
{
	#ifdef NAZARA_DEBUG
	if (!Create(vertexDeclaration, vertexBuffer, indexBuffer))
	{
		NazaraError("Failed to create mesh");
		throw std::runtime_error("Constructor failed");
	}
	#else
	Create(vertexDeclaration, vertexBuffer, indexBuffer);
	#endif
}

NzStaticMesh::~NzStaticMesh()
{
	Destroy();
}

bool NzStaticMesh::Create(const NzVertexDeclaration* vertexDeclaration, NzVertexBuffer* vertexBuffer, NzIndexBuffer* indexBuffer)
{
	Destroy();

	#if NAZARA_UTILITY_SAFE
	if (!vertexDeclaration)
	{
		NazaraError("Invalid vertex declaration");
		return false;
	}

	if (!vertexBuffer)
	{
		NazaraError("Invalid vertex buffer");
		return false;
	}
	#endif

	if (indexBuffer)
		indexBuffer->AddResourceListener(this);

	m_indexBuffer = indexBuffer;

	m_vertexBuffer = vertexBuffer;
	m_vertexBuffer->AddResourceListener(this);

	m_vertexDeclaration = vertexDeclaration;
	m_vertexDeclaration->AddResourceListener(this);

	return true;
}

void NzStaticMesh::Destroy()
{
	m_aabb.SetNull();

	if (m_indexBuffer)
	{
		m_indexBuffer->RemoveResourceListener(this);
		m_indexBuffer = nullptr;
	}

	if (m_vertexBuffer)
	{
		m_vertexBuffer->RemoveResourceListener(this);
		m_vertexBuffer = nullptr;
	}

	if (m_vertexDeclaration)
	{
		m_vertexDeclaration->RemoveResourceListener(this);
		m_vertexDeclaration = nullptr;
	}
}

bool NzStaticMesh::GenerateAABB()
{
	if (!m_aabb.IsNull())
		return true;

	const NzVertexElement* position = m_vertexDeclaration->GetElement(nzElementStream_VertexData, nzElementUsage_Position);
	if (position && position->type == nzElementType_Float3) // Si nous avons des positions du type Vec3
	{
		// On lock le buffer pour itérer sur toutes les positions et composer notre AABB
		nzUInt8* buffer = reinterpret_cast<nzUInt8*>(m_vertexBuffer->Map(nzBufferAccess_ReadOnly));
		if (!buffer)
		{
			NazaraWarning("Failed to lock vertex buffer");
			return false;
		}

		buffer += position->offset;
		unsigned int stride = m_vertexDeclaration->GetStride(nzElementStream_VertexData);
		unsigned int vertexCount = m_vertexBuffer->GetVertexCount();
		for (unsigned int i = 0; i < vertexCount; ++i)
		{
			m_aabb.ExtendTo(*reinterpret_cast<NzVector3f*>(buffer));

			buffer += stride;
		}

		if (!m_vertexBuffer->Unmap())
			NazaraWarning("Failed to unmap vertex buffer");
	}

	return true;
}

const NzAxisAlignedBox& NzStaticMesh::GetAABB() const
{
	return m_aabb;
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

void NzStaticMesh::SetAABB(const NzAxisAlignedBox& aabb)
{
	m_aabb = aabb;
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

void NzStaticMesh::OnResourceReleased(const NzResource* resource, int index)
{
	NazaraUnused(index);

	if (resource == m_indexBuffer)
		m_indexBuffer = nullptr;
	else if (resource == m_vertexBuffer)
		m_vertexBuffer = nullptr;
	else if (resource == m_vertexDeclaration)
		m_vertexDeclaration = nullptr;
	else
		NazaraInternalError("Not listening to " + NzString::Pointer(resource));
}
