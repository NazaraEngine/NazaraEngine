// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Graphics module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Graphics/DepthRenderTechnique.hpp>
#include <Nazara/Core/ErrorFlags.hpp>
#include <Nazara/Core/OffsetOf.hpp>
#include <Nazara/Graphics/AbstractBackground.hpp>
#include <Nazara/Graphics/Camera.hpp>
#include <Nazara/Graphics/Drawable.hpp>
#include <Nazara/Graphics/Light.hpp>
#include <Nazara/Graphics/Material.hpp>
#include <Nazara/Graphics/Sprite.hpp>
#include <Nazara/Renderer/Config.hpp>
#include <Nazara/Renderer/Renderer.hpp>
#include <Nazara/Utility/BufferMapper.hpp>
#include <Nazara/Utility/StaticMesh.hpp>
#include <Nazara/Utility/VertexStruct.hpp>
#include <limits>
#include <memory>
#include <Nazara/Graphics/Debug.hpp>

namespace
{
	struct BillboardPoint
	{
		NzColor color;
		NzVector3f position;
		NzVector2f size;
		NzVector2f sinCos; // must follow `size` (both will be sent as a Vector4f)
		NzVector2f uv;
	};

	static_assert(NzOffsetOf(BillboardPoint, sinCos) - NzOffsetOf(BillboardPoint, size) == sizeof(NzVector2f), "size and sinCos members should be packed");

	unsigned int s_maxQuads = std::numeric_limits<nzUInt16>::max()/6;
	unsigned int s_vertexBufferSize = 4*1024*1024; // 4 MiB
}

NzDepthRenderTechnique::NzDepthRenderTechnique() :
m_vertexBuffer(nzBufferType_Vertex)
{
	NzErrorFlags flags(nzErrorFlag_ThrowException, true);

	m_vertexBuffer.Create(s_vertexBufferSize, nzDataStorage_Hardware, nzBufferUsage_Dynamic);

	m_billboardPointBuffer.Reset(&s_billboardVertexDeclaration, &m_vertexBuffer);
	m_spriteBuffer.Reset(NzVertexDeclaration::Get(nzVertexLayout_XYZ_Color_UV), &m_vertexBuffer);
}

bool NzDepthRenderTechnique::Draw(const NzSceneData& sceneData) const
{
	NzRenderer::Enable(nzRendererParameter_DepthBuffer, true);
	NzRenderer::Enable(nzRendererParameter_DepthWrite, true);
	NzRenderer::Clear(nzRendererBuffer_Depth);

	// Just in case the background does render depth
	if (sceneData.background)
		sceneData.background->Draw(sceneData.viewer);

	if (!m_renderQueue.meshes.empty())
		DrawOpaqueModels(sceneData);

	if (!m_renderQueue.basicSprites.empty())
		DrawBasicSprites(sceneData);

	if (!m_renderQueue.billboards.empty())
		DrawBillboards(sceneData);

	// Other custom drawables
	for (const NzDrawable* drawable : m_renderQueue.otherDrawables)
		drawable->Draw();

	return true;
}

NzAbstractRenderQueue* NzDepthRenderTechnique::GetRenderQueue()
{
	return &m_renderQueue;
}

nzRenderTechniqueType NzDepthRenderTechnique::GetType() const
{
	return nzRenderTechniqueType_BasicForward;
}

bool NzDepthRenderTechnique::Initialize()
{
	try
	{
		NzErrorFlags flags(nzErrorFlag_ThrowException, true);

		s_quadIndexBuffer.Reset(false, s_maxQuads*6, nzDataStorage_Hardware, nzBufferUsage_Static);

		NzBufferMapper<NzIndexBuffer> mapper(s_quadIndexBuffer, nzBufferAccess_WriteOnly);
		nzUInt16* indices = static_cast<nzUInt16*>(mapper.GetPointer());

		for (unsigned int i = 0; i < s_maxQuads; ++i)
		{
			*indices++ = i*4 + 0;
			*indices++ = i*4 + 2;
			*indices++ = i*4 + 1;

			*indices++ = i*4 + 2;
			*indices++ = i*4 + 3;
			*indices++ = i*4 + 1;
		}

		mapper.Unmap(); // Inutile de garder le buffer ouvert plus longtemps

		// Quad buffer (utilisé pour l'instancing de billboard et de sprites)
		//Note: Les UV sont calculés dans le shader
		s_quadVertexBuffer.Reset(NzVertexDeclaration::Get(nzVertexLayout_XY), 4, nzDataStorage_Hardware, nzBufferUsage_Static);

		float vertices[2*4] = {
		   -0.5f, -0.5f,
			0.5f, -0.5f,
		   -0.5f, 0.5f,
			0.5f, 0.5f,
		};

		s_quadVertexBuffer.FillRaw(vertices, 0, sizeof(vertices));

		// Déclaration lors du rendu des billboards par sommet
		s_billboardVertexDeclaration.EnableComponent(nzVertexComponent_Color,     nzComponentType_Color,  NzOffsetOf(BillboardPoint, color));
		s_billboardVertexDeclaration.EnableComponent(nzVertexComponent_Position,  nzComponentType_Float3, NzOffsetOf(BillboardPoint, position));
		s_billboardVertexDeclaration.EnableComponent(nzVertexComponent_TexCoord,  nzComponentType_Float2, NzOffsetOf(BillboardPoint, uv));
		s_billboardVertexDeclaration.EnableComponent(nzVertexComponent_Userdata0, nzComponentType_Float4, NzOffsetOf(BillboardPoint, size)); // Englobe sincos

		// Declaration utilisée lors du rendu des billboards par instancing
		// L'avantage ici est la copie directe (std::memcpy) des données de la RenderQueue vers le buffer GPU
		s_billboardInstanceDeclaration.EnableComponent(nzVertexComponent_InstanceData0, nzComponentType_Float3, NzOffsetOf(NzForwardRenderQueue::BillboardData, center));
		s_billboardInstanceDeclaration.EnableComponent(nzVertexComponent_InstanceData1, nzComponentType_Float4, NzOffsetOf(NzForwardRenderQueue::BillboardData, size)); // Englobe sincos
		s_billboardInstanceDeclaration.EnableComponent(nzVertexComponent_InstanceData2, nzComponentType_Color,  NzOffsetOf(NzForwardRenderQueue::BillboardData, color));

		// Material
		s_material = NzMaterial::New();
		s_material->Enable(nzRendererParameter_ColorWrite, false);
		s_material->Enable(nzRendererParameter_FaceCulling, false);
	}
	catch (const std::exception& e)
	{
		NazaraError("Failed to initialise: " + NzString(e.what()));
		return false;
	}

	return true;
}

