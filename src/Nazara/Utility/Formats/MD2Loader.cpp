// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Formats/MD2Loader.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Stream.hpp>
#include <Nazara/Math/Algorithm.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/MaterialData.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/Formats/MD2Constants.hpp>
#include <cstddef>
#include <cstring>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	namespace
	{
		bool IsSupported(const String& extension)
		{
			return (extension == "md2");
		}

		Ternary Check(Stream& stream, const MeshParams& parameters)
		{
			bool skip;
			if (parameters.custom.GetBooleanParameter("SkipNativeMD2Loader", &skip) && skip)
				return Ternary_False;

			UInt32 magic[2];
			if (stream.Read(&magic[0], 2*sizeof(UInt32)) == 2*sizeof(UInt32))
			{
				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(&magic[0], sizeof(UInt32));
				SwapBytes(&magic[1], sizeof(UInt32));
				#endif

				if (magic[0] == md2Ident && magic[1] == 8)
					return Ternary_True;
			}

			return Ternary_False;
		}

		bool Load(Mesh* mesh, Stream& stream, const MeshParams& parameters)
		{
			MD2_Header header;
			if (stream.Read(&header, sizeof(MD2_Header)) != sizeof(MD2_Header))
			{
				NazaraError("Failed to read header");
				return false;
			}

			#ifdef NAZARA_BIG_ENDIAN
			SwapBytes(&header.skinwidth, sizeof(UInt32));
			SwapBytes(&header.skinheight, sizeof(UInt32));
			SwapBytes(&header.framesize, sizeof(UInt32));
			SwapBytes(&header.num_skins, sizeof(UInt32));
			SwapBytes(&header.num_vertices, sizeof(UInt32));
			SwapBytes(&header.num_st, sizeof(UInt32));
			SwapBytes(&header.num_tris, sizeof(UInt32));
			SwapBytes(&header.num_glcmds, sizeof(UInt32));
			SwapBytes(&header.num_frames, sizeof(UInt32));
			SwapBytes(&header.offset_skins, sizeof(UInt32));
			SwapBytes(&header.offset_st, sizeof(UInt32));
			SwapBytes(&header.offset_tris, sizeof(UInt32));
			SwapBytes(&header.offset_frames, sizeof(UInt32));
			SwapBytes(&header.offset_glcmds, sizeof(UInt32));
			SwapBytes(&header.offset_end, sizeof(UInt32));
			#endif

			if (stream.GetSize() < header.offset_end)
			{
				NazaraError("Incomplete MD2 file");
				return false;
			}

			/// Création du mesh
			// Le moteur ne supporte plus les animations image-clé, nous ne pouvons charger qu'en statique
			if (!mesh->CreateStatic()) // Ne devrait jamais échouer
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
					String baseDir = stream.GetDirectory();
					char skin[68];
					for (unsigned int i = 0; i < header.num_skins; ++i)
					{
						stream.Read(skin, 68*sizeof(char));

						ParameterList matData;
						matData.SetParameter(MaterialData::DiffuseTexturePath, baseDir + skin);

						mesh->SetMaterialData(i, std::move(matData));
					}
				}
			}

			/// Chargement des submesh
			// Actuellement le loader ne charge qu'un submesh
			IndexBufferRef indexBuffer = IndexBuffer::New(false, header.num_tris*3, parameters.storage, BufferUsage_Static);

			/// Lecture des triangles
			std::vector<MD2_Triangle> triangles(header.num_tris);

			stream.SetCursorPos(header.offset_tris);
			stream.Read(&triangles[0], header.num_tris*sizeof(MD2_Triangle));

			BufferMapper<IndexBuffer> indexMapper(indexBuffer, BufferAccess_DiscardAndWrite);
			UInt16* index = static_cast<UInt16*>(indexMapper.GetPointer());

			for (unsigned int i = 0; i < header.num_tris; ++i)
			{
				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(&triangles[i].vertices[0], sizeof(UInt16));
				SwapBytes(&triangles[i].texCoords[0], sizeof(UInt16));

				SwapBytes(&triangles[i].vertices[1], sizeof(UInt16));
				SwapBytes(&triangles[i].texCoords[1], sizeof(UInt16));

				SwapBytes(&triangles[i].vertices[2], sizeof(UInt16));
				SwapBytes(&triangles[i].texCoords[2], sizeof(UInt16));
				#endif

				// On respécifie le triangle dans l'ordre attendu
				*index++ = triangles[i].vertices[0];
				*index++ = triangles[i].vertices[2];
				*index++ = triangles[i].vertices[1];
			}

			indexMapper.Unmap();

			if (parameters.optimizeIndexBuffers)
				indexBuffer->Optimize();

			/// Lecture des coordonnées de texture
			std::vector<MD2_TexCoord> texCoords(header.num_st);

			stream.SetCursorPos(header.offset_st);
			stream.Read(&texCoords[0], header.num_st*sizeof(MD2_TexCoord));

			#ifdef NAZARA_BIG_ENDIAN
			for (unsigned int i = 0; i < header.num_st; ++i)
			{
				SwapBytes(&texCoords[i].u, sizeof(Int16));
				SwapBytes(&texCoords[i].v, sizeof(Int16));
			}
			#endif

			VertexBufferRef vertexBuffer = VertexBuffer::New(VertexDeclaration::Get(VertexLayout_XYZ_Normal_UV_Tangent), header.num_vertices, parameters.storage, BufferUsage_Static);
			StaticMeshRef subMesh = StaticMesh::New(mesh);
			if (!subMesh->Create(vertexBuffer))
			{
				NazaraError("Failed to create SubMesh");
				return false;
			}

			/// Chargement des vertices
			stream.SetCursorPos(header.offset_frames);

			std::unique_ptr<MD2_Vertex[]> vertices(new MD2_Vertex[header.num_vertices]);
			Vector3f scale, translate;
			stream.Read(scale, sizeof(Vector3f));
			stream.Read(translate, sizeof(Vector3f));
			stream.Read(nullptr, 16*sizeof(char)); // Nom de la frame, inutile ici
			stream.Read(vertices.get(), header.num_vertices*sizeof(MD2_Vertex));

			#ifdef NAZARA_BIG_ENDIAN
			SwapBytes(&scale.x, sizeof(float));
			SwapBytes(&scale.y, sizeof(float));
			SwapBytes(&scale.z, sizeof(float));

			SwapBytes(&translate.x, sizeof(float));
			SwapBytes(&translate.y, sizeof(float));
			SwapBytes(&translate.z, sizeof(float));
			#endif

			constexpr float ScaleAdjust = 1.f / 27.8f; // Make a 50 Quake 2 units character a 1.8 unit long

			scale *= ScaleAdjust;
			translate *= ScaleAdjust;

			BufferMapper<VertexBuffer> vertexMapper(vertexBuffer, BufferAccess_DiscardAndWrite);
			MeshVertex* vertex = static_cast<MeshVertex*>(vertexMapper.GetPointer());

			/// Chargement des coordonnées de texture
			const unsigned int indexFix[3] = {0, 2, 1}; // Pour respécifier les indices dans le bon ordre
			for (unsigned int i = 0; i < header.num_tris; ++i)
			{
				for (unsigned int j = 0; j < 3; ++j)
				{
					const unsigned int fixedIndex = indexFix[j];
					const MD2_TexCoord& texC = texCoords[triangles[i].texCoords[fixedIndex]];
					float u = static_cast<float>(texC.u) / header.skinwidth;
					float v = static_cast<float>(texC.v) / header.skinheight;

					vertex[triangles[i].vertices[fixedIndex]].uv.Set(u, (parameters.flipUVs) ? 1.f - v : v);
				}
			}

			/// Chargement des positions
			// Pour que le modèle soit correctement aligné, on génère un quaternion que nous appliquerons à chacune des vertices
			Quaternionf rotationQuat = EulerAnglesf(-90.f, 90.f, 0.f);
			Nz::Matrix4f matrix = Matrix4f::Transform(translate, rotationQuat, scale);
			matrix *= parameters.matrix;

			for (unsigned int v = 0; v < header.num_vertices; ++v)
			{
				const MD2_Vertex& vert = vertices[v];
				Vector3f position = matrix * Vector3f(vert.x, vert.y, vert.z);

				vertex->position = position;
				vertex->normal = rotationQuat * md2Normals[vert.n];

				vertex++;
			}

			vertexMapper.Unmap();

			subMesh->SetIndexBuffer(indexBuffer);
			subMesh->SetMaterialIndex(0);

			subMesh->GenerateAABB();
			subMesh->GenerateTangents();

			mesh->AddSubMesh(subMesh);

			if (parameters.center)
				mesh->Recenter();

			return true;
		}
	}

	namespace Loaders
	{
		void RegisterMD2()
		{
			MeshLoader::RegisterLoader(IsSupported, Check, Load);
		}

		void UnregisterMD2()
		{
			MeshLoader::UnregisterLoader(IsSupported, Check, Load);
		}
	}
}
