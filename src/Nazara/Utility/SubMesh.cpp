// Copyright (C) 2017 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/SubMesh.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/String.hpp>
#include <Nazara/Utility/Config.hpp>
#include <Nazara/Utility/TriangleIterator.hpp>
#include <Nazara/Utility/VertexMapper.hpp>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	SubMesh::SubMesh(const Mesh* parent) :
	RefCounted(false), // Un SubMesh n'est pas persistant par défaut
	m_primitiveMode(PrimitiveMode_TriangleList),
	m_parent(parent),
	m_matIndex(0)
	{
	}

	SubMesh::~SubMesh()
	{
		OnSubMeshRelease(this);
	}

	void SubMesh::GenerateNormals()
	{
		VertexMapper mapper(this);
		unsigned int vertexCount = GetVertexCount();

		SparsePtr<Vector3f> normals = mapper.GetComponentPtr<Vector3f>(VertexComponent_Normal);
		SparsePtr<Vector3f> positions = mapper.GetComponentPtr<Vector3f>(VertexComponent_Position);

		for (unsigned int i = 0; i < vertexCount; ++i)
			normals[i].MakeZero();

		TriangleIterator iterator(this);
		do
		{
			Vector3f pos0 = positions[iterator[0]];

			Vector3f dv[2];
			dv[0] = positions[iterator[1]] - pos0;
			dv[1] = positions[iterator[2]] - pos0;

			Vector3f normal = dv[0].CrossProduct(dv[1]);

			for (unsigned int i = 0; i < 3; ++i)
				normals[iterator[i]] += normal;
		}
		while (iterator.Advance());

		for (unsigned int i = 0; i < vertexCount; ++i)
			normals[i].Normalize();
	}

	void SubMesh::GenerateNormalsAndTangents()
	{
		VertexMapper mapper(this);
		unsigned int vertexCount = GetVertexCount();

		SparsePtr<Vector3f> normals = mapper.GetComponentPtr<Vector3f>(VertexComponent_Normal);
		SparsePtr<Vector3f> positions = mapper.GetComponentPtr<Vector3f>(VertexComponent_Position);
		SparsePtr<Vector3f> tangents = mapper.GetComponentPtr<Vector3f>(VertexComponent_Tangent);
		SparsePtr<Vector2f> texCoords = mapper.GetComponentPtr<Vector2f>(VertexComponent_TexCoord);

		for (unsigned int i = 0; i < vertexCount; ++i)
		{
			normals[i].MakeZero();
			tangents[i].MakeZero();
		}

		TriangleIterator iterator(this);
		do
		{
			Vector3f pos0 = positions[iterator[0]];

			Vector3f dv[2];
			dv[0] = positions[iterator[1]] - pos0;
			dv[1] = positions[iterator[2]] - pos0;

			Vector3f normal = dv[0].CrossProduct(dv[1]);

			Vector2f uv0 = texCoords[iterator[0]];

			Vector2f duv[2];
			duv[0] = texCoords[iterator[1]] - uv0;
			duv[1] = texCoords[iterator[2]] - uv0;

			float coef = 1.f / (duv[0].x*duv[1].y - duv[1].x*duv[0].y);

			Vector3f tangent;
			tangent.x = coef * (dv[0].x*duv[1].y + dv[1].x*(-duv[0].y));
			tangent.y = coef * (dv[0].y*duv[1].y + dv[1].y*(-duv[0].y));
			tangent.z = coef * (dv[0].z*duv[1].y + dv[1].z*(-duv[0].y));

			for (unsigned int i = 0; i < 3; ++i)
			{
				UInt32 index = iterator[i];
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

	void SubMesh::GenerateTangents()
	{
		VertexMapper mapper(this);

		SparsePtr<Vector3f> normals = mapper.GetComponentPtr<Vector3f>(VertexComponent_Normal);
		SparsePtr<Vector3f> positions = mapper.GetComponentPtr<Vector3f>(VertexComponent_Position);
		SparsePtr<Vector3f> tangents = mapper.GetComponentPtr<Vector3f>(VertexComponent_Tangent);
		SparsePtr<Vector2f> texCoords = mapper.GetComponentPtr<Vector2f>(VertexComponent_TexCoord);

		TriangleIterator iterator(this);
		do
		{
			Vector3f pos0 = positions[iterator[0]];
			Vector2f uv0 = texCoords[iterator[0]];
			Vector2f uv1 = texCoords[iterator[1]];
			Vector2f uv2 = texCoords[iterator[2]];

			Vector3f dv[2];
			dv[0] = positions[iterator[1]] - pos0;
			dv[1] = positions[iterator[2]] - pos0;

			float ds[2];
			ds[0] = uv1.x - uv0.x;
			ds[1] = uv2.x - uv0.x;

			Vector3f ppt;
			ppt.x = ds[0]*dv[1].x - dv[0].x*ds[1];
			ppt.y = ds[0]*dv[1].y - dv[0].y*ds[1];
			ppt.z = ds[0]*dv[1].z - dv[0].z*ds[1];
			ppt.Normalize();

			for (unsigned int i = 0; i < 3; ++i)
			{
				Vector3f normal = normals[iterator[i]];
				float d = ppt.DotProduct(normal);

				tangents[iterator[i]] = ppt - (d * normal);
			}
		}
		while (iterator.Advance());
	}

	const Mesh* SubMesh::GetParent() const
	{
		return m_parent;
	}

	PrimitiveMode SubMesh::GetPrimitiveMode() const
	{
		return m_primitiveMode;
	}

	unsigned int SubMesh::GetTriangleCount() const
	{
		const IndexBuffer* indexBuffer = GetIndexBuffer();
		unsigned int indexCount;
		if (indexBuffer)
			indexCount = indexBuffer->GetIndexCount();
		else
			indexCount = GetVertexCount();

		switch (m_primitiveMode)
		{
			case PrimitiveMode_LineList:
			case PrimitiveMode_LineStrip:
			case PrimitiveMode_PointList:
				return 0;

			case PrimitiveMode_TriangleFan:
				return (indexCount - 1) / 2;

			case PrimitiveMode_TriangleList:
				return indexCount / 3;

			case PrimitiveMode_TriangleStrip:
				return indexCount - 2;
		}

		NazaraError("Primitive mode not handled (0x" + String::Number(m_primitiveMode, 16) + ')');
		return 0;
	}

	UInt32 SubMesh::GetMaterialIndex() const
	{
		return m_matIndex;
	}

	void SubMesh::SetPrimitiveMode(PrimitiveMode mode)
	{
		m_primitiveMode = mode;
	}

	void SubMesh::SetMaterialIndex(UInt32 matIndex)
	{
		m_matIndex = matIndex;
	}
}
