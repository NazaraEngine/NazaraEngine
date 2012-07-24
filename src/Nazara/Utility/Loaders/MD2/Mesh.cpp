// Copyright (C) 2011 Jérôme Leclercq
// This file is part of the "Ungine".
// For conditions of distribution and use, see copyright notice in Core.h

#include <Nazara/Utility/Loaders/MD2/Mesh.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Math/Matrix4.hpp>
#include <Nazara/Utility/IndexBuffer.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/VertexBuffer.hpp>
#include <Nazara/Utility/Debug.hpp>

NzMD2Mesh::NzMD2Mesh(const NzMesh* parent) :
NzKeyframeMesh(parent),
m_frames(nullptr),
m_indexBuffer(nullptr),
m_vertexBuffer(nullptr)
{
}

NzMD2Mesh::~NzMD2Mesh()
{
	Destroy();
}

bool NzMD2Mesh::Create(const md2_header& header, NzInputStream& stream, const NzMeshParams& parameters)
{
	Destroy();

	unsigned int startFrame = NzClamp(parameters.animation.startFrame, 0U, static_cast<unsigned int>(header.num_frames-1));
	unsigned int endFrame = NzClamp(parameters.animation.endFrame, 0U, static_cast<unsigned int>(header.num_frames-1));

	m_frameCount = endFrame - startFrame + 1;
	m_vertexCount = header.num_tris*3;

	/// Chargement des vertices
	std::vector<md2_texCoord> texCoords(header.num_st);
	std::vector<md2_triangle> triangles(header.num_tris);

	// Lecture des coordonnées de texture
	stream.SetCursorPos(header.offset_st);
	stream.Read(&texCoords[0], header.num_st*sizeof(md2_texCoord));

	#if NAZARA_BIG_ENDIAN
	for (unsigned int i = 0; i < header.num_st; ++i)
	{
		NzByteSwap(&texCoords[i].u, sizeof(nzInt16));
		NzByteSwap(&texCoords[i].v, sizeof(nzInt16));
	}
	#endif

	stream.SetCursorPos(header.offset_tris);
	stream.Read(&triangles[0], header.num_tris*sizeof(md2_triangle));

	#if NAZARA_BIG_ENDIAN
	for (unsigned int i = 0; i < header.num_tris; ++i)
	{
		NzByteSwap(&triangles[i].vertices[0], sizeof(nzUInt16));
		NzByteSwap(&texCoords[i].texCoords[0], sizeof(nzUInt16));

		NzByteSwap(&triangles[i].vertices[1], sizeof(nzUInt16));
		NzByteSwap(&texCoords[i].texCoords[1], sizeof(nzUInt16));

		NzByteSwap(&triangles[i].vertices[2], sizeof(nzUInt16));
		NzByteSwap(&texCoords[i].texCoords[2], sizeof(nzUInt16));
	}
	#endif

	stream.SetCursorPos(header.offset_frames + header.framesize*startFrame);

	md2_frame frame;
	frame.vertices.resize(header.num_vertices);

	// Pour que le modèle soit correctement aligné, on génère une matrice de rotation que nous appliquerons à chacune des vertices
	NzMatrix4f rotationMatrix = NzMatrix4f::Rotate(NzEulerAnglesf(-90.f, 0.f, -90.f));
	//NzMatrix4f rotationMatrix;
	//rotationMatrix.SetIdentity();

	unsigned int stride = s_declaration.GetStride(nzElementStream_VertexData);

	m_frames = new Frame[m_frameCount];
	for (unsigned int i = 0; i < m_frameCount; ++i)
	{
		stream.Read(&frame.scale, sizeof(NzVector3f));
		stream.Read(&frame.translate, sizeof(NzVector3f));
		stream.Read(&frame.name, 16*sizeof(char));
		stream.Read(&frame.vertices[0], header.num_vertices*sizeof(md2_vertex));

		#if NAZARA_BIG_ENDIAN
		NzByteSwap(&frame.scale.x, sizeof(float));
		NzByteSwap(&frame.scale.y, sizeof(float));
		NzByteSwap(&frame.scale.z, sizeof(float));

		NzByteSwap(&frame.translate.x, sizeof(float));
		NzByteSwap(&frame.translate.y, sizeof(float));
		NzByteSwap(&frame.translate.z, sizeof(float));
		#endif

		m_frames[i].normal = new nzUInt8[m_vertexCount]; // Nous stockons l'indice de la normale plutôt que la normale (gain d'espace)
		m_frames[i].vertices = new NzVector3f[m_vertexCount];
		for (unsigned int t = 0; t < header.num_tris; ++t)
		{
			for (unsigned int v = 0; v < 3; ++v)
			{
				const md2_vertex& vert = frame.vertices[triangles[t].vertices[v]];

				NzVector3f vertex = rotationMatrix * NzVector3f(vert.x * frame.scale.x + frame.translate.x, vert.y * frame.scale.y + frame.translate.y, vert.z * frame.scale.z + frame.translate.z);

				m_frames[i].normal[t*3+v] = vert.n;
				m_frames[i].vertices[t*3+v] = vertex;
			}
		}
	}

	nzBufferStorage storage = (NzBuffer::IsSupported(nzBufferStorage_Hardware) && !parameters.forceSoftware) ? nzBufferStorage_Hardware : nzBufferStorage_Software;

	m_indexBuffer = nullptr; // Pas d'indexbuffer pour l'instant
	m_vertexBuffer = new NzVertexBuffer(m_vertexCount, (3+3+2)*sizeof(float), storage, nzBufferUsage_Dynamic);

	nzUInt8* ptr = reinterpret_cast<nzUInt8*>(m_vertexBuffer->Map(nzBufferAccess_WriteOnly));
	if (!ptr)
	{
		NazaraError("Failed to map vertex buffer");
		Destroy();

		return false;
	}

	// On avance jusqu'aux premières coordonnées de texture
	ptr += s_declaration.GetElement(nzElementStream_VertexData, nzElementUsage_TexCoord)->offset;
	for (unsigned int t = 0; t < header.num_tris; ++t)
	{
		for (unsigned int v = 0; v < 3; ++v)
		{
			const md2_texCoord& texC = texCoords[triangles[t].texCoords[v]];

			NzVector2f* coords = reinterpret_cast<NzVector2f*>(ptr);
			coords->x = texC.u / static_cast<float>(header.skinwidth);
			coords->y = 1.f - texC.v / static_cast<float>(header.skinheight);

			ptr += stride;
		}
	}

	if (!m_vertexBuffer->Unmap())
	{
		NazaraError("Failed to unmap buffer");
		Destroy();

		return false;
	}

	m_vertexBuffer->AddResourceReference();
	m_vertexBuffer->SetPersistent(false);

	AnimateImpl(0, 0, 0.f);

	return true;
}