void NzDepthRenderTechnique::Uninitialize()
{
	s_material.Reset();
	s_quadIndexBuffer.Reset();
	s_quadVertexBuffer.Reset();
}

void NzDepthRenderTechnique::DrawBasicSprites(const NzSceneData& sceneData) const
{
	NzRenderer::SetIndexBuffer(&s_quadIndexBuffer);
	NzRenderer::SetMatrix(nzMatrixType_World, NzMatrix4f::Identity());
	NzRenderer::SetVertexBuffer(&m_spriteBuffer);

	auto& spriteChainVector = m_renderQueue.basicSprites;
	unsigned int spriteChainCount = spriteChainVector.size();
	if (spriteChainCount > 0)
	{
		s_material->Apply();

		unsigned int spriteChain = 0; // Quelle chaîne de sprite traitons-nous
		unsigned int spriteChainOffset = 0; // À quel offset dans la dernière chaîne nous sommes-nous arrêtés

		do
		{
			// On ouvre le buffer en écriture
			NzBufferMapper<NzVertexBuffer> vertexMapper(m_spriteBuffer, nzBufferAccess_DiscardAndWrite);
			NzVertexStruct_XYZ_Color_UV* vertices = reinterpret_cast<NzVertexStruct_XYZ_Color_UV*>(vertexMapper.GetPointer());

			unsigned int spriteCount = 0;
			unsigned int maxSpriteCount = std::min(s_maxQuads, m_spriteBuffer.GetVertexCount()/4);

			do
			{
				NzDepthRenderQueue::SpriteChain_XYZ_Color_UV& currentChain = spriteChainVector[spriteChain];
				unsigned int count = std::min(maxSpriteCount - spriteCount, currentChain.spriteCount - spriteChainOffset);

				std::memcpy(vertices, currentChain.vertices + spriteChainOffset*4, 4*count*sizeof(NzVertexStruct_XYZ_Color_UV));
				vertices += count*4;

				spriteCount += count;
				spriteChainOffset += count;

				// Avons-nous traité la chaîne entière ?
				if (spriteChainOffset == currentChain.spriteCount)
				{
					spriteChain++;
					spriteChainOffset = 0;
				}
			}
			while (spriteCount < maxSpriteCount && spriteChain < spriteChainCount);

			vertexMapper.Unmap();

			NzRenderer::DrawIndexedPrimitives(nzPrimitiveMode_TriangleList, 0, spriteCount*6);
		}
		while (spriteChain < spriteChainCount);

		spriteChainVector.clear();
	}
}

