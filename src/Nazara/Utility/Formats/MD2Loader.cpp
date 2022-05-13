// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Utility module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Utility/Formats/MD2Loader.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/Stream.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <Nazara/Utility/MaterialData.hpp>
#include <Nazara/Utility/Mesh.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/VertexMapper.hpp>
#include <Nazara/Utility/Formats/MD2Constants.hpp>
#include <Nazara/Utils/Endianness.hpp>
#include <array>
#include <cassert>
#include <memory>
#include <Nazara/Utility/Debug.hpp>

namespace Nz
{
	namespace
	{
		bool IsMD2Supported(const std::string_view& extension)
		{
			return (extension == "md2");
		}

		Ternary CheckMD2(Stream& stream, const MeshParams& parameters)
		{
			bool skip;
			if (parameters.custom.GetBooleanParameter("SkipBuiltinMD2Loader", &skip) && skip)
				return Ternary::False;

			UInt32 magic[2];
			if (stream.Read(&magic[0], 2*sizeof(UInt32)) == 2*sizeof(UInt32))
			{
				#ifdef NAZARA_BIG_ENDIAN
				SwapBytes(&magic[0], sizeof(UInt32));
				SwapBytes(&magic[1], sizeof(UInt32));
				#endif

				if (magic[0] == md2Ident && magic[1] == 8)
					return Ternary::True;
			}

			return Ternary::False;
		}

		std::shared_ptr<Mesh> LoadMD2(Stream& stream, const MeshParams& parameters)
		{
			MD2_Header header;
			if (stream.Read(&header, sizeof(MD2_Header)) != sizeof(MD2_Header))
			{
				NazaraError("Failed to read header");
				return nullptr;
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
				return nullptr;
			}

			// Since the engine no longer supports keyframe animations, let's make a static mesh
			std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
			if (!mesh->CreateStatic())
			{
				NazaraInternalError("Failed to create mesh");
				return nullptr;
			}

			// Extract skins (texture name)
			if (header.num_skins > 0)
			{
				mesh->SetMaterialCount(header.num_skins);
				stream.SetCursorPos(header.offset_skins);
				{
					std::filesystem::path baseDir = stream.GetDirectory();
					char skin[68];
					for (unsigned int i = 0; i < header.num_skins; ++i)
					{
						stream.Read(skin, 68*sizeof(char));

						ParameterList matData;
						matData.SetParameter(MaterialData::BaseColorTexturePath, (baseDir / skin).generic_u8string());

						mesh->SetMaterialData(i, std::move(matData));
					}
				}
			}

			std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBuffer>(IndexType::U16, 3 * header.num_tris, parameters.indexBufferFlags, parameters.bufferFactory);

			// Extract triangles data
			std::vector<MD2_Triangle> triangles(header.num_tris);

			stream.SetCursorPos(header.offset_tris);
			stream.Read(&triangles[0], header.num_tris*sizeof(MD2_Triangle));

			// And convert them into an index buffer
			BufferMapper<IndexBuffer> indexMapper(*indexBuffer, 0, indexBuffer->GetIndexCount());
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

				// Reverse winding order
				*index++ = triangles[i].vertices[0];
				*index++ = triangles[i].vertices[2];
				*index++ = triangles[i].vertices[1];
			}

			indexMapper.Unmap();

			// Optimize if requested (improves cache locality)
			if (parameters.optimizeIndexBuffers)
				indexBuffer->Optimize();

			// Extracting texture coordinates
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

			std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>(parameters.vertexDeclaration, header.num_vertices, parameters.vertexBufferFlags, parameters.bufferFactory);
			std::shared_ptr<StaticMesh> subMesh = std::make_shared<StaticMesh>(vertexBuffer, indexBuffer);

			// Extracting vertices
			stream.SetCursorPos(header.offset_frames);

			std::vector<MD2_Vertex> vertices(header.num_vertices);
			Vector3f scale, translate;
			stream.Read(&scale, sizeof(Vector3f));
			stream.Read(&translate, sizeof(Vector3f));
			stream.Read(nullptr, 16*sizeof(char)); //< Frame name, unused
			stream.Read(vertices.data(), header.num_vertices*sizeof(MD2_Vertex));

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
			scale *= parameters.vertexScale;

			translate *= ScaleAdjust;
			translate += parameters.vertexOffset;

			// Align the model to our coordinates system
			Quaternionf rotation = EulerAnglesf(-90.f, 90.f, 0.f);
			rotation *= parameters.vertexRotation;

			VertexMapper vertexMapper(*vertexBuffer);

			// Loading texture coordinates
			if (auto uvPtr = vertexMapper.GetComponentPtr<Vector2f>(VertexComponent::TexCoord))
			{
				constexpr std::array<UInt32, 3> indexFix = {0, 2, 1};

				Vector2f invSkinSize(1.f / header.skinwidth, 1.f / header.skinheight);
				for (UInt32 i = 0; i < header.num_tris; ++i)
				{
					for (UInt32 fixedIndex : indexFix) //< Reverse winding order
					{
						const MD2_TexCoord& texC = texCoords[triangles[i].texCoords[fixedIndex]];
						Vector2f uv(texC.u, texC.v);
						uv *= invSkinSize;

						uvPtr[triangles[i].vertices[fixedIndex]].Set(parameters.texCoordOffset + uv * parameters.texCoordScale);
					}
				}
			}

			// Vertex normals
			if (auto normalPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Normal))
			{
				for (UInt32 v = 0; v < header.num_vertices; ++v)
				{
					const MD2_Vertex& vert = vertices[v];

					*normalPtr++ = TransformNormalTRS(rotation, scale, md2Normals[vert.n]);
				}
			}

			// Vertex positions
			if (auto posPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Position))
			{
				for (UInt32 v = 0; v < header.num_vertices; ++v)
				{
					const MD2_Vertex& vert = vertices[v];

					*posPtr++ = TransformPositionTRS(translate, rotation, scale, Vector3f(vert.x, vert.y, vert.z));
				}
			}

			// Vertex colors (.md2 files have no vertex color)
			if (auto colorPtr = vertexMapper.GetComponentPtr<Color>(VertexComponent::Color))
			{
				for (UInt32 v = 0; v < header.num_vertices; ++v)
					*colorPtr++ = Color::White;
			}

			vertexMapper.Unmap();

			subMesh->SetIndexBuffer(std::move(indexBuffer));
			subMesh->SetMaterialIndex(0);

			subMesh->GenerateAABB();

			if (parameters.vertexDeclaration->HasComponentOfType<Vector3f>(VertexComponent::Tangent))
				subMesh->GenerateTangents();

			mesh->AddSubMesh(subMesh);

			if (parameters.center)
				mesh->Recenter();

			return mesh;
		}
	}

	namespace Loaders
	{
		MeshLoader::Entry GetMeshLoader_MD2()
		{
			MeshLoader::Entry loader;
			loader.extensionSupport = IsMD2Supported;
			loader.streamChecker = CheckMD2;
			loader.streamLoader = LoadMD2;

			return loader;
		}
	}
}
