// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Loaders/MD2.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Math/Basic.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Utility/Animation.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/KeyframeMesh.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/Loaders/MD2/Constants.hpp>
#include <cstddef>
#include <cstring>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace
{
	/// Loader de mesh
	bool CheckMesh(NzInputStream& stream, const NzMeshParams& parameters)
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

	bool LoadMesh(NzMesh* mesh, NzInputStream& stream, const NzMeshParams& parameters)
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
		if (parameters.animated)
			mesh->CreateKeyframe();
		else
			mesh->CreateStatic();

		if (!mesh->IsValid()) // Ne devrait jamais échouer
		{
			NazaraInternalError("Failed to create mesh");
			return false;
		}

		mesh->SetAnimation(stream.GetPath()); // Même fichier

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
		std::unique_ptr<NzIndexBuffer> indexBuffer(new NzIndexBuffer(header.num_tris * 3, false, parameters.storage, nzBufferUsage_Static));

		/// Lecture des triangles
		std::vector<md2_triangle> triangles(header.num_tris);

		stream.SetCursorPos(header.offset_tris);
		stream.Read(&triangles[0], header.num_tris*sizeof(md2_triangle));

		NzBufferMapper<NzIndexBuffer> indexMapper(indexBuffer.get(), nzBufferAccess_DiscardAndWrite);
		nzUInt16* index = reinterpret_cast<nzUInt16*>(indexMapper.GetPointer());

		for (unsigned int i = 0; i < header.num_tris; ++i)
		{
			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(&triangles[i].vertices[0], sizeof(nzUInt16));
			NzByteSwap(&triangles[i].texCoords[0], sizeof(nzUInt16));

			NzByteSwap(&triangles[i].vertices[1], sizeof(nzUInt16));
			NzByteSwap(&triangles[i].texCoords[1], sizeof(nzUInt16));

			NzByteSwap(&triangles[i].vertices[2], sizeof(nzUInt16));
			NzByteSwap(&triangles[i].texCoords[2], sizeof(nzUInt16));
			#endif

			// On respécifie le triangle dans le bon ordre
			*index++ = triangles[i].vertices[0];
			*index++ = triangles[i].vertices[2];
			*index++ = triangles[i].vertices[1];
		}

		indexMapper.Unmap();

		/// Lecture des coordonnées de texture
		std::vector<md2_texCoord> texCoords(header.num_st);

		stream.SetCursorPos(header.offset_st);
		stream.Read(&texCoords[0], header.num_st*sizeof(md2_texCoord));

		#ifdef NAZARA_BIG_ENDIAN
		for (unsigned int i = 0; i < header.num_st; ++i)
		{
			NzByteSwap(&texCoords[i].u, sizeof(nzInt16));
			NzByteSwap(&texCoords[i].v, sizeof(nzInt16));
		}
		#endif

		const unsigned int indexFix[3] = {0, 2, 1}; // Pour respécifier les indices dans le bon ordre

		// Pour que le modèle soit correctement aligné, on génère un quaternion que nous appliquerons à chacune des vertices
		NzQuaternionf rotationQuat = NzEulerAnglesf(-90.f, 90.f, 0.f);

		if (parameters.animated)
		{
			std::unique_ptr<NzVertexBuffer> vertexBuffer(new NzVertexBuffer(NzMesh::GetDeclaration(), header.num_vertices, parameters.storage, nzBufferUsage_Dynamic));
			std::unique_ptr<NzKeyframeMesh> subMesh(new NzKeyframeMesh(mesh));
			if (!subMesh->Create(vertexBuffer.get(), header.num_frames))
			{
				NazaraError("Failed to create SubMesh");
				return false;
			}

			subMesh->SetIndexBuffer(indexBuffer.release());

			vertexBuffer->SetPersistent(false);
			vertexBuffer.release();

			/// Chargement des frames
			stream.SetCursorPos(header.offset_frames);

			std::unique_ptr<md2_vertex[]> vertices(new md2_vertex[header.num_vertices]);
			for (unsigned int f = 0; f < header.num_frames; ++f)
			{
				NzVector3f scale, translate;

				stream.Read(scale, sizeof(NzVector3f));
				stream.Read(translate, sizeof(NzVector3f));
				stream.Read(nullptr, 16*sizeof(char)); // On avance en ignorant le nom de la frame (Géré par l'animation)
				stream.Read(vertices.get(), header.num_vertices*sizeof(md2_vertex));

				#ifdef NAZARA_BIG_ENDIAN
				NzByteSwap(&scale.x, sizeof(float));
				NzByteSwap(&scale.y, sizeof(float));
				NzByteSwap(&scale.z, sizeof(float));

				NzByteSwap(&translate.x, sizeof(float));
				NzByteSwap(&translate.y, sizeof(float));
				NzByteSwap(&translate.z, sizeof(float));
				#endif

				for (unsigned int v = 0; v < header.num_vertices; ++v)
				{
					const md2_vertex& vert = vertices[v];
					NzVector3f position = rotationQuat * NzVector3f(vert.x * scale.x + translate.x, vert.y * scale.y + translate.y, vert.z * scale.z + translate.z);

					subMesh->SetNormal(f, v, rotationQuat * md2Normals[vert.n]);
					subMesh->SetPosition(f, v, position);
				}
			}

			/// Chargement des coordonnées de texture
			for (unsigned int i = 0; i < header.num_tris; ++i)
			{
				for (unsigned int j = 0; j < 3; ++j)
				{
					const unsigned int fixedIndex = indexFix[j];
					const md2_texCoord& texC = texCoords[triangles[i].texCoords[fixedIndex]];
					subMesh->SetTexCoords(triangles[i].vertices[fixedIndex], NzVector2f(static_cast<float>(texC.u) / header.skinwidth, 1.f - static_cast<float>(texC.v)/header.skinheight));
				}
			}

			subMesh->SetMaterialIndex(0);
			mesh->AddSubMesh(subMesh.release());
		}
		else
		{
			std::unique_ptr<NzVertexBuffer> vertexBuffer(new NzVertexBuffer(NzMesh::GetDeclaration(), header.num_vertices, parameters.storage, nzBufferUsage_Static));
			std::unique_ptr<NzStaticMesh> subMesh(new NzStaticMesh(mesh));
			if (!subMesh->Create(vertexBuffer.get()))
			{
				NazaraError("Failed to create SubMesh");
				return false;
			}

			subMesh->SetIndexBuffer(indexBuffer.release());

			/// Chargement des vertices
			stream.SetCursorPos(header.offset_frames);

			std::unique_ptr<md2_vertex[]> vertices(new md2_vertex[header.num_vertices]);
			NzVector3f scale, translate;
			stream.Read(scale, sizeof(NzVector3f));
			stream.Read(translate, sizeof(NzVector3f));
			stream.Read(nullptr, 16*sizeof(char)); // On avance en ignorant le nom de la frame (Géré par l'animation)
			stream.Read(vertices.get(), header.num_vertices*sizeof(md2_vertex));

			#ifdef NAZARA_BIG_ENDIAN
			NzByteSwap(&scale.x, sizeof(float));
			NzByteSwap(&scale.y, sizeof(float));
			NzByteSwap(&scale.z, sizeof(float));

			NzByteSwap(&translate.x, sizeof(float));
			NzByteSwap(&translate.y, sizeof(float));
			NzByteSwap(&translate.z, sizeof(float));
			#endif

			NzBufferMapper<NzVertexBuffer> vertexMapper(vertexBuffer.get(), nzBufferAccess_DiscardAndWrite);
			NzMeshVertex* vertex = reinterpret_cast<NzMeshVertex*>(vertexMapper.GetPointer());

			/// Chargement des coordonnées de texture
			for (unsigned int i = 0; i < header.num_tris; ++i)
			{
				for (unsigned int j = 0; j < 3; ++j)
				{
					const unsigned int fixedIndex = indexFix[j];
					const md2_texCoord& texC = texCoords[triangles[i].texCoords[fixedIndex]];
					vertex[triangles[i].vertices[fixedIndex]].uv.Set(static_cast<float>(texC.u) / header.skinwidth, 1.f - static_cast<float>(texC.v)/header.skinheight);
				}
			}

			for (unsigned int v = 0; v < header.num_vertices; ++v)
			{
				const md2_vertex& vert = vertices[v];
				NzVector3f position = rotationQuat * NzVector3f(vert.x * scale.x + translate.x, vert.y * scale.y + translate.y, vert.z * scale.z + translate.z);

				vertex->normal = rotationQuat * md2Normals[vert.n];
				vertex->position = position;

				vertex++;
			}

			vertexMapper.Unmap();

			vertexBuffer->SetPersistent(false);
			vertexBuffer.release();

			subMesh->SetMaterialIndex(0);
			mesh->AddSubMesh(subMesh.release());
		}

		return true;
	}

	/// Loader d'animations
	bool CheckAnim(NzInputStream& stream, const NzAnimationParams& parameters)
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

	bool LoadAnim(NzAnimation* animation, NzInputStream& stream, const NzAnimationParams& parameters)
	{
		md2_header header;
		if (stream.Read(&header, sizeof(md2_header)) != sizeof(md2_header))
		{
			NazaraError("Failed to read header");
			return false;
		}

		#ifdef NAZARA_BIG_ENDIAN
		NzByteSwap(&header.framesize, sizeof(nzUInt32));
		NzByteSwap(&header.num_frames, sizeof(nzUInt32));
		NzByteSwap(&header.offset_frames, sizeof(nzUInt32));
		NzByteSwap(&header.offset_end, sizeof(nzUInt32));
		#endif

		if (stream.GetSize() < header.offset_end)
		{
			NazaraError("Incomplete MD2 file");
			return false;
		}

		unsigned int startFrame = std::min(parameters.startFrame, static_cast<unsigned int>(header.num_frames-1));
		unsigned int endFrame = std::min(parameters.endFrame, static_cast<unsigned int>(header.num_frames-1));

		unsigned int frameCount = endFrame - startFrame + 1;
		if (!animation->CreateKeyframe(frameCount))
		{
			NazaraInternalError("Failed to create animaton");
			return false;
		}

		// Le MD2 requiert une interpolation de la dernière à la première frame (en cas de loop)
		animation->EnableLoopPointInterpolation(true);

		// Décodage des séquences
		///TODO: Optimiser le calcul
		char last[16];

		stream.SetCursorPos(header.offset_frames + startFrame*header.framesize + 2*sizeof(NzVector3f));
		stream.Read(last, 16*sizeof(char));

		int pos = std::strlen(last)-1;
		for (unsigned int j = 0; j < 2; ++j)
		{
			if (!std::isdigit(last[pos]))
				break;

			pos--;
		}
		last[pos+1] = '\0';

		NzSequence sequence;
		sequence.firstFrame = startFrame;
		sequence.frameCount = 0;
		sequence.frameRate = 10; // Par défaut pour les animations MD2
		sequence.name = last;

		char name[16];
		for (unsigned int i = startFrame; i <= endFrame; ++i)
		{
			stream.SetCursorPos(header.offset_frames + i*header.framesize + 2*sizeof(NzVector3f));
			stream.Read(name, 16*sizeof(char));

			pos = std::strlen(name)-1;
			for (unsigned int j = 0; j < 2; ++j)
			{
				if (!std::isdigit(name[pos]))
					break;

				pos--;
			}
			name[pos+1] = '\0';

			if (std::strcmp(name, last) != 0) // Si les deux frames n'ont pas le même nom
			{
				std::strcpy(last, name);

				// Alors on enregistre la séquence actuelle
				animation->AddSequence(sequence);

				// Et on initialise la séquence suivante
				sequence.firstFrame = i;
				sequence.frameCount = 0;
				sequence.name = last;
			}

			sequence.frameCount++;
		}
		// On ajoute la dernière frame (Qui n'a pas été traitée par la boucle)
		animation->AddSequence(sequence);

		return true;
	}
}

void NzLoaders_MD2_Register()
{
	NzAnimationLoader::RegisterLoader("md2", CheckAnim, LoadAnim);
	NzMeshLoader::RegisterLoader("md2", CheckMesh, LoadMesh);
}

void NzLoaders_MD2_Unregister()
{
	NzAnimationLoader::UnregisterLoader("md2", CheckAnim, LoadAnim);
	NzMeshLoader::UnregisterLoader("md2", CheckMesh, LoadMesh);
}
