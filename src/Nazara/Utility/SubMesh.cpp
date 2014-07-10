// Copyright (C) 2014 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/SubMesh.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/TriangleIterator.hpp>
#include <Nazara/Utility/VertexMapper.hpp>
#include <stdexcept>
#include <Nazara/Utility/Debug.hpp>

NzSubMesh::NzSubMesh(const NzMesh* parent) :
NzResource(false), // Un SubMesh n'est pas persistant par défaut
m_primitiveMode(nzPrimitiveMode_TriangleList),
m_parent(parent),
m_matIndex(0)
{
}

NzSubMesh::~NzSubMesh() = default;

void NzSubMesh::GenerateNormals()
{
	NzVertexMapper mapper(this);
	unsigned int vertexCount = mapper.GetVertexCount();

	NzSparsePtr<NzVector3f> normals = mapper.GetComponentPtr<NzVector3f>(nzVertexComponent_Normal);
	NzSparsePtr<NzVector3f> positions = mapper.GetComponentPtr<NzVector3f>(nzVertexComponent_Position);

	for (unsigned int i = 0; i < vertexCount; ++i)
		normals[i].MakeZero();

	NzTriangleIterator iterator(this);
	do
	{
		NzVector3f pos0 = positions[iterator[0]];

		NzVector3f dv[2];
		dv[0] = positions[iterator[1]] - pos0;
		dv[1] = positions[iterator[2]] - pos0;

		NzVector3f normal = dv[0].CrossProduct(dv[1]);

		for (unsigned int i = 0; i < 3; ++i)
			normals[iterator[i]] += normal;
	}
	while (iterator.Advance());

	for (unsigned int i = 0; i < vertexCount; ++i)
		normals[i].Normalize();
}

void NzSubMesh::GenerateNormalsAndTangents()
{
	NzVertexMapper mapper(this);
	unsigned int vertexCount = mapper.GetVertexCount();

	NzSparsePtr<NzVector3f> normals = mapper.GetComponentPtr<NzVector3f>(nzVertexComponent_Normal);
	NzSparsePtr<NzVector3f> positions = mapper.GetComponentPtr<NzVector3f>(nzVertexComponent_Position);
	NzSparsePtr<NzVector3f> tangents = mapper.GetComponentPtr<NzVector3f>(nzVertexComponent_Tangent);
	NzSparsePtr<NzVector2f> texCoords = mapper.GetComponentPtr<NzVector2f>(nzVertexComponent_TexCoord);

	for (unsigned int i = 0; i < vertexCount; ++i)
	{
		normals[i].MakeZero();
		tangents[i].MakeZero();
	}

	NzTriangleIterator iterator(this);
	do
	{
		NzVector3f pos0 = positions[iterator[0]];

		NzVector3f dv[2];
		dv[0] = positions[iterator[1]] - pos0;
		dv[1] = positions[iterator[2]] - pos0;

		NzVector3f normal = dv[0].CrossProduct(dv[1]);

		NzVector2f uv0 = texCoords[iterator[0]];

		NzVector2f duv[2];
		duv[0] = texCoords[iterator[1]] - uv0;
		duv[1] = texCoords[iterator[2]] - uv0;

		float coef = 1.f / (duv[0].x*duv[1].y - duv[1].x*duv[0].y);

		NzVector3f tangent;
		tangent.x = coef * (dv[0].x*duv[1].y + dv[1].x*(-duv[0].y));
		tangent.y = coef * (dv[0].y*duv[1].y + dv[1].y*(-duv[0].y));
		tangent.z = coef * (dv[0].z*duv[1].y + dv[1].z*(-duv[0].y));

		for (unsigned int i = 0; i < 3; ++i)
		{
			nzUInt32 index = iterator[i];
			normals[index] += normal;
			tangents[index] += tangent;
		}
	}
	while (iterator.Advance());

	for (unsigned int i = 0; i < vertexCount; ++i)
	{
		normals[i].Normalize();
		tangents[i].Normalize();
	}
}

void NzSubMesh::GenerateTangents()
{
	NzVertexMapper mapper(this);

	NzSparsePtr<NzVector3f> normals = mapper.GetComponentPtr<NzVector3f>(nzVertexComponent_Normal);
	NzSparsePtr<NzVector3f> positions = mapper.GetComponentPtr<NzVector3f>(nzVertexComponent_Position);
	NzSparsePtr<NzVector3f> tangents = mapper.GetComponentPtr<NzVector3f>(nzVertexComponent_Tangent);
	NzSparsePtr<NzVector2f> texCoords = mapper.GetComponentPtr<NzVector2f>(nzVertexComponent_TexCoord);

	NzTriangleIterator iterator(this);
	do
	{
		NzVector3f pos0 = positions[iterator[0]];
		NzVector2f uv0 = texCoords[iterator[0]];
		NzVector2f uv1 = texCoords[iterator[1]];
		NzVector2f uv2 = texCoords[iterator[2]];

		NzVector3f dv[2];
		dv[0] = positions[iterator[1]] - pos0;
		dv[1] = positions[iterator[2]] - pos0;

		NzVector2f duv[2];
		duv[0] = uv1 - uv0;
		duv[1] = uv2 - uv0;

		float ds[2];
		ds[0] = uv1.x - uv0.x;
		ds[1] = uv2.x - uv0.x;

		NzVector3f ppt;
		ppt.x = ds[0]*dv[1].x - dv[0].x*ds[1];
		ppt.y = ds[0]*dv[1].y - dv[0].y*ds[1];
		ppt.z = ds[0]*dv[1].z - dv[0].z*ds[1];
		ppt.Normalize();

		for (unsigned int i = 0; i < 3; ++i)
		{
			NzVector3f normal = normals[iterator[i]];
			float d = ppt.DotProduct(normal);

			tangents[iterator[i]] = ppt - (d * normal);
		}
	}
	while (iterator.Advance());
}

const NzMesh* NzSubMesh::GetParent() const
{
	return m_parent;
}

nzPrimitiveMode NzSubMesh::GetPrimitiveMode() const
{
	return m_primitiveMode;
}

unsigned int NzSubMesh::GetTriangleCount() const
{
	const NzIndexBuffer* indexBuffer = GetIndexBuffer();
	unsigned int indexCount;
	if (indexBuffer)
		indexCount = indexBuffer->GetIndexCount();
	else
		indexCount = GetVertexCount();

	switch (m_primitiveMode)
	{
		case nzPrimitiveMode_LineList:
		case nzPrimitiveMode_LineStrip:
		case nzPrimitiveMode_PointList:
			return 0;

		case nzPrimitiveMode_TriangleFan:
			return (indexCount - 1) / 2;

		case nzPrimitiveMode_TriangleList:
			return indexCount / 3;

		case nzPrimitiveMode_TriangleStrip:
			return indexCount - 2;
	}

	NazaraError("Primitive mode not handled (0x" + NzString::Number(m_primitiveMode, 16) + ')');
	return 0;
}

unsigned int NzSubMesh::GetMaterialIndex() const
{
	return m_matIndex;
}

void NzSubMesh::SetPrimitiveMode(nzPrimitiveMode mode)
{
	m_primitiveMode = mode;
}

void NzSubMesh::SetMaterialIndex(unsigned int matIndex)
{
	m_matIndex = matIndex;
}
