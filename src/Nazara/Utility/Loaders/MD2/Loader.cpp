// Copyright (C) 2012 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Loaders/MD2.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Math/Basic.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/Loaders/MD2/Constants.hpp>
#include <Nazara/Utility/Loaders/MD2/Mesh.hpp>
#include <cstddef>
#include <cstring>
#include <Nazara/Utility/Debug.hpp>

namespace
{
	bool NzLoader_MD2_Check(NzInputStream& stream, const NzMeshParams& parameters)
	{
		NazaraUnused(parameters);

		nzUInt32 magic[2];
		if (stream.Read(&magic[0], 2*sizeof(nzUInt32)) != 2*sizeof(nzUInt32))
			return false;

		#if defined(NAZARA_BIG_ENDIAN)
		NzByteSwap(&magic[0], sizeof(nzUInt32));
		NzByteSwap(&magic[1], sizeof(nzUInt32));
		#endif

		return magic[0] == md2Ident && magic[1] == 8;
	}

	bool NzLoader_MD2_Load(NzMesh* mesh, NzInputStream& stream, const NzMeshParams& parameters)
	{
		md2_header header;
		if (stream.Read(&header, sizeof(md2_header)) != sizeof(md2_header))
		{
			NazaraError("Failed to read header");
			return false;
		}

		#if defined(NAZARA_BIG_ENDIAN)
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
		bool animated;
		unsigned int startFrame = NzClamp(parameters.animation.startFrame, 0U, static_cast<unsigned int>(header.num_frames-1));
		unsigned int endFrame = NzClamp(parameters.animation.endFrame, 0U, static_cast<unsigned int>(header.num_frames-1));

		if (parameters.loadAnimations && startFrame != endFrame)
			animated = true;
		else
			animated = false;

		if (!mesh->Create((animated) ? nzAnimationType_Keyframe : nzAnimationType_Static)) // Ne devrait jamais échouer
		{
			NazaraInternalError("Failed to create mesh");
			return false;
		}

		/// Chargement des skins
		if (header.num_skins > 0)
		{
			stream.SetCursorPos(header.offset_skins);
			{
				char skin[68];
				for (unsigned int i = 0; i < header.num_skins; ++i)
				{
					stream.Read(skin, 68*sizeof(char));
					mesh->AddSkin(skin);
				}
			}
		}

		/// Chargement des animations
		if (animated)
		{
			NzAnimation* animation = new NzAnimation;
			if (animation->Create(nzAnimationType_Keyframe, endFrame-startFrame+1))
			{
				// Décodage des séquences
				NzString frameName;

				NzSequence sequence;
				sequence.framePerSecond = 10; // Par défaut pour les animations MD2

				char name[16], last[16];
				stream.SetCursorPos(header.offset_frames + startFrame*header.framesize + offsetof(md2_frame, name));
				stream.Read(last, 16*sizeof(char));

				int pos = std::strlen(last)-1;
				for (unsigned int j = 0; j < 2; ++j)
				{
					if (!std::isdigit(last[pos]))
						break;

					pos--;
				}
				last[pos+1] = '\0';

				unsigned int numFrames = 0;
				for (unsigned int i = startFrame; i <= endFrame; ++i)
				{
					stream.SetCursorPos(header.offset_frames + i*header.framesize + offsetof(md2_frame, name));
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
						// Alors on enregistre la séquence
						sequence.firstFrame = i-numFrames;
						sequence.lastFrame = i-1;
						sequence.name = last;
						animation->AddSequence(sequence);

						std::strcpy(last, name);

						numFrames = 0;
					}

					numFrames++;
				}

				// On ajoute la dernière frame (Qui n'a pas été traitée par la boucle)
				sequence.firstFrame = endFrame-numFrames;
				sequence.lastFrame = endFrame;
				sequence.name = last;
				animation->AddSequence(sequence);

				mesh->SetAnimation(animation);
				animation->SetPersistent(false);
			}
			else
				NazaraInternalError("Failed to create animaton");
		}

		/// Chargement des submesh
		// Actuellement le loader ne charge qu'un submesh
		// TODO: Utiliser les commandes OpenGL pour accélérer le rendu
		NzMD2Mesh* subMesh = new NzMD2Mesh(mesh);
		if (!subMesh->Create(header, stream, parameters))
		{
			NazaraError("Failed to create MD2 mesh");
			return false;
		}

		mesh->AddSubMesh(subMesh);

		return true;
	}
}

void NzLoaders_MD2_Register()
{
	NzMD2Mesh::Initialize();

	NzMeshLoader::RegisterLoader("md2", NzLoader_MD2_Check, NzLoader_MD2_Load);
}

void NzLoaders_MD2_Unregister()
{
	NzMeshLoader::UnregisterLoader("md2", NzLoader_MD2_Check, NzLoader_MD2_Load);

	NzMD2Mesh::Uninitialize();
}