void NzMD2Mesh::Destroy()
{
	if (m_frames)
	{
		for (unsigned int i = 0; i < m_frameCount; ++i)
		{
			delete[] m_frames[i].normal;
			delete[] m_frames[i].vertices;
		}

		delete[] m_frames;
		m_frames = nullptr;
	}

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
}

nzAnimationType NzMD2Mesh::GetAnimationType() const
{
	if (m_frameCount > 1)
		return nzAnimationType_Keyframe;
	else
		return nzAnimationType_Static;
}

unsigned int NzMD2Mesh::GetFrameCount() const
{
	return m_frameCount;
}

const NzIndexBuffer* NzMD2Mesh::GetIndexBuffer() const
{
	return nullptr;
	//return m_indexBuffer;
}

nzPrimitiveType NzMD2Mesh::GetPrimitiveType() const
{
	return nzPrimitiveType_TriangleList;
}

const NzVertexBuffer* NzMD2Mesh::GetVertexBuffer() const
{
	return m_vertexBuffer;
}

const NzVertexDeclaration* NzMD2Mesh::GetVertexDeclaration() const
{
	return &s_declaration;
}

void NzMD2Mesh::Initialize()
{
	NzVertexElement elements[3];
	elements[0].offset = 0;
	elements[0].type = nzElementType_Float3;
	elements[0].usage = nzElementUsage_Position;

	elements[1].offset = 3*sizeof(float);
	elements[1].type = nzElementType_Float3;
	elements[1].usage = nzElementUsage_Normal;

	elements[2].offset = 3*sizeof(float) + 3*sizeof(float);
	elements[2].type = nzElementType_Float2;
	elements[2].usage = nzElementUsage_TexCoord;

	s_declaration.Create(elements, 3);
}

void NzMD2Mesh::Uninitialize()
{
	s_declaration.Destroy();
}

void NzMD2Mesh::AnimateImpl(unsigned int frameA, unsigned int frameB, float interpolation)
{
	nzUInt8* ptr = reinterpret_cast<nzUInt8*>(m_vertexBuffer->Map(nzBufferAccess_WriteOnly));
	if (!ptr)
	{
		NazaraError("Failed to map vertex buffer");
		return;
	}

	unsigned int stride = s_declaration.GetStride(nzElementStream_VertexData);
	unsigned int positionOffset = s_declaration.GetElement(nzElementStream_VertexData, nzElementUsage_Position)->offset;
	unsigned int normalOffset = s_declaration.GetElement(nzElementStream_VertexData, nzElementUsage_Normal)->offset;

	Frame* fA = &m_frames[frameA];
	Frame* fB = &m_frames[frameB];
	for (unsigned int i = 0; i < m_vertexCount; ++i)
	{
		NzVector3f* position = reinterpret_cast<NzVector3f*>(ptr + positionOffset);
		NzVector3f* normal = reinterpret_cast<NzVector3f*>(ptr + normalOffset);

		*position	= fA->vertices[i] + interpolation * (fB->vertices[i] - fA->vertices[i]);
		*normal		= md2Normals[fA->normal[i]] + interpolation * (md2Normals[fB->normal[i]] - md2Normals[fA->normal[i]]);

		ptr += stride;
	}

	if (!m_vertexBuffer->Unmap())
		NazaraWarning("Failed to unmap vertex buffer, expect mesh corruption");
}

NzVertexDeclaration NzMD2Mesh::s_declaration;
