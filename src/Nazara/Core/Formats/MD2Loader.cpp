// Copyright (C) 2024 Jérôme "SirLynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Export.hpp

#include <Nazara/Core/Formats/MD2Loader.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/MaterialData.hpp>
#include <Nazara/Core/Mesh.hpp>
#include <Nazara/Core/StaticMesh.hpp>
#include <Nazara/Core/Stream.hpp>
#include <Nazara/Core/VertexMapper.hpp>
#include <Nazara/Core/Formats/MD2Constants.hpp>
#include <Nazara/Math/Quaternion.hpp>
#include <NazaraUtils/Endianness.hpp>
#include <array>
#include <cassert>
#include <memory>

namespace Nz
{
	namespace
	{
		bool IsMD2Supported(std::string_view extension)
		{
			return (extension == ".md2");
		}

		Result<std::shared_ptr<Mesh>, ResourceLoadingError> LoadMD2(Stream& stream, const MeshParams& parameters)
		{
			MD2_Header header;
			if (stream.Read(&header, sizeof(MD2_Header)) != sizeof(MD2_Header))
				return Err(ResourceLoadingError::Unrecognized);

#ifdef NAZARA_BIG_ENDIAN
			header.ident = ByteSwap(header.ident);
			header.version = ByteSwap(header.version);
#endif

			if (header.ident != md2Ident)
				return Err(ResourceLoadingError::Unrecognized);


			if (header.version != 8)
				return Err(ResourceLoadingError::Unsupported);

#ifdef NAZARA_BIG_ENDIAN
			header.skinwidth = ByteSwap(header.skinwidth);
			header.skinheight = ByteSwap(header.skinheight);
			header.framesize = ByteSwap(header.framesize);
			header.num_skins = ByteSwap(header.num_skins);
			header.num_vertices = ByteSwap(header.num_vertices);
			header.num_st = ByteSwap(header.num_st);
			header.num_tris = ByteSwap(header.num_tris);
			header.num_glcmds = ByteSwap(header.num_glcmds);
			header.num_frames = ByteSwap(header.num_frames);
			header.offset_skins = ByteSwap(header.offset_skins);
			header.offset_st = ByteSwap(header.offset_st);
			header.offset_tris = ByteSwap(header.offset_tris);
			header.offset_frames = ByteSwap(header.offset_frames);
			header.offset_glcmds = ByteSwap(header.offset_glcmds);
			header.offset_end = ByteSwap(header.offset_end);
#endif

			if (stream.GetSize() < header.offset_end)
			{
				NazaraError("incomplete MD2 file");
				return Err(ResourceLoadingError::DecodingError);
			}

			// Since the engine no longer supports keyframe animations, let's make a static mesh
			std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
			if (!mesh->CreateStatic())
			{
				NazaraInternalError("Failed to create mesh");
				return Err(ResourceLoadingError::Internal);
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
						matData.SetParameter(MaterialData::BaseColorTexturePath, PathToString(baseDir / skin));
						matData.SetParameter(MaterialData::Type, "Phong");

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

#ifdef NAZARA_BIG_ENDIAN
			for (UInt32 i = 0; i < header.num_tris; ++i)
			{
				triangles[i].vertices[0] = ByteSwap(triangles[i].vertices[0]);
				triangles[i].texCoords[0] = ByteSwap(triangles[i].texCoords[0]);
				triangles[i].vertices[1] = ByteSwap(triangles[i].vertices[1]);
				triangles[i].texCoords[1] = ByteSwap(triangles[i].texCoords[1]);
				triangles[i].vertices[2] = ByteSwap(triangles[i].vertices[2]);
				triangles[i].texCoords[2] = ByteSwap(triangles[i].texCoords[2]);
			}
#endif

			if (parameters.reverseWinding)
			{
				// Winding order is reversed relative to what the engine expects

				for (UInt32 i = 0; i < header.num_tris; ++i)
				{
					*index++ = triangles[i].vertices[0];
					*index++ = triangles[i].vertices[1];
					*index++ = triangles[i].vertices[2];
				}
			}
			else
			{
				for (UInt32 i = 0; i < header.num_tris; ++i)
				{
					*index++ = triangles[i].vertices[0];
					*index++ = triangles[i].vertices[2];
					*index++ = triangles[i].vertices[1];
				}
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
				texCoords[i].u = ByteSwap(texCoords[i].u);
				texCoords[i].v = ByteSwap(texCoords[i].v);
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
			scale.x = ByteSwap(scale.x);
			scale.y = ByteSwap(scale.y);
			scale.z = ByteSwap(scale.z);

			translate.x = ByteSwap(translate.x);
			translate.y = ByteSwap(translate.y);
			translate.z = ByteSwap(translate.z);
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

						uvPtr[triangles[i].vertices[fixedIndex]] = parameters.texCoordOffset + uv * parameters.texCoordScale;
					}
				}
			}

			// Vertex normals
			if (auto normalPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Normal))
			{
				for (UInt32 v = 0; v < header.num_vertices; ++v)
				{
					const MD2_Vertex& vert = vertices[v];

					*normalPtr++ = TransformDirectionSRT(rotation, scale, md2Normals[vert.n]);
				}
			}

			// Vertex positions
			if (auto posPtr = vertexMapper.GetComponentPtr<Vector3f>(VertexComponent::Position))
			{
				for (UInt32 v = 0; v < header.num_vertices; ++v)
				{
					const MD2_Vertex& vert = vertices[v];

					*posPtr++ = TransformPositionSRT(translate, rotation, scale, Vector3f(vert.x, vert.y, vert.z));
				}
			}

			// Vertex colors (.md2 files have no vertex color)
			if (auto colorPtr = vertexMapper.GetComponentPtr<Color>(VertexComponent::Color))
			{
				for (UInt32 v = 0; v < header.num_vertices; ++v)
					*colorPtr++ = Color::White();
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
			loader.streamLoader = LoadMD2;
			loader.parameterFilter = [](const MeshParams& parameters)
			{
				if (auto result = parameters.custom.GetBooleanParameter("SkipBuiltinMD2Loader"); result.GetValueOr(false))
					return false;

				return true;
			};

			return loader;
		}
	}
}
