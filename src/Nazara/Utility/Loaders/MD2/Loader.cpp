// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Loaders/MD2.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Math/Basic.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Utility/KeyframeMesh.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/Loaders/MD2/Constants.hpp>
#include <cstddef>
#include <cstring>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace
{
	bool Check(NzInputStream& stream, const NzMeshParams& parameters)
	{
		NazaraUnused(parameters);

		nzUInt32 magic[2];
		if (stream.Read(&magic[0], 2*sizeof(nzUInt32)) != 2*sizeof(nzUInt32))
			return false;

		#ifdef NAZARA_BIG_ENDIAN
		NzByteSwap(&magic[0], sizeof(nzUInt32));
		NzByteSwap(&magic[1], sizeof(nzUInt32));
		#endif

		return magic[0] == md2Ident && magic[1] == 8;
	}

	bool Load(NzMesh* mesh, NzInputStream& stream, const NzMeshParams& parameters)
	{
		md2_header header;
		if (stream.Read(&header, sizeof(md2_header)) != sizeof(md2_header))
		{
			NazaraError("Failed to read header");
			return false;
		}

		#ifdef NAZARA_BIG_ENDIAN
		NzByteSwap(&header.skinwidth, sizeof(nzUInt32));
		NzByteSwap(&header.skinheight, sizeof(nzUInt32));
		NzByteSwap(&header.framesize, sizeof(nzUInt32));
		NzByteSwap(&header.num_skins, sizeof(nzUInt32));
		NzByteSwap(&header.num_vertices, sizeof(nzUInt32));
		NzByteSwap(&header.num_st, sizeof(nzUInt32));
		NzByteSwap(&header.num_tris, sizeof(nzUInt32));
		NzByteSwap(&header.num_glcmds, sizeof(nzUInt32));
		NzByteSwap(&header.num_frames, sizeof(nzUInt32));
		NzByteSwap(&header.offset_skins, sizeof(nzUInt32));
		NzByteSwap(&header.offset_st, sizeof(nzUInt32));
		NzByteSwap(&header.offset_tris, sizeof(nzUInt32));
		NzByteSwap(&header.offset_frames, sizeof(nzUInt32));
		NzByteSwap(&header.offset_glcmds, sizeof(nzUInt32));
		NzByteSwap(&header.offset_end, sizeof(nzUInt32));
		#endif

		if (stream.GetSize() < header.offset_end)
		{
			NazaraError("Incomplete MD2 file");
			return false;
		}

		/// Création du mesh
		// Animé ou statique, c'est la question
		unsigned int startFrame = NzClamp(parameters.animation.startFrame, 0U, static_cast<unsigned int>(header.num_frames-1));
		unsigned int endFrame = NzClamp(parameters.animation.endFrame, 0U, static_cast<unsigned int>(header.num_frames-1));

		bool animated = (parameters.animated && startFrame != endFrame);
		if (animated)
			mesh->CreateKeyframe();
		else
			mesh->CreateStatic();

		if (!mesh->IsValid()) // Ne devrait jamais échouer
		{
			NazaraInternalError("Failed to create mesh");
			return false;
		}

		/// Chargement des skins
		if (header.num_skins > 0)
		{
			mesh->SetMaterialCount(header.num_skins);
			stream.SetCursorPos(header.offset_skins);
			{
				NzString baseDir = stream.GetDirectory();
				char skin[68];
				for (unsigned int i = 0; i < header.num_skins; ++i)
				{
					stream.Read(skin, 68*sizeof(char));
					mesh->SetMaterial(i, baseDir + skin);
				}
			}
		}

		/// Chargement des submesh
		// Actuellement le loader ne charge qu'un submesh
		// TODO: Utiliser les commandes OpenGL pour créer des indices et accélérer le rendu
		unsigned int frameCount = endFrame - startFrame + 1;
		unsigned int vertexCount = header.num_tris * 3;

		std::unique_ptr<NzVertexBuffer> vertexBuffer(new NzVertexBuffer(NzMesh::GetDeclaration(), vertexCount, parameters.storage, nzBufferUsage_Dynamic));
		std::unique_ptr<NzKeyframeMesh> subMesh(new NzKeyframeMesh(mesh));
		if (!subMesh->Create(vertexBuffer.get(), frameCount))
		{
			NazaraError("Failed to create SubMesh");
			return false;
		}

		vertexBuffer->SetPersistent(false);
		vertexBuffer.release();

		/// Lecture des triangles
		std::vector<md2_triangle> triangles(header.num_tris);

		stream.SetCursorPos(header.offset_tris);
		stream.Read(&triangles[0], header.num_tris*sizeof(md2_triangle));

		#ifdef NAZARA_BIG_ENDIAN
		for (unsigned int i = 0; i < header.num_tris; ++i)
		{
			NzByteSwap(&triangles[i].vertices[0], sizeof(nzUInt16));
			NzByteSwap(&triangles[i].texCoords[0], sizeof(nzUInt16));

			NzByteSwap(&triangles[i].vertices[1], sizeof(nzUInt16));
			NzByteSwap(&triangles[i].texCoords[1], sizeof(nzUInt16));

			NzByteSwap(&triangles[i].vertices[2], sizeof(nzUInt16));
			NzByteSwap(&triangles[i].texCoords[2], sizeof(nzUInt16));
		}
		#endif

		/// Lecture des coordonnées de texture
		std::vector<md2_texCoord> texCoords(header.num_st);

		// Lecture des coordonnées de texture
		stream.SetCursorPos(header.offset_st);
		stream.Read(&texCoords[0], header.num_st*sizeof(md2_texCoord));

		#ifdef NAZARA_BIG_ENDIAN
		for (unsigned int i = 0; i < header.num_st; ++i)
		{
			NzByteSwap(&texCoords[i].u, sizeof(nzInt16));
			NzByteSwap(&texCoords[i].v, sizeof(nzInt16));
		}
		#endif

		/// Chargement des frames
		stream.SetCursorPos(header.offset_frames + header.framesize*startFrame);

		// Pour que le modèle soit correctement aligné, on génère un quaternion que nous appliquerons à chacune des vertices
		NzQuaternionf rotationQuat = NzEulerAnglesf(-90.f, 90.f, 0.f);

		md2_vertex* vertices = new md2_vertex[header.num_vertices];
		for (unsigned int f = 0; f < frameCount; ++f)
		{
			NzVector3f scale, translate;

			stream.Read(scale, sizeof(NzVector3f));
			stream.Read(translate, sizeof(NzVector3f));
			stream.Read(nullptr, 16*sizeof(char));
			stream.Read(vertices, header.num_vertices*sizeof(md2_vertex));

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(&scale.x, sizeof(float));
			NzByteSwap(&scale.y, sizeof(float));
			NzByteSwap(&scale.z, sizeof(float));

			NzByteSwap(&translate.x, sizeof(float));
			NzByteSwap(&translate.y, sizeof(float));
			NzByteSwap(&translate.z, sizeof(float));
			#endif

			NzAxisAlignedBox aabb;
			for (unsigned int t = 0; t < header.num_tris; ++t)
			{
				for (unsigned int v = 0; v < 3; ++v)
				{
					const md2_vertex& vert = vertices[triangles[t].vertices[v]];
					NzVector3f position = rotationQuat * NzVector3f(vert.x * scale.x + translate.x, vert.y * scale.y + translate.y, vert.z * scale.z + translate.z);

					// On fait en sorte d'étendre l'AABB pour qu'il contienne ce sommet
					aabb.ExtendTo(position);

					// Et on finit par copier les éléments dans le buffer
					NzMeshVertex vertex;
					vertex.normal = md2Normals[vert.n];
					vertex.position = position;

					// On ne définit les coordonnées de texture que pour la première frame
					bool firstFrame = (f == 0);
					if (firstFrame)
					{
						const md2_texCoord& texC = texCoords[triangles[t].texCoords[v]];
						vertex.uv.Set(texC.u / static_cast<float>(header.skinwidth), 1.f - texC.v / static_cast<float>(header.skinheight));
					}

					subMesh->SetVertex(vertex, f, vertexCount - (t*3 + v) - 1, firstFrame);
				}
			}

			subMesh->SetAABB(f, aabb);
		}
		delete[] vertices;

		subMesh->Unlock();

		subMesh->SetMaterialIndex(0);
		mesh->AddSubMesh(subMesh.release());

		return true;
	}
}

void NzLoaders_MD2_Register()
{
	NzMeshLoader::RegisterLoader("md2", Check, Load);
}

void NzLoaders_MD2_Unregister()
{
	NzMeshLoader::UnregisterLoader("md2", Check, Load);
}