void NzDepthRenderTechnique::DrawBillboards(const NzSceneData& sceneData) const
{
	if (NzRenderer::HasCapability(nzRendererCap_Instancing))
	{
		NzVertexBuffer* instanceBuffer = NzRenderer::GetInstanceBuffer();
		instanceBuffer->SetVertexDeclaration(&s_billboardInstanceDeclaration);

		NzRenderer::SetVertexBuffer(&s_quadVertexBuffer);

		auto& billboardVector = m_renderQueue.billboards;
		unsigned int billboardCount = billboardVector.size();
		if (billboardCount > 0)
		{
			s_material->Apply(nzShaderFlags_Billboard | nzShaderFlags_Instancing);

			const NzDepthRenderQueue::BillboardData* data = &billboardVector[0];
			unsigned int maxBillboardPerDraw = instanceBuffer->GetVertexCount();
			do
			{
				unsigned int renderedBillboardCount = std::min(billboardCount, maxBillboardPerDraw);
				billboardCount -= renderedBillboardCount;

				instanceBuffer->Fill(data, 0, renderedBillboardCount, true);
				data += renderedBillboardCount;

				NzRenderer::DrawPrimitivesInstanced(renderedBillboardCount, nzPrimitiveMode_TriangleStrip, 0, 4);
			}
			while (billboardCount > 0);

			billboardVector.clear();
		}
	}
	else
	{
		NzRenderer::SetIndexBuffer(&s_quadIndexBuffer);
		NzRenderer::SetVertexBuffer(&m_billboardPointBuffer);

		auto& billboardVector = m_renderQueue.billboards;

		unsigned int billboardCount = billboardVector.size();
		if (billboardCount > 0)
		{
			s_material->Apply(nzShaderFlags_Billboard);

			const NzDepthRenderQueue::BillboardData* data = &billboardVector[0];
			unsigned int maxBillboardPerDraw = std::min(s_maxQuads, m_billboardPointBuffer.GetVertexCount()/4);

			do
			{
				unsigned int renderedBillboardCount = std::min(billboardCount, maxBillboardPerDraw);
				billboardCount -= renderedBillboardCount;

				NzBufferMapper<NzVertexBuffer> vertexMapper(m_billboardPointBuffer, nzBufferAccess_DiscardAndWrite, 0, renderedBillboardCount*4);
				BillboardPoint* vertices = reinterpret_cast<BillboardPoint*>(vertexMapper.GetPointer());

				for (unsigned int i = 0; i < renderedBillboardCount; ++i)
				{
					const NzDepthRenderQueue::BillboardData& billboard = *data++;

					vertices->color = billboard.color;
					vertices->position = billboard.center;
					vertices->sinCos = billboard.sinCos;
					vertices->size = billboard.size;
					vertices->uv.Set(0.f, 1.f);
					vertices++;

					vertices->color = billboard.color;
					vertices->position = billboard.center;
					vertices->sinCos = billboard.sinCos;
					vertices->size = billboard.size;
					vertices->uv.Set(1.f, 1.f);
					vertices++;

					vertices->color = billboard.color;
					vertices->position = billboard.center;
					vertices->sinCos = billboard.sinCos;
					vertices->size = billboard.size;
					vertices->uv.Set(0.f, 0.f);
					vertices++;

					vertices->color = billboard.color;
					vertices->position = billboard.center;
					vertices->sinCos = billboard.sinCos;
					vertices->size = billboard.size;
					vertices->uv.Set(1.f, 0.f);
					vertices++;
				}

				vertexMapper.Unmap();

				NzRenderer::DrawIndexedPrimitives(nzPrimitiveMode_TriangleList, 0, renderedBillboardCount*6);
			}
			while (billboardCount > 0);

			billboardVector.clear();
		}
	}
}

void NzDepthRenderTechnique::DrawOpaqueModels(const NzSceneData& sceneData) const
{
	s_material->Apply();

	for (auto& meshIt : m_renderQueue.meshes)
	{
		const NzMeshData& meshData = meshIt.first;
		auto& meshEntry = meshIt.second;

		std::vector<NzMatrix4f>& instances = meshEntry.instances;
		if (!instances.empty())
		{
			const NzIndexBuffer* indexBuffer = meshData.indexBuffer;
			const NzVertexBuffer* vertexBuffer = meshData.vertexBuffer;

			// Gestion du draw call avant la boucle de rendu
			NzRenderer::DrawCall drawFunc;
			NzRenderer::DrawCallInstanced instancedDrawFunc;
			unsigned int indexCount;

			if (indexBuffer)
			{
				drawFunc = NzRenderer::DrawIndexedPrimitives;
				instancedDrawFunc = NzRenderer::DrawIndexedPrimitivesInstanced;
				indexCount = indexBuffer->GetIndexCount();
			}
			else
			{
				drawFunc = NzRenderer::DrawPrimitives;
				instancedDrawFunc = NzRenderer::DrawPrimitivesInstanced;
				indexCount = vertexBuffer->GetVertexCount();
			}

			NzRenderer::SetIndexBuffer(indexBuffer);
			NzRenderer::SetVertexBuffer(vertexBuffer);

			// Sans instancing, on doit effectuer un draw call pour chaque instance
			// Cela reste néanmoins plus rapide que l'instancing en dessous d'un certain nombre d'instances
			// À cause du temps de modification du buffer d'instancing
			for (const NzMatrix4f& matrix : instances)
			{
				NzRenderer::SetMatrix(nzMatrixType_World, matrix);
				drawFunc(meshData.primitiveMode, 0, indexCount);
			}
			instances.clear();
		}
	}
}

NzIndexBuffer NzDepthRenderTechnique::s_quadIndexBuffer;
NzMaterialRef NzDepthRenderTechnique::s_material;
NzVertexBuffer NzDepthRenderTechnique::s_quadVertexBuffer;
NzVertexDeclaration NzDepthRenderTechnique::s_billboardInstanceDeclaration;
NzVertexDeclaration NzDepthRenderTechnique::s_billboardVertexDeclaration;
